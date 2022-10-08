#ifndef __ALGORITHMBASE_H
#define __ALGORITHMBASE_H

#include<iostream>
#include<vector>
#include<queue>
#include <algorithm>
#include "../../Network/Graph/Graph.h"
#include "../../Request/Request.h"
#include "../../config.h"
using namespace std;


class AlgorithmBase{
    int timeslot, waiting_time;
    int throughputs;
    int time_limit;
    double swap_prob;
    //Request* generate_new_request();
public:
    vector<Request> requests;
    Graph graph;
    AlgorithmBase(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    //~AlgorithmBase();
    double get_swap_prob();
    virtual void path_assignment(){};
    int find_width(vector<int> path);
    vector<int> BFS(int source, int destination);
    void assign_resource(vector<int> path, int reqno);
    virtual void entangle();
    virtual void swap();
    virtual void next_time_slot();
    void run();
    int total_throughput();
};

#endif