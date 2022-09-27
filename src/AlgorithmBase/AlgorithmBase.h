#include<iostream>
#include<vector>
#include "Graph/Graph.h"
#include "Request/Request.h"
using namespace std;


class AlgorithmBase{
    int timeslot, waiting_time;
    int throughputs;
    Graph graph;
    vector<Request*> requests;
    Request* generate_new_request();
public:
    AlgorithmBase();
    ~AlgorithmBase();
    virtual void path_assignment() = 0;
    void entangle();
    virtual void path_choosing_for_swapping() = 0;
    void swap();
    void next_time_slot();
};