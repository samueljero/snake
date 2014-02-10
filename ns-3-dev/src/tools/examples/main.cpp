#include "dotParser.h"

int main(int argc, char **argv) {
    struct dotParseData dpd;
    const char *filename = "filename.txt";
    std::string state = "ESTB";
    if (argc > 1) {
        std::cout << "test file:[" << argv[1] << "]" << std::endl;
        filename = argv[1];
    }
    if (argc > 2) {
        state = argv[2];
    }
    parseGraph(filename, &dpd);
    print_graph(&dpd);
    getLegalTransision(&dpd, std::string(state));
}
