#include "MyGreedyAlgo.h"

MyGreedyAlgo::MyGreedyAlgo(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, "MyGreedyAlgo", request_time_limit, node_time_limit, swap_prob, entangle_alpha){
    if(DEBUG) cerr<<"new MyGreedyAlgo"<<endl;
}

void MyGreedyAlgo::path_assignment(){
    base_test_active();
    for(int reqno = 0; reqno < (int)requests.size(); reqno++){
        Request &request = requests[reqno];
        vector<int> path = Dijkstra(request.get_source(), request.get_destination());
        if(path.size() == 0 && find_width(path) == 0) continue;
        requests[reqno] += graph.build_path(path);
    }
}

void MyGreedyAlgo::entangle(){
    AlgorithmBase::base_entangle();
}
void MyGreedyAlgo::swap(){
    AlgorithmBase::base_swap();
}
void MyGreedyAlgo::send(){
    AlgorithmBase::base_send();
}
void MyGreedyAlgo::next_time_slot(){
    AlgorithmBase::base_next_time_slot();
}

double MyGreedyAlgo::get_weight(int node1_id, int node2_id) {
    return -log(graph.get_channel_weight(node1_id, node2_id));
}


vector<int> MyGreedyAlgo::Dijkstra(int src, int dst) {
    const double INF = numeric_limits<double>::infinity();
    int n = graph.get_size();
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
        used[cur_node] = true;
        auto neighbors = graph.get_neighbors_id(cur_node);
        for(int neighbor : neighbors) {
            if(graph.remain_resource_cnt(cur_node, neighbor) > 0 && distance[cur_node] + get_weight(cur_node, neighbor) < distance[neighbor]) {
                distance[neighbor] = distance[cur_node] + get_weight(cur_node, neighbor);
                parent[neighbor] = cur_node;
                pq.push({distance[neighbor], neighbor});
            }
        }
    }

    if(distance[dst] >= INF) return {};

    int cur_node = dst;
    vector<int> path;
    while(cur_node != -1) {
        path.push_back(cur_node);
        cur_node = parent[cur_node];
    }

    reverse(path.begin(), path.end());

    return path;
}