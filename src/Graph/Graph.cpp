#include <fstream>
#include <random>
#include <ctime>
#include "Graph.h"

Graph::Graph(string filename, int num_of_node, double swap_prob, double entangle_alpha)
	:min_channel(3), max_channel(7),
     min_memory_cnt(10), max_memory_cnt(14), swap_prob(swap_prob), entangle_alpha(entangle_alpha), time_limit(7){
    generate(filename, num_of_node);
}


/*
c++ call system to run python (waxman)
python generate graph, and then write to file
c++ read file to get the nodes and edges
*/
void Graph::generate(string filename, int num_of_node){
	string command = "python3 main.py ";
    system((command + to_string(num_of_node) + " " + filename).c_str());
	
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
    //Node node1, node2;
    int node_id1, node_id2;
    while(graph_input >> node_id1 >> node_id2){
        Node &node1 = nodes[node_id1];
        Node &node2 = nodes[node_id2];
        int channel_cnt = unif(generator) % (max_channel-min_channel) + min_channel;
        if(node1 == node2){
            cerr<<"error:\texist an edge with same node!"<<endl;
            exit(1);
	    }
        double entangle_prob = exp(-entangle_alpha * (node1.distance(node2))); // e^( -alpha * dis(node1, node2) )
        for(int i = 0; i < channel_cnt; i++){
            channels[make_pair(node1, node2)].emplace_back(&node1, &node2, entangle_prob);
        }
        //edges[make_pair(node1, node2)] = &(Edge(&node1, &node2, channel_cnt, entangle_alpha));
    }
}

void Graph::refresh(){ // refresh all channel entangle status
    for(auto &chans:channels){
        for(auto &e:chans.second){
            e.refresh();
        }
    }
}

void Graph::release(){ //clean all assigned resource(node and channel)
    for(int i = 0; i < (int)nodes.size(); i++){
        nodes[i].release();
    }
    for(auto &chans:channels){
        for(auto &e:chans.second){
            e.release();
        }
    }
}


