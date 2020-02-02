#include <regex>
#include <string>

#include "exercise2.hpp"

int main(int argc, char **argv) {
    try {
        Exercise2(argc, argv);
    } catch(Exercise2Exception e) {
        cout << e.what() << endl;
        return 1;
    }
    return 0;
}
