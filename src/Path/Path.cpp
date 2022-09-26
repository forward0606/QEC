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
}

Path::~Path(){
    release();
}

bool Path::entangle(){
    if(channels.size() == 0 || nodes.size() >1){
        cerr << "error:\tWTF" << endl;
        exit(1);
    }
    entangle_succ = true;
    for(auto &channel:channels){
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
    for(int i=1;i<nodes.size()-1;i++){
        Node* node = nodes[i];
        swap_succ &= node->swap();
    }
    return swap_succ;
} 

bool Path::get_swap_succ(){
    return swap_succ;
}

void Path::release(){
    for(auto &channel:channels){
        channel->release();
    }
}