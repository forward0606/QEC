#ifndef __ALGORITHMBASE_H
#define __ALGORITHMBASE_H

#include<iostream>
#include<vector>
#include<queue>
#include <algorithm>
#include <string>
#include "../../Network/Graph/Graph.h"
#include "../../Request/Request.h"
#include "../../config.h"
using namespace std;


class AlgorithmBase{
    string algorithm_name;
protected:
    int timeslot, waiting_time;
    int throughputs;
    int time_limit;
    double swap_prob;
    //Request* generate_new_request();
public:
    vector<Request> requests;
    Graph graph;
    AlgorithmBase(string filename, string algorithm_name, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    ~AlgorithmBase();
    double get_swap_prob();
    string get_name();
    virtual void path_assignment(){};
    int find_width(vector<int> path);
    vector<int> BFS(int source, int destination);
    void assign_resource(vector<int> path, int reqno);
    virtual void entangle();
    virtual void swap();
    void send();
    virtual void next_time_slot();
    void run();
    int total_throughput();
    map<string, double> result;

    Path *find_swap_path(vector<int> path_nodes, map<pair<int, int>, vector<Channel*>> &remain_channels);
};

#endif