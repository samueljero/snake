#include "ns3/dotParser.h"

using namespace ns3;

int main(int argc, char **argv) {
    const char *filename = "filename.txt";
    std::string state = "ESTB";
    if (argc > 1) {
        std::cout << "test file:[" << argv[1] << "]" << std::endl;
        filename = argv[1];
    }
    if (argc > 2) {
        state = argv[2];
    }
    DotParser *dt = new DotParser();
    dt->parseGraph(filename);
    dt->print_graph();
    dt->getLegalTransision(std::string(state));
}
