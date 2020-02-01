#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_LENGTH 20
#define LINE_LENGTH 300

int count_lines(FILE *fp) {
    int cnt = 0;
    size_t length;
    char *tmp = malloc(LINE_LENGTH);
    while(getline(&tmp, &length, fp) != EOF) ++cnt;
    free(tmp);
    return cnt;
}

void fill_ids(char *fname, char *ids) {
    size_t len;
    FILE *fin = fopen(fname, "r");
    char *buffer = malloc(LINE_LENGTH);
    int ln_cnt = 0, id_cnt = 0;
    while(getline(&buffer, &len, fin) != EOF) {
        if(ln_cnt++ % 4 == 0) {
            buffer[strlen(buffer) - 1] = '\0';
            memcpy(ids + ID_LENGTH * id_cnt++, buffer, ID_LENGTH);
        }
    }
    free(buffer);
}

double get_gc_content(char *sequence) {
    size_t i;
    int gc = 0, at = 0;

    for(i = 0; i < strlen(sequence); ++i) {
        switch(sequence[i]) {
            case 'G':
            case 'C':
                ++gc;
                break;
            case 'A':
            case 'T':
                ++at;
                break;
        }
    }

    return at + gc <= 0 ? -1.0 : 1.0 * gc / (at + gc);
}

void create_message(char *fname, char *message) {
    size_t size;
    int cntr = 0;
    FILE *fin = fopen(fname, "r");
    char *buffer = malloc(LINE_LENGTH);
    while(getline(&buffer, &size, fin) != EOF) {
        getline(&buffer, &size, fin);
        buffer[strlen(buffer) - 1] = '\0';
        memcpy(message + cntr++ * LINE_LENGTH, buffer, LINE_LENGTH);

        // skip the next two lines
        getline(&buffer, &size, fin);
        getline(&buffer, &size, fin);
    }
    free(buffer);
    fclose(fin);
}

void send_results(char *const buffer, int lines, int rank,
                  int nprocs, int gc_cnt) {
    int seqs = lines / 4;
    double *res = malloc(gc_cnt * sizeof (double));
    memset(res, 0, gc_cnt * sizeof (double));
    char *start = buffer + seqs / nprocs * rank * LINE_LENGTH;
    char *end = rank == nprocs - 1 ? buffer + seqs * LINE_LENGTH :
                                     start + seqs / nprocs * LINE_LENGTH;

    for(int cntr = 0; start < end; start += LINE_LENGTH)
        res[cntr++] = get_gc_content(start);

    MPI_Send(res, gc_cnt, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    free(res);
}

void calculate(char *message, char *ids, int nprocs, int lines, char *fname) {
    char tmp[20] = {0};
    FILE *fout = fopen(fname, "w");
    int seqs = lines / (4 * nprocs);

    double gc_content;
    for(int i = 0; i < seqs; ++i) {
        gc_content = get_gc_content(message + LINE_LENGTH * i);
        memcpy(tmp, ids + ID_LENGTH * i, ID_LENGTH);
        fprintf(fout, "%s\t%0.9lf\n", tmp, gc_content);
        memset(tmp, 0, sizeof tmp);
    }
    fclose(fout);
}

void write_results(char *ids, double *res, char *fname, int rank, int nprocs,
                   int lines) {
    char tmp[20] = {0};
    FILE *fout = fopen(fname, "a");
    int seqs = lines / 4, chunk = seqs / nprocs;
    if(rank == nprocs - 1) chunk += seqs % nprocs;

    for(int i = 0; i < chunk; ++i) {
        memcpy(tmp, ids + rank * ID_LENGTH * (i + chunk), ID_LENGTH);
        fprintf(fout, "%s\t%0.9lf\n", tmp, res[i]);
        memset(tmp, 0, sizeof tmp);
    }

    fclose(fout);
}

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "usage: <input> <output>\n");
        return 1;
    }

    int rank, nprocs;
    FILE *fin = fopen(argv[1], "r");
    if(fin == NULL) {
        fprintf(stderr, "File '%s' not found!\n", argv[1]);
        return 2;
    }
    int lines = count_lines(fin);
    fclose(fin);

    MPI_Status stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    char *message = malloc(lines / 4 * LINE_LENGTH);

    // add the remainder if the lines of the file
    // cannot be divided by the number of processes
    int gc_cnt = lines / 4 + (lines / 4) % nprocs;

    if(rank == 0) {
        double *results = malloc(gc_cnt * LINE_LENGTH * sizeof (double));
        memset(results, 0, lines / 4 * LINE_LENGTH * sizeof (double));
        char *ids = malloc((lines / 4 + (lines / 4) % nprocs) * ID_LENGTH);
        fill_ids(argv[1], ids);
        create_message(argv[1], message);

        MPI_Bcast(message, lines / 4 * LINE_LENGTH,
                  MPI_CHAR, 0, MPI_COMM_WORLD);
        calculate(message, ids, nprocs, lines, argv[2]);

        for(int i = 0; i < nprocs - 1; ++i) {
            MPI_Recv(results, gc_cnt, MPI_DOUBLE,
                     i + 1, 0, MPI_COMM_WORLD, &stat);
            write_results(ids, results, argv[2],
                          i + 1, nprocs, lines);
        }
        free(ids);
        free(results);

    } else {
        MPI_Bcast(message, lines / 4 * LINE_LENGTH,
                  MPI_CHAR, 0, MPI_COMM_WORLD);
        send_results(message, lines, rank, nprocs, gc_cnt);
    }

    MPI_Finalize();
    return 0;
}
