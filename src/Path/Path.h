#ifndef __PATH_H
#define __PATH_H

#include <vector>
#include "../Network/Node/Node.h"
#include "../Network/Channel/Channel.h"
#include "../config.h"

class Path{
    vector<Node*> nodes;
    vector<Channel*> channels;
    bool swap_succ;
    double prob;
public:
	Path(vector<Node*> nodes, vector<Channel*> channels);
    //Path(vector<Node*> nodes);                        zzzz
    ~Path(void);
    int get_len();
    vector<Node*> get_nodes();
    vector<Channel*> get_channels();
    void entangle();        //true->entangle success
    bool get_entangle_succ();
    bool swap();            //true->swapped success
    bool get_swap_succ();
    void release();
    void print();
    //virtual double compute_weight();
};

#endif