#include"Channel.h"

Channel::Channel(Node *node1_ptr, Node *node2_ptr, double entangle_prob)
	:node1_ptr(node1_ptr), node2_ptr(node2_ptr), entangle_prob(entangle_prob), used(0), entangled(CHANNEL_UNENTANGLE), weight(entangle_prob){
	if((*node1_ptr) == (*node2_ptr)){
		cerr<<"error:\texist an edge with same node!"<<endl;
		exit(1);
	}
	cerr<<"New channel!"<<node1_ptr->get_id() << " " << node2_ptr->get_id() << ", entangle_prob = " << entangle_prob << endl;
}

Channel::~Channel(){

}

void Channel::set_weight(double _weight){
	weight = _weight;
	return;
}

double Channel::get_weight(){
	return weight;
}

double Channel::get_entangle_prob(){
	return entangle_prob;
}

Node* Channel::get_node1_ptr(){
	return node1_ptr;
}

Node* Channel::get_node2_ptr(){
	return node2_ptr;
}

bool Channel::is_assignable(){
	Node &node1 = *node1_ptr;
	Node &node2 = *node2_ptr;
	if(!node1.is_assignable())	return false;
	if(!node2.is_assignable())	return false;
	if(used)					return false;
	return true;
}

void Channel::assign(){
	Node &node1 = *node1_ptr;
	Node &node2 = *node2_ptr;
	if(!is_assignable()){
		cerr << "error:\tassign Channel but is NOT assignable" << endl;
		exit(1);
	}
	used = true;
	entangled = CHANNEL_UNENTANGLE;
	node1--, node2--;
}

bool Channel::entangle(){
	if(!used){
		cerr<<"error:\ttry to entangle a nonassign channel !"<<endl;
		exit(1);
	}
	if(entangled != CHANNEL_UNENTANGLE){
		cerr<<"error:\tchannel is aleady entangled!"<<endl;
		exit(1);
	}
	//亂數引擎 
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<double> dis(0.0, 1.0);
    if(dis(gen) <= entangle_prob){ // entangle success
		entangled = CHANNEL_ENTANGLE_SUCC;
        return true;
    }
	entangled = CHANNEL_ENTANGLE_FAIL;
	return false;
}

bool Channel::is_used(){
	return used;
}
bool Channel::is_entangled(){
	if(!used){
		cerr<<"error:\tthe channel is not used!"<<endl;
		exit(1);
	}
	if(entangled == CHANNEL_UNENTANGLE){
		cerr<<"error:\tthe channel is unentangled!"<<endl;
		exit(1);
	}
	return entangled == CHANNEL_ENTANGLE_SUCC;
}

void Channel::refresh(){
	entangled = CHANNEL_UNENTANGLE;
}

void Channel::release(){
	if(used){
		(*node1_ptr)++;
		(*node2_ptr)++;
	}
	used = false;
	entangled = CHANNEL_UNENTANGLE;
}