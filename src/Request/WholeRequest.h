#ifndef __WHOLEREQUEST_H
#define __WHOLEREQUEST_H
#include    <iostream>
#include    <vector>
#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include    "../../config.h"
using namespace std;
using SubRequest = Request;

class WholeRequest:public Request {
    int current_temporary; // index of node in trusted_node_path
    bool divide_to_5_qubits;
public:
    vector<int> trusted_node_path;
    vector<SubRequest> subrequest;
    WholeRequest(int source, int destination, int time_limit, vector<int> trusted_node_path);
    int get_current_temporary();
    bool is_divide();
    void set_divide(bool status);
};

#endif