#ifndef __WHOLEREQUEST_H
#define __WHOLEREQUEST_H
#include    <iostream>
#include    <vector>
#include    <algorithm>
#include    <cassert>
#include    "../config.h"
#include    "Request.h"
using namespace std;
using SubRequest = Request;                 //genius


class WholeRequest:public Request {
    int current_temporary; // index of node in trusted_node_path
    bool divide_to_5_qubits;
public:
    int finished_qubits;
    int success_qubits;
    double path_length;
    double fidelity;
    vector<int> trusted_node_path;
    vector<SubRequest*> subrequests;
    WholeRequest(int source, int destination, int time_limit, vector<int> trusted_node_path);
    double calculate_subfidelity();
    int get_current_temporary();
    int get_trust_node_path_length();
    bool is_divide();
    bool is_finished();
    bool is_success();
    void set_divide(bool status);
    void try_forward();
    void temporary_forward();
    void clear();
};

#endif