#ifndef __ALGORITHMBASE_H
#define __ALGORITHMBASE_H

#include<iostream>
#include<vector>
#include<queue>
#include <algorithm>
#include <string>
#include<map>
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
    map<string, double> res;
    //Request* generate_new_request();
public:
    vector<Request> requests;
    Graph graph;
    AlgorithmBase(string filename, string algorithm_name, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    virtual~AlgorithmBase();
    double get_swap_prob();
    string get_name();
    double get_res(string s);
    virtual void path_assignment()=0;
    int find_width(vector<int> path);
    vector<int> BFS(int source, int destination);
    void assign_resource(vector<int> path, int reqno);
    void base_entangle();
    void base_swap();
    void base_send();
    void base_next_time_slot();
    virtual void entangle()=0;
    virtual void swap()=0;
    virtual void send()=0;
    virtual void next_time_slot()=0;
    void run();
    int total_throughput();

    Path *find_swap_path(vector<int> path_nodes, map<pair<int, int>, vector<Channel*>> &remain_channels);
};

#endif