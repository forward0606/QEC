#include    <iostream>
#include    <algorithm>
#include    <queue>
#include    <string>
#include    <limits>
#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Request/WholeRequest.h"
#include    "../../Network/Graph/Graph.h"
#include    "../../config.h"

using namespace std;
using SubRequest = Request;

class WholeRequest:public Request {
    vector<int> trusted_node_path;
    int current_temporary; // index of node in trusted_node_path
    bool divdie_to_5_qubits;
public:
    vector<SubRequest> subrequest;
    WholeRequest(int source, int destination, int time_limit, vector<int> trusted_node_path);
    int get_temporary_ith();
    bool is_divide();
};


class MyAlgo:public AlgorithmBase {
    vector<WholeRequest> whole_requests;
    vector<vector<vector<vector<int>>>> path_table; 
    // [node1][node2][path_indx][node_indx]
    vector<vector<vector<double>>> fidelity_table;

    void build_all_pair_path();
    int find_width(vector<int> &path, vector<vector<int>> &capacity, vector<int> &remain_qubits) {
    double get_weight(int node1_id, int node2_id);
    double get_fidelity(int src, int dst);
    double calculate_fidelity(vector<double> paths_fidelity);
    double calculate_path_fidelity(vector<int> &path);

    vector<int> get_5_path(int src, int dst);
    vector<int> get_path(int src, int dst);
    vector<int> Dijkstra(int src, int dst, vector<vector<int>> &capacity);
    vector<int> find_path_on_Social(int src, int dst);
    vector<vector<int>> find_5_paths(int src, int dst);

public:
    MyAlgo(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
    void next_time_slot();
};