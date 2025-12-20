#include "Test.hpp"
#include <cstdio>

int main(int argc, const char *argv[]) {
    if (argc == 4) {
        Test(argv[2], argv[3]);
        return 0;
    }
    printf("suppose to run tests with ./testBot test path/to/perft.epd path/to/output.epd");
    return 1;
}
