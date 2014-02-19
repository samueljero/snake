#ifndef DOT_PARSER_H
#define DOT_PARSER_H
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <string>
#include "stateMachine.h"


namespace ns3 {
    struct edge_send_t {
        typedef boost::edge_property_tag kind;
    };

    struct edge_rcvd_t {
        typedef boost::edge_property_tag kind;
    };

    enum edge_mysend_t { edge_mysend };
    enum edge_myrcvd_t { edge_myrcvd };


    // Vertex properties
    typedef boost::property < boost::vertex_name_t, std::string,
            boost::property < boost::vertex_color_t, float > > vertex_p;  
    // Edge properties
    typedef boost::property < edge_send_t, std::string, 
            boost::property < edge_rcvd_t, std::string > > edge_p;
    // Graph properties
    typedef boost::property < boost::graph_name_t, std::string > graph_p;
    // adjacency_list-based type
    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
            vertex_p, edge_p, graph_p > StateGraph;


    class DotParser 
    {
        public:
        StateGraph graph;
        boost::property_map<StateGraph, boost::vertex_name_t>::type name;
        boost::property_map<StateGraph, boost::vertex_color_t>::type mass;
        boost::property_map<StateGraph, edge_send_t>::type send;
        boost::property_map<StateGraph, edge_rcvd_t>::type rcvd;
        boost::unordered_map<std::string, boost::graph_traits < StateGraph>::vertex_descriptor> states;
        boost::unordered_multimap<std::string, std::string> valid;

        int parseGraph(const char *);
        void print_graph ();
        int getLegalTransision(std::string cur) ;
        void getInvalidTransitions(std::string state);
        void MakeTargetTransition();
        void BuildStateMachine(StateMachine *machine);

        DotParser () {
            StateGraph graph(0);
            graph = graph;
        };


    };
}
#endif 
