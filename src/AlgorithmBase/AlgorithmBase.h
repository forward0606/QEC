#include<iostream>
#include<vector>
#include "Graph/Graph.h"
#include "Request/Request.h"
using namespace std;


class AlgorithmBase{
    Graph graph;
    vector<Request*> requests;
public:
    void entangle();
    void swap();
    void next_time_slot();
};