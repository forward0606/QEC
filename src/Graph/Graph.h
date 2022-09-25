#include <iostream>
#include <vector>
#include <map>
#include "../Node/Node.h"
#include "../Edge/Edge.h"
using namespace std;

class Graph{
	// parameter for generating graph
	const int min_channel, max_channel;       //channel range
	const int min_memory_cnt, max_memory_cnt; //number of quantum memory
	int time_limit;
	double swap_prob, entangle_alpha; 
	pair<double, double> min_pos, max_pos; // rectangle's left up, right down
	double average_distance;


    vector<Node> nodes;
    map<pair<const Node, const Node>, vector<Channel>> channels;
public:
    //Graph(vector<Node> nodes, map<node, vector<Edge>> edges);
    Graph(string filename, int num_of_node, double swap_prob, double entangle_alpha);
	// waxman_graph(n, beta=0.4, alpha=0.1, L=None, domain=(0, 0, 1, 1), metric=None, seed=None)
	~Graph(void);
	void generate(string, int);
	void refresh();
	void release();
	Channel* get_channel(int node_id1, int node_id2);
	int remain_resource_cnt(int node_id1, int node_id2);	//the resource width between node1 and node2
};
