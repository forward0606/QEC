#ifndef __MYGREEDYALGO
#define __MYGREEDYALGO
#include    <iostream>
#include    <algorithm>
#include    <queue>
#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include    "../../config.h"
using namespace std;

class MyGreedyAlgo:public AlgorithmBase{
public:
    MyGreedyAlgo(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
    void entangle();
    void swap();
    void send();
    void next_time_slot();
    vector<int>  Dijkstra(int src, int dst);
    double get_weight(int node1_id, int node2_id);
};

#endif