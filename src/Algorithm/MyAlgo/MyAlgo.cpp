#include "MyAlgo.h"


MyAlgo::MyAlgo(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, "MyAlgo", request_time_limit, node_time_limit, swap_prob, entangle_alpha) {
    if(DEBUG) cerr << "new My" << endl;
    build_all_pair_path();
}

MyAlgo::~MyAlgo() {
    if(DEBUG) cerr << "delete My";
    for(WholeRequest &whole_request : whole_requests) {
        whole_request.clear();
    }
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
        used[cur_node] = true;
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

            if(path_table[i][j].size() == 0LL) {
                cerr<<"path_table["<<i<<"]["<<j<<"]:";
                cerr << "can't find path error\n";
                exit(1);
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
        int node1 = path[i - 1], node2 = path[i];
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

        remain_qubits[src] -= width;
        remain_qubits[dst] -= width;
        if(remain_qubits[src] <= 0 || remain_qubits[dst] <= 0) {
            break;
        }

        for(int i = 1; i < (int)path.size() - 1; i++) {
            int node = path[i];
            remain_qubits[node] -= 2 * width;
            if(remain_qubits[node] <= 1) {
                for(int neighbor : adjacency_list[node]) {
                    adjacency_list[neighbor].erase(find(adjacency_list[neighbor].begin(), adjacency_list[neighbor].end(), node));
                }
                adjacency_list[node].clear();
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
    width = min(width, (int)fidelity_table[src][dst].size());
    if(width == 0) return 0;

    auto first = fidelity_table[src][dst].begin();
    auto last = first + width;

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

    vector<int> id_to_index(graph.get_size());
    for(int i = 0; i < n; i++) {
        id_to_index[subgraph_node[i]] = i;
    }

    priority_queue<pair<double, int>> pq;
    vector<int> parent(n, -1);
    vector<bool> used(n, false);
    vector<double> fidelity(n, 0);

    fidelity[id_to_index[src]] = 1;
    pq.push({1, id_to_index[src]}); // {fidelity, src}

    while(!pq.empty()) {
        int node_index = pq.top().second;
        pq.pop();
        if(used[node_index]) continue;
        used[node_index] = true;
        for(int next_index = 0; next_index < (int)subgraph_node.size(); next_index++) {
            if(next_index == node_index) continue;
            int cur_node = subgraph_node[node_index], next_node = subgraph_node[next_index];
            double fidelity_this_path = fidelity[node_index] * max(get_max_fidelity_1_path(cur_node, next_node), get_fidelity_with_remain_qubits(cur_node, next_node));
            if(fidelity[next_index] < fidelity_this_path) {
                fidelity[next_index] = fidelity_this_path;
                parent[next_index] = node_index;
                pq.push({fidelity[next_index], next_index});
            }
        }
    }
    if(parent[id_to_index[dst]] == -1){
        cerr<<"error:\tno path found!"<<endl;
        exit(1);
    }
    int cur_node_index = id_to_index[dst];
    vector<int> path_on_trusted;
    while(cur_node_index != -1) {
        path_on_trusted.push_back(subgraph_node[cur_node_index]);
        cur_node_index = parent[cur_node_index];
    }

    reverse(path_on_trusted.begin(), path_on_trusted.end());
    return path_on_trusted;
}

void MyAlgo::calculate_before_path_assignment() {
    if(!whole_requests.empty()) res["active_timeslot"]++;
    res["waiting_time"] += whole_requests.size();
}

void MyAlgo::path_assignment() {

    for(Request &request : requests) {
        int src = request.get_source(), dst = request.get_destination();
        whole_requests.emplace_back(request.get_source(), request.get_destination(), request.get_time_limit(), find_path_on_Social(src, dst));
        res["divide_cnt"] += whole_requests.back().get_trust_node_path_length();
    }
    requests.clear();
    
    calculate_before_path_assignment();

    for(WholeRequest &request : whole_requests) {
        if(request.subrequests.empty()) {
            // find path
            int temp = request.get_current_temporary();
            int src = request.trusted_node_path[temp], dst = request.trusted_node_path[temp + 1];

            vector<vector<int>> paths = get_paths(src, dst);
            vector<Path*> sufficient_paths;
            vector<double> sufficient_fidelities;

            int first_path_id = -1;
            for(int path_id = 0; path_id < (int)paths.size(); path_id++) {
                int width = AlgorithmBase::find_width(paths[path_id]);
                if(width >= 1) {
                    if(first_path_id == -1) first_path_id = path_id;
                    sufficient_fidelities.push_back(fidelity_table[src][dst][path_id]);
                    sufficient_paths.push_back(graph.build_path(paths[path_id]));
                }
            }

            if(sufficient_paths.empty()) continue;

            double fidelity = calculate_fidelity(sufficient_fidelities);
            double fidelity_threshold = max(get_max_fidelity_1_path(src, dst), calculate_fidelity(fidelity_table[src][dst]));
            
            if(fidelity >= fidelity_threshold && graph.Node_id2ptr(src)->get_remain() >= 5 && \
                (int)sufficient_paths.size() + graph.Node_id2ptr(dst)->get_remain() >= 5) {
                request.set_encode(true);
                (*graph.Node_id2ptr(src)) -= 4;
                (*graph.Node_id2ptr(dst)) -= (5 - (int)sufficient_paths.size());

                res["encode_cnt"]++;
                for(int path_index = 0; path_index < (int)sufficient_paths.size(); path_index++) {
                    request.subrequests.emplace_back(new SubRequest(src, dst, request.get_time_limit()));
                }
                for(int path_index = 0; path_index < (int)sufficient_paths.size(); path_index++) {
                    *(request.subrequests[path_index]) += sufficient_paths[path_index];
                }
            } else {
                request.set_encode(false);
                for(Path* &path : sufficient_paths) {
                    path->release();
                    delete path;
                }

                if(sufficient_fidelities[0] >= fidelity_threshold) {
                    res["unencode_cnt"]++;
                    request.subrequests.emplace_back(new SubRequest(src, dst, request.get_time_limit()));
                    Path* single_path = graph.build_path(paths[first_path_id]);
                    *(request.subrequests.back()) += single_path;
                }
            }
        }
    }
}

void MyAlgo::entangle() {
    for(WholeRequest &whole_request : whole_requests) {
        for(SubRequest* subrequest : whole_request.subrequests) {
            subrequest->entangle();
        }
    }
}
void MyAlgo::swap() {
    for(WholeRequest &whole_request : whole_requests) {
        for(SubRequest* subrequest : whole_request.subrequests) {
            subrequest->swap();
        }
    }
}
void MyAlgo::send() {
    for(WholeRequest &whole_request : whole_requests) {
        for(SubRequest* subrequest : whole_request.subrequests) {
            subrequest->send();
        }
    }
}

void MyAlgo::next_time_slot() {
    // graph.release();
   
    for(WholeRequest &whole_request : whole_requests) {
        whole_request.try_forward();
    }

    vector<int> finished_reqno;
    for(int reqno = 0; reqno < (int)whole_requests.size(); reqno++) {
        if(whole_requests[reqno].is_finished()) {
            res["finished_throughputs"]++;
            res["path_length"] += whole_requests[reqno].path_length;
            path_length_encode[whole_requests[reqno].path_length] += whole_requests[reqno].get_encode_cnt();
            path_length_cnt[whole_requests[reqno].path_length] += 1;
            res["fidelity"] += whole_requests[reqno].fidelity;
            map<int, int> single_path_count = whole_requests[reqno].get_use_path_count();
            for(auto e : single_path_count){
                num_of_path_count[e.first] += e.second;
            }
            
            finished_reqno.push_back(reqno);
            if(whole_requests[reqno].is_success()) {
                res["throughputs"]++;
            }
        }
    }

    reverse(finished_reqno.begin(), finished_reqno.end());

    for(int reqno : finished_reqno) {
        whole_requests.erase(whole_requests.begin() + reqno);
    }

    graph.refresh();
    for(WholeRequest &whole_request : whole_requests) {
        for(SubRequest* subrequest : whole_request.subrequests) {
            subrequest->refresh_paths();
        }
    }
}