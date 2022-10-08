#include "Path.h"

Path::Path(vector<Node*> nodes, vector<Channel*> channels)
    :nodes(nodes), channels(channels), entangle_succ(false), swap_succ(false){
        prob = 1;
        for(auto channel:channels){
            prob *= channel->get_entangle_prob();
        }
        for(int i = 1; i < (int)nodes.size()-1; i++){ // source and destination no swap
            prob *= nodes[i]->get_swap_prob();
        }
        if(DEBUG)cerr<<"new Path"<<endl;
}

// Path::Path(vector<Node*> nodes)
//     :nodes(nodes), entangle_succ(true), swap_succ(false){
//         cerr<<"new entangled path"<<endl;
// }

Path::~Path(void){
    if(DEBUG)cerr<<"delete Path"<<endl;
    //release();
}

int Path::get_len(){
    return channels.size();
}

vector<Node*> Path::get_nodes(){
    return nodes;
}

vector<Channel*> Path::get_channels(){
    return channels;
}

bool Path::entangle(){
    if(DEBUG)cerr<<"entangle path :"<<endl;
    this->print();
    if(channels.size() == 0 || nodes.size() <= 1){
        cerr << "error:\tWTF" << endl;
        exit(1);
    }
    entangle_succ = true;
    for(auto &channel:channels){
        if(DEBUG)cerr<<"entangle channel : ";
        entangle_succ &= channel->entangle();
    }
    return entangle_succ;
}

bool Path::get_entangle_succ(){
    return entangle_succ;
}

bool Path::swap(){
    if(!entangle_succ){
        cerr<<"error:\tcan't swap on a path with fail entanglement!"<<endl;
    }
    swap_succ = true;
    for(int i=1;i<(int)nodes.size()-1;i++){
        Node* node = nodes[i];
        swap_succ &= node->swap();
    }
    return swap_succ;
} 

bool Path::get_swap_succ(){
    return swap_succ;
}

void Path::release(){
    if(DEBUG)cerr<<"release()"<<endl;
    for(auto &channel:channels){
        channel->release();
    }
}

void Path::print(){
    if(!DEBUG)  return;
    cerr<<"this is a Path"<<endl;
    cerr<<"\tentangle_succ = "<<entangle_succ<<endl;
    cerr<<"\tswap_succ = "<<swap_succ<<endl;
    cerr<<"\t the node in path is:";
    for(auto n:nodes){
        cerr<<n->get_id()<<" ";
    }
    cerr<<endl;
    cerr<<"\t the channel in path is:";
    for(auto c:channels){
        cerr<<c<<" ";
    }
    cerr<<endl;
}


// double compute_weight(){
    
// }