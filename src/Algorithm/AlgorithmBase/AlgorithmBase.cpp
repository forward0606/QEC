#include "AlgorithmBase.h"



AlgorithmBase::AlgorithmBase(string filename, string algorithm_name, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :algorithm_name(algorithm_name), timeslot(0), waiting_time(0), time_limit(request_time_limit), swap_prob(swap_prob), graph(Graph(filename, node_time_limit, swap_prob, entangle_alpha)){
    
}

AlgorithmBase::~AlgorithmBase(){
    if(DEBUG)cerr<<"delete AlgorithmBase"<<endl;
}

double AlgorithmBase::get_swap_prob(){
    return swap_prob;
}

void AlgorithmBase::base_next_time_slot(){
    graph.refresh();
    graph.release();
    for(auto &request: requests){
        request.next_timeslot();
    }

    //好強
    vector<int> finished_reqno;
    for(int reqno = 0; reqno < (int)requests.size(); reqno++) {
        if(!requests[reqno].is_finished()) {
            continue;
        }
        finished_reqno.push_back(reqno);
        if(requests[reqno].is_success()){
            res["throughputs"]++;
            //result["waiting_time"] += requests[reqno].get_waiting_time(); segmentation fault
        }
    }

    reverse(finished_reqno.begin(), finished_reqno.end());
    for(int reqno : finished_reqno) {
        requests.erase(requests.begin() + reqno);
    }
}

void AlgorithmBase::base_entangle(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].entangle();
    }
}

void AlgorithmBase::base_swap(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].swap();
    }
}
void AlgorithmBase::base_send(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].send();
    }
}

void AlgorithmBase::run(){
    path_assignment();
    entangle();
    swap();
    send();
    cout<<algorithm_name<<" run end"<<endl;
}

int AlgorithmBase::find_width(vector<int> path){
    if(path.size() < 2){
        return 0x3f3f3f3f;
    }
    if(path.size() == 2){
        return graph.remain_resource_cnt(path[0], path[1], false, false);
    }
    int path_size = (int)path.size();
    int width = graph.remain_resource_cnt(path[0], path[1], false);
    for(int i=2;i<path_size-1;i++){
        width = min(width, graph.remain_resource_cnt(path[i-1], path[i]));
    }
    width = min(width, graph.remain_resource_cnt(path[path_size-2], path[path_size-1], true, false));
    return width;
}

void AlgorithmBase::assign_resource(vector<int> path, int reqno){
    
    if(DEBUG) cerr<< "---------AlgorithmBase::assign_resource----------" << endl;
    int width = find_width(path);
    if(DEBUG) {
        cerr<<"find a path to build! with width:    "<<width<<endl;
        for(auto ele:path){
            cerr<<ele<<" ";
        }
        cerr << endl;
    }
    if(width == 0x3f3f3f3f){
        cerr<<"error:\twidth = INF"<<endl;
        exit(1);
    }
    while(width-- > 0){
        requests[reqno] += graph.build_path(path);
    }
    if(DEBUG) cerr<< "---------AlgorithmBase::assign_resource----------end" << endl;
}

vector<int> AlgorithmBase::BFS(int source, int destination){
    // if(source > graph.get_size() || destination > graph.get_size()){
    //     cerr<<"error:\tnode id is out of range"<<endl;
    //     exit(1);
    // }
    vector<bool> vis(graph.get_size()+5, false);
    vector<int> parent(graph.get_size()+5, -1);
    // for(int i = 0; i < graph.get_size(); i++){
    //     vis[i] = false;
    //     parent[i] = -1;
    // }

    queue<int> q;
    q.push(source);
    vis[source] = true;
    while (!q.empty()){
        int now = q.front();
        q.pop();
        vector<int> neighbors = graph.get_neighbors_id(now);
        for(auto neighbor: neighbors){
            if(vis[neighbor]) continue;
            bool is1_repeater = true, is2_repeater = true;
            if(now == source) is1_repeater = false;
            if(neighbor == destination) is2_repeater = false;
            if(graph.remain_resource_cnt(now, neighbor, is1_repeater, is2_repeater) > 0){
                parent[neighbor] = now;
                q.push(neighbor);
                vis[neighbor] = true;
            }
        }
    }
    vector<int> path_nodes;
    path_nodes.clear();
    if(parent[destination] == -1) return path_nodes;

    int now = destination;
    while(now != -1){
        path_nodes.push_back(now); 
        now = parent[now];
    }
    reverse(path_nodes.begin(), path_nodes.end());
    return path_nodes;
}

int AlgorithmBase::total_throughput(){
    return res["throughputs"];
}

Path* AlgorithmBase::find_swap_path(vector<int> path_nodes, map<pair<int, int>, vector<Channel*>> &remain_channels){
    //the the path for swaping
    //cout << "AlgorithmBase::find_swap_path" << endl;
    
    
    //find the channels of path for swaping 
    vector<Channel*> path_channels;
    int u, v;
    for(int i=1;i<(int)path_nodes.size();i++){
        u = path_nodes[i-1];
        v = path_nodes[i];
        if(u > v){
            int swap_tmp = u;
            u = v;
            v = swap_tmp;
        }
        if(remain_channels[make_pair(u, v)].empty()){
            cerr<<"error:\t remain channel is not sufficient"<<endl;
            exit(1);
        }
        path_channels.emplace_back(remain_channels[make_pair(u, v)].back());
        remain_channels[make_pair(u, v)].pop_back();
    }

    //find the nodes of path for swapin
    vector<Node *> nodes;
    if(DEBUG) cerr << "path to swap in P4: ";
    for(int i:path_nodes){
        nodes.emplace_back(graph.Node_id2ptr(i));
        if(DEBUG) cerr <<  i << ' ';
    }
    if(DEBUG) cerr << '\n';
    return new Path(nodes, path_channels);
}

string AlgorithmBase::get_name(){
    return algorithm_name;
}

double AlgorithmBase::get_res(string s){
    return res[s];
}