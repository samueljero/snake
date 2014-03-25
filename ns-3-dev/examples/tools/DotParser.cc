#include "ns3/dotParser.h"
#include "ns3/stateMachine.h"
#include "ns3/stateMetric.h"
#include "ns3/nstime.h"
       #include <sys/time.h>


using namespace ns3;

static const uint64_t US_PER_NS = (uint64_t)1000;
static const uint64_t US_PER_SEC = (uint64_t)1000000;
static const uint64_t NS_PER_SEC = (uint64_t)1000000000;

uint64_t GetNow() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t nsResult = tv.tv_sec * NS_PER_SEC + tv.tv_usec * US_PER_NS;
    return nsResult;
}
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
    //dt->print_graph();
    StateMachine *m = new StateMachine();
    dt->BuildStateMachine(m);
    
    m->PrintRules();
    uint64_t now = GetNow();
    m->Start(initState, GetNow());
    StateMetricTracker *smt = m->GetStateMetricTracker();
    cout << "prop1: " << smt->GetMetricValue("prop1", m->GetCurrentState()) << endl;
    smt->UpdateMetric("prop1", m->GetCurrentState(), 1);
    cout << "prop1: " << smt->GetMetricValue("prop1", m->GetCurrentState()) << endl;
    smt->UpdateMetric("prop1", m->GetCurrentState(), 2);
    cout << "prop1: " << smt->GetMetricValue("prop1", m->GetCurrentState()) << endl;
    NextMap valid = m->GetValidTransitions(m->GetCurrentState());
    if (!valid.empty()) {
        std::cout << "valid" << std::endl;
        m->MakeTransition(valid.begin()->first, GetNow());
        m->Print();
    } else {
        std::cout << "No possible Transition from " << initState << std::endl;
    }

    valid = m->GetValidTransitions(m->GetCurrentState());
    if (!valid.empty()) {
        std::cout << "valid" << std::endl;
        m->MakeTransition(valid.begin()->first, GetNow());
        m->Print();
    } else {
        std::cout << "No possible Transition from " << initState << std::endl;
    }

    valid = m->GetValidTransitions(m->GetCurrentState());
    if (!valid.empty()) {
        std::cout << "valid" << std::endl;
        m->MakeTransition(valid.begin()->first, GetNow());
        m->Print();
    } else {
        std::cout << "No possible Transition from " << initState << std::endl;
    }

    valid = m->GetValidTransitions(m->GetCurrentState());
    if (!valid.empty()) {
        std::cout << "valid" << std::endl;
        m->MakeTransition(valid.begin()->first, GetNow());
        m->Print();
    } else {
        std::cout << "No possible Transition from " << initState << std::endl;
    }
    NextMap toGo = m->GetTransitionsTo(toState);
    if (!toGo.empty()) {
        m->MakeTransition(toGo.begin()->first, GetNow());
        m->Print();
    } else {
        std::cout << "No Transition to Go " << toState << std::endl;
    }
    
    std::cout << "=============" << std::endl;
    TrSet invalid = m->GetInvalidTransitions(m->GetCurrentState());
    Transition tr = *invalid.begin();

    m->MakeTransition(tr, GetNow());
    smt->UpdateMetric("prop1", m->GetCurrentState(), 2);
    m->Finish(GetNow());
    smt->PrintAll(cout);

    m->Print();
    smt->Reset();
    smt->PrintAll(cout);
    
}
