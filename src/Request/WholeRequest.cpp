#include "WholeRequest.h"

WholeRequest::WholeRequest(int source, int destination, int time_limit, vector<int> trusted_node_path)
    : Request(source, destination, time_limit), trusted_node_path(trusted_node_path), divide_to_5_qubits(false), current_temporary(0), finished_qubits(0) {
    if(DEBUG)cerr<<"new WholeRequest"<<endl;
}

int WholeRequest::get_current_temporary() {
    return current_temporary;
}

bool WholeRequest::is_divide() {
    return divide_to_5_qubits;
}

void WholeRequest::set_divide(bool status) {
    divide_to_5_qubits = status;
}