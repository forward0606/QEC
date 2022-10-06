#include "QCAST.h"

QCAST::QCAST(AlgorithmBase base)
    :AlgorithmBase(base){
    
}

double QCAST::EXT(vector<double> path, int w){
    if(path.size() == 0){
        return 0x3f3f3f3f;
    }
    vector<vector<double>> Q(path.size()); //Q[i][j] the prob of exactly j links success in ith edge on path 
    vector<vector<double>> P(path.size());
    for(int i=0;i<(int)path.size();i++){
        Q[i].resize(w+1);
        P[i].resize(w+1);
    }
    for(int k=0;k<(int)path.size();k++){
        for(int i=0;i<=w;i++){
            Q[k][i] = C(w, i) * pow(path[k], i) * pow(1-path[k], w-i);
        }
    }
    for(int i=0;i<=w;i++){      //P0i = q0i
        P[0][i] = Q[0][i];
    }
    for(int k=1;k<(int)path.size();k++){
        for(int i=0;i<=w;i++){
            P[k][i] = 0;
            for(int l = i;l<=w;l++){
                P[k][i] += P[k-1][i] * Q[k][l];
            }
            for(int l = i+1;l<=w;l++){
                P[k][i] += Q[k][i] * P[k-1][l];
            }
        }
    }

    //Et = q^h * sum(i * P[h][i])
    double ext = 0;
    for(int i=1;i<=w;i++){
        ext += i * P.back()[i];
    }
    return ext * pow(get_swap_prob(), path.size());
}

double QCAST::C(int n, int m){
    if(n < m){
        cerr<<"err:\tint QCAST combination m > n"<<endl;
        exit(1);
    }
    if(n == m || m == 0){return 1;}
    if(combination.find(make_pair(n, m)) != combination.end()){
        return combination[make_pair(n, m)];
    }
    combination[make_pair(n, m)] = C(n-1, m) + C(n-1, m-1);
    return combination[make_pair(n, m)];
}

int QCAST::find_width(vector<int> path){
    if(path.size() < 2){
        return 0x3f3f3f3f;
    }
    int path_size = (int)path.size();
    int width = graph.remain_resource_cnt(path[0], path[1], false);
    for(int i=2;i<path_size-1;i++){
        width = min(width, graph.remain_resource_cnt(path[i-1], path[i]));
    }
    width = min(width, graph.remain_resource_cnt(path[path_size-2], path[path_size-1], true, false));
    return width;
}

void QCAST::assign_resource(vector<int> path, int reqno){
    
    int width = find_width(path);
    while(width-->0){
        requests[reqno] += graph.build_path(path);
    }
}



struct CandPath{
    int id, source, destination;
    vector<double> p;
    vector<int> path;
    bool useable;
    CandPath(int id, int s, int d, vector<double> p, vector<int> path):
        id(id), source(s), destination(d), p(p), path(path), useable(true){}
    CandPath(int id, int s, int d):
        id(id), source(s), destination(d), useable(true){
            path.emplace_back(s);
    }
    CandPath(){};
    CandPath(const CandPath& copy){
        *this = copy;
    }
    CandPath& operator=(const CandPath& copy){
        id = copy.id;
        source = copy.source;
        destination = copy.destination;
        p = copy.p;
        path = copy.path;
        useable = copy.useable;
        return *this;
    }
    bool vis(int node){
        for(int i=0;i<(int)path.size();i++){
            if(path[i] == node){
                return true;
            }
        }
        return false;
    }
};



void QCAST::path_assignment(){
    vector<CandPath> candiate;
    for(int i=0;i<(int)requests.size();i++){
        candiate.emplace_back(i, requests[i].get_source(), requests[i].get_destination());
    }
    int find_path_cnt = 0;
    vector<int> neightbors;
    while(find_path_cnt < 100){
        //find the path with biggest EXT
        int mxi = -1, mxv = 0, width;
        for(int i = 0;i<(int)candiate.size();i++){
            if(!candiate[i].useable)    continue;
            width = find_width(candiate[i].path);
            if(width == 0 || candiate[i].path.size() >= 200){
                candiate[i].useable = false;
                continue;
            }
            double ext = EXT(candiate[i].p, width);
            if(ext > mxv){
                mxv = ext;
                mxi = i;
            }
        }
        if(mxi == -1){
            //all path is used
            break;
        }
        if(candiate[mxi].destination == candiate[mxi].path.back()){
            //find new paths
            find_path_cnt += find_width(candiate[mxi].path);
            assign_resource(candiate[mxi].path, candiate[mxi].id);
        }else{
            //go one step next
            int node = candiate[mxi].path.back();
            neightbors = graph.get_neighbors_id(node);
            for(int ele:neightbors){
                //cout<<ele<<" ";
                bool node1_is_repeater = !(node == candiate[mxi].source || node == candiate[mxi].destination);
                bool node2_is_repeater = !(ele == candiate[mxi].source || ele == candiate[mxi].destination);
                if(graph.remain_resource_cnt(node, ele, node1_is_repeater, node2_is_repeater) == 0){
                    continue;
                }
                if(candiate[mxi].vis(ele)){
                    continue;
                } 
            
                candiate[mxi].path.emplace_back(ele);
                candiate[mxi].p.emplace_back(graph.get_channel_weight(node, ele));
                candiate.emplace_back(candiate[mxi]);
                candiate[mxi].path.pop_back();
                candiate[mxi].p.pop_back();
            }
        }
        candiate[mxi].useable = false;
        //remove used path
        int l = 0;
        for(int i = 0;i<(int)candiate.size();i++){
            if(candiate[i].useable){
                candiate[l] = candiate[i];
                l++;
            }
        }
        candiate.resize(l);
    }
}

void QCAST::find_recovery_path(int R){ // R: max amount of recovery path for any node x 
    for(auto &request: requests){
        for(auto path_ptr: request.get_paths()){
            Path path = *path_ptr;
                vector<Node*> path_nodes = path.get_nodes();
                for(int i = 0; i < (int)path_nodes.size(); i++){
                    int recovery_path_cnt = 0;
                    Path *recovery_path_ptr = nullptr;
                    for(int hop = 1; hop <= path.get_len(); hop++){
                        while(recovery_path_cnt < R){
                            recovery_path_ptr = BFS(path_nodes[i]->get_id(), path_nodes[i+hop]->get_id());
                            if(recovery_path_ptr == nullptr) break;
                            recovery_paths[make_pair(&request, path_ptr)].push_back(recovery_path_ptr);
                            recovery_path_cnt++;
                            recovery_path_ptr = nullptr;
                        }
                    }
                }
            
        }
    }
}

void QCAST::p4(){
    
    //delete unsuccess recovery path
    for(auto &recs:recovery_paths){
        int l = 0;
        for(int i=0;i<(int)recs.second.size();i++){
            Path *path = recs.second[i];
            if(path->get_entangle_succ()){
                recs.second[l] = recs.second[i];
                l++;
            }else{
                delete recs.second[i];
            }
        }
    }
    for(int i=0;i<(int)requests.size();i++){

    }
}

Path* QCAST::BFS(int source, int destination){
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
            if(vis[now]) continue;
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
    if(parent[destination] == -1) return nullptr;

    vector<int> path_nodes;
    int now = destination;
    while(now != -1){
        path_nodes.push_back(now); 
        now = parent[now];
    }
    reverse(path_nodes.begin(), path_nodes.end());
    return graph.build_path(path_nodes);
}