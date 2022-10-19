#ifndef __GRAPH_H
#define __GRAPH_H
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include "../Node/Node.h"
#include "../../Path/Path.h"
#include "../../config.h"
using namespace std;

class Graph{
	// parameter for generating graph
	int num_of_node;
	int num_of_edge;
	int time_limit;
	double swap_prob, entangle_alpha;

    vector<Node> nodes;
    map<pair<const Node, const Node>, vector<Channel>> channels;
	vector<vector<int>> neighbor;
	vector<vector<int>> social;

	Channel* assign_channel(Node &node1, Node &node2);	//in algorithm we use this function to find a channel, then collect a sequence of channels. Finally, construct them to be a Path.
public:
    //Graph(vector<Node> nodes, map<node, vector<Edge>> edges);
    Graph(string filename, int time_limit, double swap_prob, double entangle_alpha);
	// waxman_graph(n, beta=0.4, alpha=0.1, L=None, domain=(0, 0, 1, 1), metric=None, seed=None)
	~Graph(void);

	int get_size();											//return |V|
	vector<int> get_neighbors_id(int node1_id);				//int get_neighbor_size(int node_id); int get_neighbor_id(int node1_id, int nth);
	double get_channel_weight(int node1_id, int node2_id);	//return the average weight of channel between node1 and node2
	int get_channel_size(int node1_id, int node2_id);
	Node* Node_id2ptr(int id);

	bool is_friend(int source, int middle_node);			//trust each other?
	void generate(string);
	void refresh();
	void release();
	void set_weight(int node1_id, int node2_id, double value);
	int remain_resource_cnt(int node1_id, int node2_id, bool is1_repeater = true, bool is2_repeater = true);	//the resource width between node1 and node2
	Path *build_path(vector<int> nodes_id);
};


#endif



