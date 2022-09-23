#include <fstream>
#include <random>
#include <ctime>
#include "../Graph/Graph.h"

Graph::Graph(string filename, int num_of_node, double swap_prob, double entangle_alpha)
	:num_of_node(num_of_node), num_of_edge(0), min_channel(3), max_channel(7),
     min_memory_cnt(10), max_memory_cnt(14), swap_prob(swap_prob), entangle_alpha(entangle_alpha), time_limit(7){
    generate(filename);
}

void Graph::generate(string filename){
	system("python3 main.py " + to_string(num_of_node) + " " + filename);
	
    //亂數引擎 
    random_device rd;
    default_random_engine generator = default_random_engine(rd());
    uniform_int_distribution<int> unif(0,2147483647);
    
    ifstream graph_input;
    graph_input.open (filename);
    srand(time(NULL));
	double pos_x, pos_y;
    for(int i = 0; i < num_of_node; i++){
		graph_input >> pos_x >> pos_y;
		int memory_cnt =unif(generator) % (max_memory_cnt-min_memory_cnt) + min_memory_cnt;
		nodes.emplace_back(memory_cnt, time_limit, pos_x, pos_y);
	}
}

/*
c++ call system to run python (waxman)
python generate graph, and then write to file
c++ read file to get the nodes and edges
*/