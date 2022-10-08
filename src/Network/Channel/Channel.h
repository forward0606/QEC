#ifndef __CHANNEL_H
#define __CHANNEL_H


#include <iostream>
#include <random>
#include "../Node/Node.h"
#include "../../config.h"
using namespace std;

//use for entangled
const int CHANNEL_UNENTANGLE = 0;
const int CHANNEL_ENTANGLE_SUCC = 1;
const int CHANNEL_ENTANGLE_FAIL = -1;


class Channel{
    Node *node1_ptr, *node2_ptr;
    double entangle_prob;
    bool used; // assigned to path or not
    int entangled; // -1: CHANNEL_ENTANGLE_FAIL, 0: CHANNEL_UNENTANGLE, 1: CHANNEL_ENTANGLE_SUCC
    double weight;
public:
    
    Channel(Node* node1, Node* node2, double entangle_prob);
    ~Channel();

    void set_weight(double _weight);
    double get_weight();
    double get_entangle_prob();
    Node* get_node1_ptr();
    Node* get_node2_ptr();

    bool is_assignable();
    void assign();
    bool entangle();
    bool is_used();
    bool is_entangled();
    void refresh();
    void release();
};

#endif
