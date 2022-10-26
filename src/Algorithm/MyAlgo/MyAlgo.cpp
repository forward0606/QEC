#include "MyAlgo.h"


MyAlgo::MyAlgo(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, "MY", request_time_limit, node_time_limit, swap_prob, entangle_alpha) {
    if(DEBUG) cerr << "new My" << endl;
}

double MyAlgo::get_weight(int node1_id, int node2_id) {
    return -log(graph.get_channel_weight(node1_id, node2_id));
}

vector<int> MyAlgo::Dijkstra(int src, int dst, vector<vector<int>> &adjacency_list) {
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

        for(int neighbor : adjacency_list[cur_node]) {
            if(distance[cur_node] + get_weight(cur_node, neighbor) < distance[neighbor]) {
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

double MyAlgo::calculate_path_fidelity(vector<int> &path) {
    double fidelity = 1;
    for(int node = 1; node < (int)path.size(); node++) {
        fidelity *= graph.get_channel_weight(path[node - 1], path[node]);
    }
    return fidelity;
}

double MyAlgo::calculate_fidelity(vector<double> paths_fidelity) {
    sort(paths_fidelity.rbegin(), paths_fidelity.rend());
    vector<double> fidelities;
    while(fidelities.size() < 5LL) {
        for(int i = 0; i < (int)paths_fidelity.size() && fidelities.size() < 5LL; i++) {
            fidelities.push_back(paths_fidelity[i]);
        }
    }

    double product = 1;
    for(double fidelity : fidelities) {
        product *= fidelity;
    }

    double sum = product;
    for(double fidelity : fidelities) {
        sum += (product / fidelity) * (1 - fidelity);
    }

    return sum;
}

void MyAlgo::build_all_pair_path() {
    int n = graph.get_size();
    path_table = vector<vector<vector<vector<int>>>>(n, vector<vector<vector<int>>>(n));
    fidelity_table = vector<vector<vector<double>>>(n, vector<vector<double>>(n));
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            path_table[i][j] = find_5_paths(i, j);
            for(vector<int> path : path_table[i][j]) {
                double fidelity = calculate_path_fidelity(path);

                reverse(path.begin(), path.end());
                path_table[j][i].push_back(path);

                fidelity_table[i][j].push_back(fidelity);
                fidelity_table[j][i].push_back(fidelity);
            }
        }
    }
}

int MyAlgo::find_width(vector<int> &path, vector<vector<int>> &capacity, vector<int> &remain_qubits) {
    const double INF = numeric_limits<int>::infinity();
    if(path.size() == 1LL) return INF;

    int src = path[0], dst = path.back();
    int width = min(remain_qubits[src], remain_qubits[dst]);
    for(int i = 1; i < (int)path.size() - 1; i++) {
        int node = path[i];
        width = min(width, remain_qubits[node] / 2);
    }

    for(int i = 1; i < (int)path.size(); i++) {
        int node1 = i - 1, node2 = i;
        width = min(width, capacity[node1][node2]);
    }

    return width;
}

vector<vector<int>> MyAlgo::find_5_paths(int src, int dst) {
    int n = graph.get_size();
    vector<vector<int>> paths;
    int need = 5;

    vector<vector<int>> adjacency_list(n);
    vector<vector<int>> capacity(n, vector<int>(n, 0));
    vector<int> remain_qubits(n);

    for(int node = 0; node < n; node++) {
        remain_qubits[node] = graph.Node_id2ptr(node)->get_memory_cnt();
        for(int neighbor : graph.get_neighbors_id(node)) {
            adjacency_list[node].push_back(neighbor);
            capacity[node][neighbor] = graph.get_channel_size(node, neighbor);
        }
    }

    while((int)paths.size() < need) {
        vector<int> path = Dijkstra(src, dst, adjacency_list);
        if(path.empty()) break;
        int width = find_width(path, capacity, remain_qubits);
        
        for(int times = 0; times < width && paths.size() < 5LL; times++) {
            paths.push_back(path);
        }

        for(int i = 0; i < (int)path.size() - 1; i++) {
            int node1 = path[i], node2 = path[i + 1];
            capacity[node1][node2] -= width;
            capacity[node2][node1] -= width;
            if (capacity[node1][node2] <= 0) {
                adjacency_list[node1].erase(find(adjacency_list[node1].begin(), adjacency_list[node1].end(), node2));
                adjacency_list[node2].erase(find(adjacency_list[node2].begin(), adjacency_list[node2].end(), node1));
            }
        }

        remain_qubits[src]--;
        remain_qubits[dst]--;
        if(remain_qubits[src] <= 0 || remain_qubits[dst] <= 0) {
            break;
        }

        for(int i = 1; i < (int)path.size() - 1; i++) {
            int node = path[i];
            remain_qubits[node] -= 2;
            if(remain_qubits[node] <= 1) {
                for(int neightbor : adjacency_list[node]) {
                    adjacency_list[neightbor].erase(find(adjacency_list[neightbor].begin(), adjacency_list[neightbor].end(), node));
                }
                adjacency_list[i].clear();
            }
        }
    }

    return paths;
}

vector<vector<int>> MyAlgo::get_paths(int src, int dst) {
    return path_table[src][dst];
}

double MyAlgo::get_fidelity_with_remain_qubits(int src, int dst) {
    int width = min(graph.Node_id2ptr(src)->get_remain(), graph.Node_id2ptr(dst)->get_remain());
    width = min(width, 5);
    if(width == 0) return 0;

    auto first = fidelity_table[src][dst].begin();
    auto last = first + width;

    if((int)fidelity_table[src][dst].size() < width) {
        cerr << "fidelity and width error" << endl;
        exit(1);
    }

    return calculate_fidelity(vector<double>(first, last));
}

double MyAlgo::get_max_fidelity_1_path(int src, int dst) {
    return fidelity_table[src][dst][0];
}

vector<int> MyAlgo::find_path_on_Social(int src, int dst) { 
    // use Dijkstra

    vector<int> subgraph_node;

    subgraph_node.push_back(src);
    subgraph_node.push_back(dst);
    for(int node = 0; node < graph.get_size(); node++) {
        if(node == src || node == dst) continue;
        if(graph.is_trusted(src, node)) {
            subgraph_node.push_back(node);
        }
    }

    int n = (int)subgraph_node.size();

    vector<int> id_to_index(n);
    for(int i = 0; i < n; i++) {
        id_to_index[subgraph_node[i]] = i;
    }

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    vector<int> parent(n, -1);
    vector<bool> used(n, false);
    vector<double> fidelity(n, 0);
    pq.push({1, id_to_index[src]}); // {fidelity, src}

    while(pq.empty()) {
        int cur_node = pq.top().second;
        pq.pop();
        if(used[cur_node]) continue;

        for(auto next_node : subgraph_node) {
            int node_id = subgraph_node[cur_node], next_id = subgraph_node[next_node];
            double fidelity_this_path = max(get_max_fidelity_1_path(node_id, cur_node), get_fidelity_with_remain_qubits(node_id, next_id));
            if(fidelity[next_node] < fidelity_this_path) {
                fidelity[next_node] = fidelity_this_path;
                parent[next_node] = cur_node;
            }
        }
    }

    int cur_node = id_to_index[dst];
    vector<int> path_on_trusted;
    while(cur_node != -1) {
        path_on_trusted.push_back(subgraph_node[cur_node]);
        cur_node = parent[cur_node];
    }

    reverse(path_on_trusted.begin(), path_on_trusted.end());
    return path_on_trusted;
}

void MyAlgo::path_assignment() {
    for(Request &request : requests) {
        int src = request.get_source(), dst = request.get_destination();
        whole_requests.emplace_back(request.get_source(), request.get_destination(), request.get_time_limit(), find_path_on_Social(src, dst));
    }
    requests.clear();

    for(WholeRequest request : whole_requests) {
        if(request.subrequest.empty()) {
            // find path
            int temp = request.get_current_temporary();
            int src = request.trusted_node_path[temp], dst = request.trusted_node_path[temp + 1];

            vector<vector<int>> paths = get_paths(src, dst);
            vector<Path*> sufficient_paths;
            vector<double> sufficient_fidelities;
            for(int path_id = 0; path_id < (int)paths.size(); path_id++) {
                int width = AlgorithmBase::find_width(paths[path_id]);
                if(width >= 1) {
                    sufficient_fidelities.push_back(fidelity_table[src][dst][path_id]);
                    sufficient_paths.push_back(graph.build_path(paths[path_id]));
                }
            }

            double fidelity = calculate_fidelity(sufficient_fidelities);
            double fidelity_threshold = max(get_max_fidelity_1_path(src, dst), calculate_fidelity(fidelity_table[src][dst]));
            

            if(fidelity >= fidelity_threshold) {
                for(Path* &path : sufficient_paths) {
                    request.subrequest.emplace_back(src, dst, request.get_time_limit());
                    request.subrequest.back() += path;
                }
            } else {
                for(Path* &path : sufficient_paths) {
                    path->release();
                }
            }
            

        }
    }
}

void MyAlgo::entangle() {
    AlgorithmBase::base_entangle();
}
void MyAlgo::swap() {
    AlgorithmBase::base_swap();
}
void MyAlgo::send() {
    AlgorithmBase::base_send();
}

void MyAlgo::next_time_slot() {
    graph.refresh();
    // graph.release();
    for(Request &request: requests){
        request.next_timeslot();
    }

    vector<int> finished_reqno;
    for(int reqno = 0; reqno < (int)requests.size(); reqno++) {
        if(!requests[reqno].is_finished()) {
            continue;
        }
        finished_reqno.push_back(reqno);
        if(requests[reqno].is_success()){
            throughputs += 1;
            //result["waiting_time"] += requests[reqno].get_waiting_time(); segmentation fault
        }
    }

    reverse(finished_reqno.begin(), finished_reqno.end());

    for(int reqno : finished_reqno) {
        requests.erase(requests.begin() + reqno);
    }
}