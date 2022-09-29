#include <iostream>
#include <vector>
#include <map>
#include "../Node/Node.h"
#include "../../Path/Path.h"
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
	vector<vector<int>> neighbor;

	Channel* assign_channel(Node node1, Node node2);	//in algorithm we use this function to find a channel, then collect a sequence of channels. Finally, construct them to be a Path.
public:
    //Graph(vector<Node> nodes, map<node, vector<Edge>> edges);
    Graph(string filename, int num_of_node, double swap_prob, double entangle_alpha);
	// waxman_graph(n, beta=0.4, alpha=0.1, L=None, domain=(0, 0, 1, 1), metric=None, seed=None)
	~Graph(void);

	int get_size();											//return |V|
	vector<int> get_neighbors_id(int node1_id);				//int get_neighbor_size(int node_id); int get_neighbor_id(int node1_id, int nth);
	double get_channel_weight(int node1_id, int node2_id);	//return the average weight of channel between node1 and node2
	Node* Node_id2ptr(int id);

	void generate(string, int);
	void refresh();
	void release();
	void set_weight(int node1_id, int node2_id, double value);
	int remain_resource_cnt(int node1_id, int node2_id);	//the resource width between node1 and node2
	Path *build_path(vector<int> nodes_id);
};






