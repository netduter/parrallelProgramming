#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_LENGTH 20
#define LINE_LENGTH 300

#ifndef NPROC
#define NPROC (-1)
#endif /* ifndef NPROC */

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
 * Calculate the GC content of a sequence.
 *
 * @param sequence A sequence of nucleotides.
 * @return The ratio of G or C nucleotides.
 */
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

/**
 * Calculate sequences in parallel.
 *
 * @param fname The name of a file to read from.
 * @param lines The number of lines of the file.
 */
void calculate_sequences(char *fname, int lines) {
    int curr = 0, seqns = lines / 4,
        num = omp_get_thread_num(),
        threads = omp_get_num_threads(),
        start = seqns / threads * num * 4,
        end = threads - 1 == num ? lines : \
              start + seqns / threads * 4;

    FILE *fin = fopen(fname, "r");
    char *outfile = malloc(32);
    snprintf(outfile, 32, "results%d.txt", num);
    FILE *fout = fopen(outfile, "w");

    size_t length;
    char *id = malloc(ID_LENGTH);
    char *buffer = malloc(LINE_LENGTH);

    while(curr++ != start)
        getline(&buffer, &length, fin);

    for(; curr < end; curr += 4) {
        getline(&id, &length, fin);
        id[strlen(id) - 1] = '\0';

        getline(&buffer, &length, fin);
        fprintf(fout, "%s\t%0.9lf\n", id, get_gc_content(buffer));

        // skip next lines
        getline(&buffer, &length, fin);
        getline(&buffer, &length, fin);
    }

    fclose(fin);
    fclose(fout);
    free(id);
    free(buffer);
    free(outfile);
}

/**
 * Merge the temporary files into one.
 *
 * @param fname The name of the file to write to.
 * @param threads The number of threads available.
 */
void merge_files(char *fname, int threads) {
    size_t len;
    char *tmp = malloc(32);
    char *buffer = malloc(LINE_LENGTH);
    FILE *fout = fopen(fname, "w");

    for(int i = 0; i < threads; ++i) {
        snprintf(tmp, 32, "results%d.txt", i);
        FILE *fin = fopen(tmp, "r");

        while(getline(&buffer, &len, fin) != EOF)
            fprintf(fout, "%s", buffer);

        fclose(fin);
        remove(tmp);
    }

    fclose(fout);
    free(buffer);
    free(tmp);
}

int main(int argc, char** argv){
    if(argc < 3) {
        fprintf(stderr, "usage: <input> <output> [threads]\n");
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if(fin == NULL) {
        fprintf(stderr, "File '%s' not found!\n", argv[1]);
        return 2;
    }
    int lines = count_lines(fin);
    fclose(fin);

    int threads = argc < 4 ? NPROC : \
                  (int) strtol(argv[3], NULL, 10);
    if(threads < 0) {
        fprintf(stderr, "Threads can't be <0 (%d).", threads);
        return 3;
    }
    omp_set_num_threads(threads);

    #pragma omp parallel
    calculate_sequences(argv[1], lines);

    merge_files(argv[2], threads);
    return 0;
}
