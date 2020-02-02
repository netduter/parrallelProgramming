#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_LENGTH 20
#define LINE_LENGTH 300

/**
 * Count the number of lines in a file.
 *
 * @param fp The file pointer to read from.
 * @return The number of lines in the file.
 */
int count_lines(FILE *fp) {
    int cnt = 0;
    size_t length;
    char *tmp = malloc(LINE_LENGTH);
    while(getline(&tmp, &length, fp) != EOF) ++cnt;
    free(tmp);
    return cnt;
}

/**
 * Fill sequence IDs from a FASTQ file.
 *
 * @param fname The name of the file to read from.
 * @param ids The string that will be filled.
 */
void fill_ids(char *file, char *ids) {
    FILE *fin = fopen(file, "r");
    int ln_cnt = 0, id_cnt = 0;
    char *buffer = malloc(LINE_LENGTH);
    size_t len;

    while(getline(&buffer, &len, fin) != EOF) {
        if(ln_cnt++ % 4 == 0) {
            buffer[strlen(buffer) - 1] = '\0';
            memcpy(ids + ID_LENGTH * id_cnt++, buffer, ID_LENGTH);
        }
    }
    free(buffer);
}

/**
 * Calculate the GC content of a sequence.
 *
 * @param sequence A sequence of nucleotides.
 * @return The ratio of G or C nucleotides.
 */
double get_gc_content(char *sequence) {
    int gc = 0, at = 0;

    for(size_t i = 0; i < strlen(sequence); ++i) {
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

    return (gc + at > 0) ? 1.0 * gc / (gc + at) : -1.0;
}

/**
 * Create a message that will be sent through MPI.
 *
 * @param message The message that will be created.
 * @param fname The name of a file to read from.
 */
void create_message(char *message, char *fname) {
    FILE *fin = fopen(fname, "r");
    char *buffer = malloc(LINE_LENGTH);
    int cntr = 0;
    size_t size;

    while(getline(&buffer, &size, fin) != EOF) {
        getline(&buffer, &size, fin);
        buffer[strlen(buffer) - 1] = '\0';
        memcpy(message + cntr++ * LINE_LENGTH, buffer, LINE_LENGTH);

        // skip the next two lines
        getline(&buffer, &size, fin);
        getline(&buffer, &size, fin);
    }

    fclose(fin);
    free(buffer);
}

/**
 * Send the calculated results using MPI_Send.
 *
 * @param buffer A buffer of sequences.
 * @param lines The number of lines in the file.
 * @param rank The rank of the current process.
 * @param nprocs The number of process.
 * @param gc_cnt A counter of GC ratios.
 */
void send_results(char *const buffer, int lines,
                  int rank, int nprocs, int gc_cnt) {
    char *start = buffer + lines / 4 / nprocs * rank * LINE_LENGTH;
    char *end = rank == nprocs - 1 ? \
                buffer + lines / 4 * LINE_LENGTH : \
                start + lines / 4 / nprocs * LINE_LENGTH;
    double *res = malloc(gc_cnt * sizeof (double));
    memset(res, 0, gc_cnt * sizeof (double));

    for(int cntr = 0; start < end; start += LINE_LENGTH)
        res[cntr++] = get_gc_content(start);

    MPI_Send(res, gc_cnt, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    free(res);
}

/**
 * Calculate the GC content of some sequences.
 *
 * @param buffer A buffer containing the sequences.
 * @param ids The IDs of the sequences.
 * @param nprocs The number of processes.
 * @param lines The number of lines in the input file.
 * @param fname The file name to write to.
 */
void calculate(char *buffer, char *ids, int nprocs, int lines, char *fname) {
    FILE *fout = fopen(fname, "w");
    char tmp[ID_LENGTH] = {0};

    for(int i = 0, n = lines / 4 / nprocs; i < n; ++i) {
        memcpy(tmp, ids + ID_LENGTH * i, ID_LENGTH);
        fprintf(fout, "%s\t%0.9lf\n", tmp,
                get_gc_content(buffer + LINE_LENGTH * i));
        memset(tmp, 0, ID_LENGTH);
    }
    fclose(fout);
}

/**
 * Write the calculated results to a file.
 *
 * @param ids The IDs of the sequences.
 * @param res The GC content results.
 * @param fname The file name to write to.
 * @param rank The rank of the current process.
 * @param nprocs The number of process.
 * @param lines The number of lines in the file.
 * @param gc_cnt A counter of GC ratios.
 */
void write_results(char *ids, double *res, char *fname,
                   int rank, int nprocs, int lines, int gc_cnt) {
    FILE *fout = fopen(fname, "a");
    char tmp[ID_LENGTH] = {0};
    char *start = ids + lines / 4 / nprocs * ID_LENGTH * rank;

    for(int i = 0; i < gc_cnt && res[i] != 0; ++i) {
        memcpy(tmp, start + ID_LENGTH * i, ID_LENGTH);
        fprintf(fout, "%s\t%0.9lf\n", tmp, res[i]);
        memset(tmp, 0, ID_LENGTH);
    }

    fclose(fout);
}

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "usage: <input> <output>\n");
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if(fin == NULL) {
        fprintf(stderr, "File '%s' not found!\n", argv[1]);
        return 2;
    }
    int lines = count_lines(fin);
    fclose(fin);

    int rank, nprocs;
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

        create_message(message, argv[1]);

        MPI_Bcast(message, lines / 4 * LINE_LENGTH,
                  MPI_CHAR, 0, MPI_COMM_WORLD);
        calculate(message, ids, nprocs, lines, argv[2]);

        for(int i = 0; i < nprocs - 1; ++i) {
            MPI_Recv(results, gc_cnt, MPI_DOUBLE,
                     i + 1, 0, MPI_COMM_WORLD, &stat);
            write_results(ids, results, argv[2], i + 1, nprocs, lines, gc_cnt);
        }
        free(ids);
        free(results);
    } else {
        MPI_Bcast(message, lines / 4 * LINE_LENGTH,
                  MPI_CHAR, 0, MPI_COMM_WORLD);
        send_results(message, lines, rank, nprocs, gc_cnt);
    }

    MPI_Finalize();
    free(message);
    return 0;
}
