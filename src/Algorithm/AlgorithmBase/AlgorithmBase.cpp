#include "AlgorithmBase.h"



AlgorithmBase::AlgorithmBase(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :timeslot(0), waiting_time(0), throughputs(0), time_limit(request_time_limit), swap_prob(swap_prob), graph(Graph(filename, node_time_limit, swap_prob, entangle_alpha)){
    
}
<<<<<<< HEAD
AlgorithmBase::~AlgorithmBase(){
    cerr<<"delete AlgorithmBase"<<endl;
=======

AlgorithmBase::~AlgorithmBase(){
    if(DEBUG)cerr<<"delete AlgorithmBase"<<endl;
>>>>>>> origin/new_start
}

double AlgorithmBase::get_swap_prob(){
    return swap_prob;
}

void AlgorithmBase::next_time_slot(){
    graph.refresh();
    graph.release();
    for(auto &request: requests){
        request.next_timeslot();
    }
}

void AlgorithmBase::entangle(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].entangle();
    }
}

void AlgorithmBase::swap(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].swap();
    }
}


void AlgorithmBase::run(){
    path_assignment();
    entangle();
    swap();
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
    bool vis[graph.get_size()];
    int parent[graph.get_size()];
    for(int i = 0; i < graph.get_size(); i++){
        vis[i] = false;
        parent[i] = -1;
    }

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
    int sum = 0;
    for(auto &request:requests){
        sum += request.get_throughput();
    }
    return sum;
}