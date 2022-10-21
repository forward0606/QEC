#include "My.h"
#include <limits>
My::My(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha) {
    if(DEBUG) cerr<<"new My"<<endl;
}

double My::get_weight(int node1_id, int node2_id) {
    return -log(graph.get_channel_weight(node1_id, node2_id));
}

void My::build_all_pair_path() {
    const double INF = numeric_limits<double>::infinity();;
    int n = (int)graph.get_size();
    vector<vector<double>> distance(n, vector<double>(n, INF));
    split_node = vector<vector<int>> (n, vector<int>(n, -1));

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(i == j) distance[i][j] = 0;
            else distance[i][j] = get_weight(i, j);
        }
    }

    for(int k = 0; k < n; k++) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(distance[i][j] > distance[i][k] + distance[k][j]) {
                    split_node[i][j] = k;
                    distance[i][j] = distance[i][k] + distance[k][j];
                }
            }
        }
    }
}

vector<int> My::get_path(int src, int dst) {

    int split = split_node[src][dst];
    if(split == -1) return {};
    vector<int> path;
    vector<int> subpath1 = get_path(src, split);
    vector<int> subpath2 = get_path(split, dst);

    path.push_back(src);
    for(int node : subpath1) path.push_back(node);
    path.push_back(split);
    for(int node : subpath2) path.push_back(node);
    path.push_back(dst);

    return path;
}

void My::path_assignment() {
    for(Request &request : requests) {
        
    }
}

void My::next_time_slot() {
    graph.refresh();
    graph.release();
    for(auto &request: requests){
        request.next_timeslot();
    }

    vector<int> finished_reqno;
    for(int reqno = 0; reqno < (int)requests.size(); reqno++) {
        if(requests[reqno].get_throughput() >= 1) {
            finished_reqno.push_back(reqno);
        }
    }

    reverse(finished_reqno.begin(), finished_reqno.end());
    throughputs += (int)finished_reqno.size();

    for(int reqno : finished_reqno) {
        requests.erase(requests.begin() + reqno);
    }
}

WholeRequest::WholeRequest(int source, int destination, vector<int> trusted_node_path)
    : source(source), destination(destination), trusted_node_path(trusted_node_path) {
    if(DEBUG)cerr<<"new WholeRequest"<<endl;
}


