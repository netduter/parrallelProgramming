#include <fstream>
#include <iomanip>
#include <iostream>
#include <omp.h>

#include "exercise3.hpp"

using namespace std;

double Exercise3::calculate_percentage(string &seq) {
    int gc = 0;
    int at = 0;

    for(string::iterator it = seq.begin(); it != seq.end(); it++) {
        if((*it) == 'A' || (*it) == 'T') {
            at++;
        } else {
            gc++;
        }
    }

    return gc * 1.0 / (at + gc);
}

void Exercise3::fill_sequences_and_ids() {
    std::ifstream infile(input_file.c_str());

    string dummy;
    string id;
    string sequence;

    while(getline(infile, id)) {
        ids.push_back(id);
        getline(infile, sequence);
        dna_sequences.push_back(sequence);
        getline(infile, dummy);
        getline(infile, dummy);
    }

    infile.close();
}

void Exercise3::parallel_calculate() {
    int chunk = ids.size() / omp_get_num_threads();
    int start = omp_get_thread_num() * chunk;
    int end = start + chunk;

    if(omp_get_thread_num() == omp_get_num_threads() - 1) {
        end = ids.size();
    }

    for(int i = start; i < end; i++) {
        results.at(i) = calculate_percentage(dna_sequences.at(i));
    }
}

Exercise3::Exercise3(int argc, char **argv) {
    if(argc < 4) {
        throw Exercise3Exception(
            "Δεν είναι σωστός ο αριθμός των ορισμάτων.\nΑπαιτούνται 3:"
            " το πρώτο για το αρχείο εισόδου, το δεύτερο για το αρχείο"
            " εξόδου, και το τρίτο για τον αριθμό των threads");
    }

    input_file = string(argv[1]);
    output_file = string(argv[2]);
    fill_sequences_and_ids();

    results = vector<double>(ids.size());

    omp_set_num_threads(stoi(argv[3]));

    #pragma omp parallel
    parallel_calculate();

    ofstream outputFile(output_file.c_str());
    cout << std::fixed << std::showpoint;
    cout << std::setprecision(3);

    outputFile << std::fixed << std::showpoint;
    outputFile << std::setprecision(3);

    for(int i = 0; i < ids.size(); i++) {
        cout << ids.at(i) << endl << results.at(i) << endl;
        outputFile << ids.at(i) << endl << results.at(i) << endl;
    }

    outputFile.close();
}

Exercise3Exception::Exercise3Exception(string exception) {
    whatHappened = exception;
}

const char *Exercise3Exception::what() { return whatHappened.c_str(); }
