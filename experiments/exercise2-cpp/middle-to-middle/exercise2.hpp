#ifndef __EXERCISE_2__
#define __EXERCISE_2__

#include <fstream>
#include <iostream>
#include <mpi.h>
#include <set>

using namespace std;

class Exercise2 {
    public:
    Exercise2(int, char **);
    ~Exercise2();

    private:
    ofstream fout;
    char *msg_buffer;
    string input_file;
    string output_file;
    const int MAX_BUFFER_SIZE = 1000;
    int rank;
    int nprocs;
    void read_file();
    void wait_for_messages();
    void print_calculated_results();
    int DEFAULT_TAG = 1;
    MPI_Status stat;
    const string FINISHED = "FINISHED";
    set<string> ids;
    const int MINIMUM_DNA_SEQUENCE_LENGTH = 50;
    int how_many_times_print_calculated_results = 0;
};


class Exercise2Exception : exception {
    public:
    Exercise2Exception(string);
    const char *what();

    private:
    string whatHappened;
};

#endif  // __EXERCISE_2__
