#include    <iostream>
#include    <algorithm>
#include    <queue>
#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include "../../config.h"
using namespace std;
using SubRequest = Request;

class My:public AlgorithmBase{
    vector<WholeRequest> requests;
    void build_all_pair_path();
    vector<int> get_path(int src, int dst);
    double get_weight(int node1_id, int node2_id);
    vector<int> My::Dijkstra(int src, int dst, vector<vector<int>> &capacity);
    vector<vector<int>> My::find_5_paths(int src, int dst);
    vector<vector<vector<vector<int>>>> path_table; // [node1][node2][path_indx][node_indx]
public:
    My(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
    void next_time_slot();
};



class WholeRequest:public Request {
    int source, destination;
    vector<int> trusted_node_path;
    vector<SubRequest> subrequest;

    WholeRequest(int source, int destination, vector<int> trusted_node_path);
};
