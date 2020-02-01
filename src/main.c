#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_LENGTH 20
#define LINE_LENGTH 300

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "usage: <input> <output>\n");
        return 1;
    }

    FILE *fin, *fout;
    // Open the input file given from CLI for input
    if((fin = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "File '%s' not found!\n", argv[1]);
        return 2;
    }
    // Open the output file given from CLI for output
    fout = fopen(argv[2], "w");

    char id[ID_LENGTH];
    char sequence[LINE_LENGTH];
    char quality[LINE_LENGTH];

    int i, cntr;
    while(fgets(id, LINE_LENGTH, fin)) {
        id[strlen(id) - 1] = '\0';
        fgets(sequence, LINE_LENGTH, fin);
        fseek(fin, 2, SEEK_CUR);
        fgets(quality, LINE_LENGTH, fin);

        cntr = 0;
        for(i = 0; i < isalpha(sequence[i]); ++i) {
            if(sequence[i] == 'G' || sequence[i] == 'C') ++cntr;
        }

        fprintf(fout, "%s\t%0.9lf\n", id,
                cntr / (double) (strlen(sequence) - 1));
    }

    // close the files
    fclose(fin);
    fclose(fout);
    return 0;
}
