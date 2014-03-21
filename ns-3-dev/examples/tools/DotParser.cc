#include "ns3/dotParser.h"
#include "ns3/stateMachine.h"
#include "ns3/stateMetric.h"

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
    StateMetricTracker smt;
    cout << "prop1: " << smt.GetMetricValue("prop1", m->GetCurrentState()) << endl;
    smt.UpdateMetric("prop1", m->GetCurrentState(), 1);
    cout << "prop1: " << smt.GetMetricValue("prop1", m->GetCurrentState()) << endl;
    smt.UpdateMetric("prop1", m->GetCurrentState(), 2);
    cout << "prop1: " << smt.GetMetricValue("prop1", m->GetCurrentState()) << endl;
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
    
    //TrSet invalid = m->GetInvalidTransitions(m->GetCurrentState());
    //Transition tr = *invalid.begin();

    //m->MakeTransition(tr);
    smt.UpdateMetric("prop1", m->GetCurrentState(), 2);

    smt.PrintAll();

    m->Print();
    
}
