#ifndef __PATH_H
#define __PATH_H

#include <vector>
#include "../Network/Node/Node.h"
#include "../Network/Channel/Channel.h"

class Path{
    vector<Node*> nodes;
    vector<Channel*> channels;
    bool entangle_succ, swap_succ;
    double prob;
public:
	Path(vector<Node*> nodes, vector<Channel*> channels);
    ~Path(void);
    int get_len();
    vector<Node*> get_nodes();
    bool entangle();        //true->entangle success
    bool get_entangle_succ();
    bool swap();            //true->swapped success
    bool get_swap_succ();
    void release();
    //virtual double compute_weight();
};

#endif