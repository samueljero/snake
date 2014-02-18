#include "ns3/dotParser.h"
#include "ns3/stateMachine.h"

using namespace ns3;

int main(int argc, char **argv) {
    const char *filename = "filename.txt";
    std::string initState = "CLOSED";
    std::string toState = "CLOSE_WAIT";
    if (argc > 1) {
        std::cout << "test file:[" << argv[1] << "]" << std::endl;
        filename = argv[1];
    }
    if (argc > 2) {
        initState = argv[2];
    }
    if (argc > 3) {
        toState = argv[3];
    }
    DotParser *dt = new DotParser();
    dt->parseGraph(filename);
    dt->print_graph();
    StateMachine *m = new StateMachine();
    dt->BuildStateMachine(m);
    
    m->Start(initState);
    TrSet valid = m->GetValidTransitions(m->GetCurrentState());
    if (!valid.empty()) {
        Transition tr = *valid.begin();
        m->MakeTransition(tr);
        m->Print();
    } else {
        std::cout << "No possible Transition from " << initState << std::endl;
    }

    TrSet toGo = m->GetTransitionsTo(toState);
    if (!toGo.empty()) {
        Transition tr = *toGo.begin();
        m->MakeTransition(tr);
        m->Print();
    } else {
        std::cout << "No Transition to Go " << toState << std::endl;
    }
    
    TrSet invalid = m->GetInvalidTransitions(m->GetCurrentState());
    Transition tr = *invalid.begin();
    m->MakeTransition(tr);

    m->Print();
    
}
