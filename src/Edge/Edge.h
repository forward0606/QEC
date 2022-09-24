#include<iostream>
#include<vector>
#include"../Channel/Channel.h"
using namespace std;

class Edge{
    Node *node1_ptr, *node2_ptr;
    int remain;
    vector<Channel> channels;
public:
	Edge(Node* node1_ptr, Node* node2_ptr, int channel_cnt, double entangle_alpha);
    void refresh();
    void release(int width);
    bool has_empty_channel()const;
    Channel& assign_channel();
};