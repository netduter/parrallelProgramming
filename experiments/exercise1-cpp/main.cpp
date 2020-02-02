#include "exercise1.hpp"

int main(int argc, char **argv) {
    try {
        Exercise1 exercise1(argc, argv);
        exercise1.readFile();
    } catch(Exercise1Exception e) {
        cout << e.what() << endl;
        return 1;
    }
    return 0;
}
