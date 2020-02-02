#ifndef __EXERCISE_3__
#define __EXERCISE_3__

#include <string>
#include <vector>

using namespace std;

class Exercise3 {
    public:
    Exercise3(int argc, char **argv);

    private:
    void fill_sequences_and_ids();
    double calculate_percentage(string &sequence);
    void parallel_calculate();
    string input_file;
    string output_file;
    vector<string> dna_sequences;
    vector<string> ids;
    vector<double> results;
};

class Exercise3Exception : exception {
    public:
    Exercise3Exception(string);
    const char *what();

    private:
    string whatHappened;
};

#endif  // __EXERCISE_3__
