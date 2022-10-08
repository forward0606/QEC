#include    <iostream>
#include    <algorithm>
#include    <queue>
#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include "../../config.h"
using namespace std;

class Greedy:public AlgorithmBase{
public:
    Greedy(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
};
