#include<iostream>
#include<vector>
#include "Graph/Graph.h"
#include "Request/Request.h"
using namespace std;


class AlgorithmBase{
    Graph graph;
    vector<Request*> requests;
public:
    AlgorithmBase();
    ~AlgorithmBase();
    void entangle();
    void swap();
    void next_time_slot();
};