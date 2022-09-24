#include"Channel.h"

Channel::Channel(Node *node1_ptr, Node *node2_ptr, double entangle_prob)
	:node1_ptr(node1_ptr), node2_ptr(node2_ptr), entangle_prob(entangle_prob), used(0), entangled(0){

}

bool Channel::entangle(){
	//亂數引擎 
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<double> dis(0.0, 1.0);
    if(dis(gen) <= entangle_prob){ // entangle success
        return true;
    }
	return false;
}

bool is_assignable(){
	Node node1 = *node1_ptr;
	Node node2 = *node2_ptr;
	if(!node1.is_assignable())	return false;
	if(!node2.is_assignable())	return false;
	if(!used)					return false;
	return true;
}

void assign(){
	Node node1 = *node1_ptr;
	Node node2 = *node2_ptr;
	if(!is_assignable())	exit(1);
	used = true;
	node1--, node2--;
}