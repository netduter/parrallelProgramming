#ifndef __EXERCISE_1__
#define __EXERCISE_1__

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;

class Exercise1 {
    public:
    Exercise1(int argc, char **argv);
    void readFile();

    private:
    vector<string> files;
    string inputFile;
    string outputFile;
};

class Exercise1Exception : exception {
    public:
    Exercise1Exception(string);
    const char *what();

    private:
    string whatHappened;
};

#endif  // __EXERCISE_1__
