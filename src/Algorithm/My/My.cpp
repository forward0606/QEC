#include "My.h"
#include <limits>
My::My(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, "MY", request_time_limit, node_time_limit, swap_prob, entangle_alpha) {
    if(DEBUG) cerr<<"new My"<<endl;
}

double My::get_weight(int node1_id, int node2_id) {
    return -log(graph.get_channel_weight(node1_id, node2_id));
}

vector<int> My::Dijkstra(int src, int dst, vector<vector<int>> &adjacency_list) {
    const double INF = numeric_limits<double>::infinity();
    int n = (int)graph.get_size();
    vector<double> distance(n, INF);
    vector<int> parent(n, -1);
    vector<bool> used(n, false);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    distance[src] = 0;
    pq.push({0, src});
    while(!pq.empty()) {
        int cur_node = pq.top().second;
        pq.pop();
        if(used[cur_node]) continue;

        for(int neighbor : adjacency_list[cur_node]) {
            if(distance[cur_node] + get_weight(cur_node, neighbor) < distance[neighbor]) {
                distance[neighbor] = distance[cur_node] + get_weight(cur_node, neighbor);
                parent[neighbor] = cur_node;
            }
        }
    }

    int cur_node = dst;
    vector<int> path;
    while(cur_node != -1) {
        path.push_back(cur_node);
        cur_node = parent[cur_node];
    }

    reverse(path.begin(), path.end());

    return path;
}

void My::build_all_pair_path() {
    path_table = vector<vector<vector<vector<int>>>>(n, vector<vector<vector<int>>>(n));

    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {

        }
    }
}

vector<vector<int>> My::find_5_paths(int src, int dst) {
    int n = graph.get_size();
    vector<vector<int>> paths;
    paths.resize(5);
    int need = 5;
    int find = 0;

    vector<vector<int>> adjacency_list;
    vector<int> reamimn_qubits(n);
    adjacency_list.resize(n);

    for(int node = 0; node < n; node++) {
        reamimn_qubits[node] = graph.Node_id2ptr(node)->get_memory_cnt();
        for(int neightbor : graph.get_neighbors_id(node)) {
            adjacency_list[node].push_back(neightbor);
        }
    }


    while(find < need) {
        vector<int> path = Dijkstra(src, dst, )
    }
}

vector<int> My::get_path(int src, int dst) {

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


