#include <iostream>

#include "exercise3.hpp"

using namespace std;

int main(int argc, char **argv) {
    try {
        Exercise3(argc, argv);
    } catch(Exercise3Exception ex) {
        cout << ex.what() << endl;
        return 1;
    }
    return 0;
}
