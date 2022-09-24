#include <fstream>
#include <random>
#include <ctime>
#include "Graph.h"

Graph::Graph(string filename, int num_of_node, double swap_prob, double entangle_alpha)
	:min_channel(3), max_channel(7),
     min_memory_cnt(10), max_memory_cnt(14), swap_prob(swap_prob), entangle_alpha(entangle_alpha), time_limit(7){
    generate(filename);
}

void Graph::generate(string filename, int num_of_node){
	system("python3 main.py " + to_string(num_of_node) + " " + filename);
	
    //亂數引擎 
    random_device rd;
    default_random_engine generator = default_random_engine(rd());
    uniform_int_distribution<int> unif(0,2147483647);
    
    ifstream graph_input;
    graph_input.open (filename);

    // input of nodes
    double pos_x, pos_y;
    for(int i = 0; i < num_of_node; i++){
		graph_input >> pos_x >> pos_y;
		int memory_cnt = unif(generator) % (max_memory_cnt-min_memory_cnt) + min_memory_cnt;
		nodes.emplace_back(memory_cnt, time_limit, pos_x, pos_y, swap_prob);
	}
    
    // input of edges
    Node node1, node2;
    int node_id1, node_id2;
    while(graph_input >> node_id1 >> node_id2){
        node1 = nodes[node_id1];
        node2 = nodes[node_id2];
        int channel_cnt = unif(generator) % (max_channel_cnt-min_channel_cnt) + min_channel_cnt;
        edges[node1].emplace_back(node1, node2, channel_cnt, entangle_alpha);
    }
}

/*
c++ call system to run python (waxman)
python generate graph, and then write to file
c++ read file to get the nodes and edges
*/