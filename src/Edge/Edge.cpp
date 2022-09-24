#include <vector>
#include <math.h>

#include "Edge.h"
#include "../Channel/Channel.h"
#include "../Node/Node.h"

Edge::Edge(Node* node1_ptr, Node* node2_ptr, int channel_cnt, double entangle_alpha)
	:node1_ptr(node1_ptr), node2_ptr(node2_ptr){
	
	Node node1 = *node1_ptr, node2 = *node2_ptr;
	double entangle_prob = exp(-entangle_alpha * (node1.distance(node2))); // e^( -alpha * dis(node1, node2) )
	vector<Channel> channels;
	for(int i = 0; i < channel_cnt; i++){
		channels.emplace_back(node1_ptr, node2_ptr, entangle_prob);
	}
}
