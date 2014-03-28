#include "dotParser.h"
#include "ns3/message.h"

static int dp_debug=0;

namespace ns3 {
using namespace boost;
int DotParser::parseGraph(const char * filename) {
    std::ifstream ifs(filename);

    // Construct an empty graph and prepare the dynamic_property_maps.
    boost::dynamic_properties dp;

    this->name = get(vertex_name, this->graph);
    //property_map<StateGraph, vertex_name_t>::type name = get(vertex_name, graph);
    dp.property("node_id",this->name);

    this->mass = get(vertex_color, this->graph);
    dp.property("mass",this->mass);

    this->send = get(edge_send_t(), this->graph);
    dp.property("send",this->send);

    this->rcvd = get(edge_rcvd_t(), this->graph);
    dp.property("rcvd",this->rcvd);

    // Use ref_property_map to turn a graph property into a property map
    boost::ref_property_map<StateGraph*,std::string> gname(get_property(this->graph,graph_name));
    dp.property("name",gname);

    // Sample graph as an std::istream;
    std::istringstream
        gvgraph("digraph { graph [name=\"graphname\"];  a-> c; e [mass = 6.66] e->c;}");

    bool status = read_graphviz(ifs,this->graph,dp,"node_id");

    graph_traits < StateGraph >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(this->graph); vi != vi_end; ++vi) {
        graph_traits <StateGraph>::vertex_descriptor v = *vi;
        this->states.insert(make_pair(this->name[v], v));
    }

    graph_traits < StateGraph >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(this->graph); ei != ei_end; ++ei) {
        graph_traits < StateGraph >::edge_descriptor e = *ei;
        graph_traits < StateGraph >::vertex_descriptor u = source(e, this->graph), v = target(e, this->graph);
        this->valid.insert(make_pair(this->name[u], this->name[v]));
    }
    
    if(dp_debug>0){std::cout << "status: " << status << std::endl;}
    return status;
}

void DotParser::BuildStateMachine (StateMachine *machine) {
    graph_traits < StateGraph >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(this->graph); ei != ei_end; ++ei) {
        graph_traits < StateGraph >::edge_descriptor e = *ei;
        graph_traits < StateGraph >::vertex_descriptor from = source(e, this->graph), to = target(e, this->graph);
        State fromState = State(this->name[from]);
        State toState = State(this->name[to]);
        machine->AddState(fromState);
        machine->AddState(toState);
        string rcvd = get(this->rcvd, e);
        string send = get(this->send, e);
        // match transition
        // TODO transition and message type matching
        if (send == "") send = "-";
        int type = Transition::getTransitionType(Transition::getMsgTypeFromAction(rcvd), Transition::getMsgTypeFromAction(send));
        //int type = machine->GetTransitionType(rcvd, send);
        Transition tr = Transition(type, rcvd, send);
        machine->AddTransition(tr, fromState, toState);
    }

}

void DotParser::print_graph () {
    graph_traits < StateGraph >::vertex_iterator vi, vi_end;

    for (boost::tie(vi, vi_end) = vertices(this->graph); vi != vi_end; ++vi) {
        graph_traits <StateGraph>::vertex_descriptor v = *vi;
        std::cout << "state: " << this->name[v] << std::endl;
    }

    graph_traits < StateGraph >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(this->graph); ei != ei_end; ++ei) {
        graph_traits < StateGraph >::edge_descriptor e = *ei;
        graph_traits < StateGraph >::vertex_descriptor u = source(e, this->graph), v = target(e, this->graph);
        std::cout << this->name[u] << " -> " << this->name[v]
            << "[rcvd=\"" << get (this->rcvd, e) << "\" "
            << "[send=\"" << get (this->send, e) << "\"";
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;

}

int DotParser::getLegalTransision(std::string cur) {
    boost::unordered_map<std::string, graph_traits < StateGraph >::vertex_descriptor>::iterator vi;
    //graph_traits < StateGraph >::vertex_iterator vi;

    vi = this->states.find(cur);

    if (vi != this->states.end()) {
    	if(dp_debug>0){std::cout << "found" << std::endl;}
        //graph_traits <StateGraph>::vertex_descriptor v = (vi)->second;
        boost::unordered_multimap<std::string, std::string>::iterator valid_next;
        valid_next = this->valid.find(cur);
        while(valid_next != this->valid.end()) {
            if (valid_next->first.compare(cur) != 0) break;
            if(dp_debug>0){std::cout << "next: " << valid_next->second << std::endl;}
            valid_next++; 
        }


        return 1;
    }
    return 0;
}

}
