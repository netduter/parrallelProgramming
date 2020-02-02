#include "exercise1.hpp"

Exercise1::Exercise1(int argc, char **argv) {
    files.resize(1);

    if(argc < 3) {
        throw Exercise1Exception(
            "Χρειάζεται να δώσετε 2 ορίσματα στο command line.\nΤο πρώτο "
            "για το αρχείο εισόδου και το δεύτερο για το αρχείο εξόδου");
    }

    files.push_back(argv[1]);
    outputFile = string(argv[2]);
}


void Exercise1::readFile() {
    string line;
    string id;
    string cmd;

    int sequence_length = -1, G, C;
    int sequences;

    int max_len = 0;
    int cntr = 0;
    int sum = 0;
    int min_len = 5000;

    smatch idmatch, dna_sequence_match, dna_sequence_values_match;
    regex sequence_id("^@HUA:([0-9]*:*)+\\/[0-9]$");
    // επιτρεπτές τιμές αλληλουχίας dna
    regex dna_sequence("^[ATGC]+$");
    // Αξιολόγηση της ποιότητας του dna
    regex dna_sequence_values("^[!-I]+");

    for(vector<string>::iterator it = files.begin(); it != files.end(); it++) {
        ifstream infile((*it).c_str());
        ofstream outputfile(outputFile, ios::out);
        outputfile.precision(3);
        outputfile << std::fixed;
        sequences = 0;

        while(infile.is_open() && getline(infile, line)) {
            // Αναζήτηση της πληροφορίας που με ενδιαφέρει με regex
            regex_match(line, idmatch, sequence_id);

            if(idmatch.size() > 0) { // id
                sequences++;
                id = line;
            }

            regex_match(line, dna_sequence_match, dna_sequence);

            if(dna_sequence_match.size() > 0) {
                if(max_len < line.length()) {
                    max_len = line.length();
                }
                if(line.length() < min_len) {
                    min_len = line.length();
                }

                sum += line.length();
                cntr++;
                sequence_length = line.length();
                G = C = 0;

                for(string::iterator it = line.begin(); it != line.end();
                    it++) {
                    if(*it == 'G') {
                        G++;
                    }
                    if(*it == 'C') {
                        C++;
                    }
                }

                continue;
            }

            regex_match(line, dna_sequence_values_match, dna_sequence_values);

            if(dna_sequence_values_match.size() > 0 &&
               sequence_length != line.length() && line.length() > 1) {
                // ασυμφωνία αριθμού βάσεων με βαθμολογία αξιοπιστίας
                infile.close();
                outputfile.close();
                string msg = "Πρόβλημα στο αρχείο " + *it +
                             " Ασυμφωνία αριθμού βάσεων με αριθμό αξιολογήσεων"
                             " στην αξιολόγηση της έλεγχος αρχείου";
                throw Exercise1Exception(msg);
            }

            if(sequence_length == line.length()) {
                // Έχουμε  φτάσει στην γραμμή που αντιστοιχίζεται
                // η βάση dna με το ποσοστό αξιοπιστίας
                cout << id << endl
                     << (G + C) * 100.0 / line.length() << "%" << endl;
                outputfile << id << endl
                           << (G + C) * 100.0 / line.length() << "%" << endl;
            }
        }

        infile.close();
        outputfile.close();
    }
}

Exercise1Exception::Exercise1Exception(string message) {
    whatHappened = message;
}

const char *Exercise1Exception::what() { return whatHappened.c_str(); }
