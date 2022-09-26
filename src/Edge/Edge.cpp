#include <vector>
#include <math.h>

#include "Edge.h"

Edge::Edge(Node* node1_ptr, Node* node2_ptr, int channel_cnt, double entangle_alpha)
	:node1_ptr(node1_ptr), node2_ptr(node2_ptr), remain(channel_cnt){
	
	Node &node1 = *node1_ptr, &node2 = *node2_ptr;
	if(node1 == node2){
		cerr<<"error:\texist an edge with same node!"<<endl;
		exit(1);
	}
	double entangle_prob = exp(-entangle_alpha * (node1.distance(node2))); // e^( -alpha * dis(node1, node2) )
	vector<Channel> channels;
	for(int i = 0; i < channel_cnt; i++){
		channels.emplace_back(node1_ptr, node2_ptr, entangle_prob);
	}
}

Edge::~Edge(){

}

bool Edge::has_empty_channel()const{
	return remain > 0;
}

Channel& Edge::assign_channel(){
	if(!has_empty_channel()){
		cerr<<"error:\tcan't assign a channel in full edge";
		exit(1);
	}
	remain--;
	channels[remain].assign();
	return channels[remain];
}

void Edge::refresh(){
	for(auto& channel:channels){
		channel.refresh();
	}
}

void Edge::release(int width){
	remain += width;
}