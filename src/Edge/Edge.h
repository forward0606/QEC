#include<iostream>
#include<vector>
#include"../Channel/Channel.h"
using namespace std;

struct Edge{
    Node node1, node2;
    vector<Channel> channels;
    double entangle_prob;
public:
	Edge(Node node1, Node node2, vector<Channel> channels);
};