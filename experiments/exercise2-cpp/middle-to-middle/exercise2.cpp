#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>

#include "exercise2.hpp"

void Exercise2::print_calculated_results() {
    for(int i = 0; i < nprocs - 1; i++)
        MPI_Recv(msg_buffer, MAX_BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE,
                 DEFAULT_TAG, MPI_COMM_WORLD, &stat);

    string res(msg_buffer);
    string id;
    string percentage;

    smatch m;
    regex e(".{18}\\d\\.\\d{3}");

    while(std::regex_search(res, m, e)) {
        id = string(m.str(0)).substr(0, 18);
        percentage = string(m.str(0)).substr(18, 24);

        if(ids.find(id) != ids.end()) {
            cout << id << endl << percentage << endl;
            fout << id << endl << percentage << endl;
            ids.erase(id);
        }

        res = m.suffix().str();
    }
}


void Exercise2::read_file() {
    ifstream infile(input_file.c_str());
    ofstream outputfile(output_file.c_str(), ios::out);
    outputfile.precision(3);
    outputfile << std::fixed;

    smatch idmatch, dna_sequence_match, dna_sequence_values_match;
    regex sequence_id("^@HUA:([0-9]*:*)+\\/[0-9]$");
    // επιτρεπτές τιμές αλληλουχίας dna
    regex dna_sequence("^[ATGC]+$");
    // Αξιολόγηση της ποιότητας του dna
    regex dna_sequence_values("^[!-I]+");

    string line;
    string id;
    string dna_seq;
    string validation;
    string msg = "";

    while(infile.is_open() && getline(infile, line)) {
        // Αναζήτηση της πληροφορίας που με ενδιαφέρει με regex
        regex_match(line, idmatch, sequence_id);

        if(idmatch.size() > 0) { // id
            id = line;
        }

        regex_match(line, dna_sequence_match, dna_sequence);

        if(dna_sequence_match.size() > 0) {
            dna_seq = line;
            continue;
        }

        regex_match(line, dna_sequence_values_match, dna_sequence_values);

        if(dna_sequence_values_match.size() > 0 &&
           dna_seq.length() != line.length() && line.length() > 1) {
            // ασυμφωνία αριθμού βάσεων με βαθμολογία αξιοπιστίας

            infile.close();
            outputfile.close();
            string msg = "Πρόβλημα στο αρχείο " + input_file +
                         " Ασυμφωνία αριθμού βάσεων με αριθμό αξιολογήσεων"
                         " στην αξιολόγηση της έλεγχος αρχείου";
            throw Exercise2Exception(msg);
        }

        if(line.length() == dna_seq.length()) {
            ids.insert(id);

            if(MAX_BUFFER_SIZE - msg.length() >
               id.length() + dna_seq.length() + 1) {
                msg += id + dna_seq;
            } else {
                memset(msg_buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));
                memcpy(msg_buffer, msg.c_str(), msg.length() * sizeof(char));
                MPI_Bcast(msg_buffer, MAX_BUFFER_SIZE,
                          MPI_CHAR, 0, MPI_COMM_WORLD);

                how_many_times_print_calculated_results++;
                print_calculated_results();
                msg = id + dna_seq;
            }
        }
    }

    if(msg.length() > 0) {
        memset(msg_buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));
        memcpy(msg_buffer, msg.c_str(), msg.length() * sizeof(char));
        MPI_Bcast(msg_buffer, MAX_BUFFER_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
        memset(msg_buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));
        memcpy(msg_buffer, FINISHED.c_str(), FINISHED.length() * sizeof(char));
        MPI_Bcast(msg_buffer, MAX_BUFFER_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

        print_calculated_results();
        how_many_times_print_calculated_results++;
    }
}

void Exercise2::wait_for_messages() {
    string received_msg;
    string processing_part;
    int start_point = 0, finish_point = 0;

    smatch match;
    regex re(".{18}[AGCT]+");

    do {
        memset(msg_buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));
        MPI_Bcast(msg_buffer, MAX_BUFFER_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

        received_msg = string(msg_buffer);
        if(received_msg.find(FINISHED) != string::npos) {
            continue;
        }

        start_point = (rank - 1) * MAX_BUFFER_SIZE / nprocs;
        finish_point = rank * MAX_BUFFER_SIZE / nprocs;

        while(start_point < MAX_BUFFER_SIZE && start_point >= 0 &&
              received_msg[start_point] != '@') {
            start_point--;
        }

        while(finish_point < MAX_BUFFER_SIZE &&
              received_msg[finish_point] != '\0' &&
              received_msg[finish_point + 1] != '@') {
            finish_point++;
        }

        processing_part = received_msg.substr(0, MAX_BUFFER_SIZE);

        stringstream results;
        results << fixed;
        results.precision(3);

        int GC = 0, AT = 0;
        while(regex_search(processing_part, match, re)) {
            for(smatch::iterator it = match.begin(); it != match.end(); it++) {
                results << string((*it).str()).substr(0, 18);

                string id_seq = string((*it).str()).substr(18);

                GC = 0, AT = 0;
                for(string::iterator it = id_seq.begin(); it != id_seq.end();
                    it++) {
                    if(*it == 'A' || *it == 'T') {
                        AT++;
                    } else {
                        GC++;
                    }
                }

                results << GC * 1.0 / (AT + GC) << endl;
            }
            processing_part = match.suffix().str();
        }

        MPI_Send(string(results.str()).c_str(), string(results.str()).length(),
                 MPI_CHAR, 0, DEFAULT_TAG, MPI_COMM_WORLD);

    } while(received_msg.find(FINISHED) == string::npos);
}

Exercise2::Exercise2(int argc, char **argv) {
    if(argc < 3) {
        throw Exercise2Exception(
            "Χρειάζονται δυο παράμετροι θέσεις στην γραμμή εντολών."
            "\nΤο πρώτο όρισμα αντιστοιχεί στο αρχείο εισόδου"
            " και το δεύτερο στο αρχείο εξόδου");
    }

    input_file = argv[1];
    output_file = argv[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    MPI_Barrier(MPI_COMM_WORLD);
    msg_buffer = new char[MAX_BUFFER_SIZE];

    if(rank == 0) {
        fout.open(argv[2]);
        read_file();
    } else {
        try {
            wait_for_messages();
        } catch(out_of_range e) {
            string msg = "Σφάλμα στην wait_for_messages() ";
            msg += e.what();

            throw Exercise2Exception(msg);
        }
    }
}

Exercise2::~Exercise2() {
    delete[] msg_buffer;

    MPI_Finalize();

    if(rank == 0) {
        fout.close();
    }
}

Exercise2Exception::Exercise2Exception(string exception) {
    whatHappened = exception;
}

const char *Exercise2Exception::what() { return whatHappened.c_str(); }
