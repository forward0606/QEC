#include<iostream>
#include<vector>
#include "../../Network/Graph/Graph.h"
#include "../../Request/Request.h"
using namespace std;


class AlgorithmBase{
    int timeslot, waiting_time;
    int throughputs;
    int new_request_min, new_request_max;
    Graph graph;
    vector<Request*> requests;
    int time_limit;
    Request* generate_new_request();
public:
    AlgorithmBase();
    ~AlgorithmBase();
    virtual void path_assignment() = 0;
    void entangle();
    void swap();
    void next_time_slot();
};
