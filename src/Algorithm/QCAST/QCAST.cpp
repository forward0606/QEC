#include "QCAST.h"

QCAST::QCAST(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha){
    cerr<<"new QCAST"<<endl;
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

void QCAST::assign_resource(vector<int> path, int reqno){
    
    cerr<< "---------QCAST::assign_resource----------" << endl;
    int width = find_width(path);
    cerr<<"find a path to build! with width:    "<<width<<endl;
    for(auto ele:path){
        cerr<<ele<<" ";
    }
    cerr << endl;
    if(width == 0x3f3f3f3f){
        cerr<<"error:\twidth = INF"<<endl;
        exit(1);
    }
    while(width-- > 0){
        requests[reqno] += graph.build_path(path);
    }
    cerr<< "---------QCAST::assign_resource----------end" << endl;
}



struct CandPath{
    vector<int> path;
    double ext;
    CandPath(vector<int> path, double ext):
        path(path), ext(ext){}
    CandPath():ext(0){}
    bool operator<(const CandPath& right)const{
        return ext < right.ext;
    }
    void print(){
        //demo
        cerr<<"\tpath:\t";
        for(int ele:path){
            cerr<<ele<<" ";
        }
        cerr<<endl;
        cerr<<"\text = "<<ext<<endl;
    }
};


void QCAST::path_assignment(){
    cerr<< "---------QCAST::path_assignment----------end" << endl;
    const int maximum_major_path_per_request = 200;
    const int maximum_path_length = 200;
    const int maximum_total_number_of_path = 200;
    int total = 0;
    while(total < maximum_total_number_of_path){
        vector<CandPath> candidate(requests.size());
        double dis[graph.get_size()];
        int parent[graph.get_size()];
        vector<int> neighbors;
        for(int reqno = 0;reqno<(int)requests.size();reqno++){   //find the best path for every request
            Request &request = requests[reqno];
            if(request.get_paths().size() > maximum_major_path_per_request){
                //force to find no path
                candidate[reqno] = CandPath();
                continue;
            }
            for(int i=0;i<(int)graph.get_size();i++){//initialize the distance
                dis[i] = -1;
                parent[i] = -1;
            }
            priority_queue<pair<double, int>> pq;
            pq.emplace(0x3f3f3f3f, request.get_source());
            dis[request.get_source()] = 0x3f3f3f3f;
            int node = -1;
            double distance = 0;
            while(!pq.empty()){
                node = pq.top().second;
                distance = pq.top().first;
                pq.pop();
                if(node == request.get_destination()){
                    //find the Best path of request
                    break;
                }
                if(dis[node] > distance){
                    continue;
                }
                
                //find the path, width, ext
                vector<int> path_nodes;
                vector<double> path_prob;
                int now = node;
                while(now != -1){
                    // cout<<"in find path: now = "<<now<<endl;
                    path_nodes.push_back(now); 
                    if(now != request.get_source()){
                        path_prob.emplace_back(graph.get_channel_weight(parent[now], now));
                    }
                    now = parent[now];
                }
                reverse(path_nodes.begin(), path_nodes.end());
                reverse(path_prob.begin(), path_prob.end());
                if(path_nodes.size() > maximum_path_length){
                    continue;
                }
                neighbors = graph.get_neighbors_id(node);
                for(int ele:neighbors){
                    //cout<<ele<<" ";
                    bool node1_is_repeater = !(node == request.get_source() || node == request.get_destination());
                    bool node2_is_repeater = !(ele == request.get_source() || ele == request.get_destination());
                    if(graph.remain_resource_cnt(node, ele, node1_is_repeater, node2_is_repeater) == 0){
                        continue;
                    }
                    path_nodes.emplace_back(ele);
                    path_prob.emplace_back(graph.get_channel_weight(node, ele));
                    int width = find_width(path_nodes);
                    double ext = EXT(path_prob, width);
                    if(dis[ele] < ext){
                        dis[ele] = ext;
                        parent[ele] = node;
                        pq.emplace(ext, ele);
                    }
                    path_nodes.pop_back();
                    path_prob.pop_back();
                }
            }
            if(node != request.get_destination()){
                //path is not found
                candidate[reqno] = CandPath();
                break;
            }
            //find the path, width, ext
            //path
            vector<int> path_nodes;
            int now = node;
            while(now != -1){
                path_nodes.push_back(now); 
                now = parent[now];
            }
            reverse(path_nodes.begin(), path_nodes.end());
            candidate[reqno] = CandPath(path_nodes, distance);
        }
        //find the best path in requests
        CandPath mx = CandPath();
        int mx_reqno = -1;
        for(int reqno=0;reqno<(int)requests.size();reqno++){
            if(mx < candidate[reqno]){
                mx = candidate[reqno];
                mx_reqno = reqno;
            }
        }

        if(mx_reqno == -1){//no path found
            break;
        }
        total += find_width(candidate[mx_reqno].path);
        assign_resource(candidate[mx_reqno].path, mx_reqno);
    }

    find_recovery_path(3);
    cerr<< "---------QCAST::path_assignment----------end" << endl;
}





void QCAST::find_recovery_path(int R){ // R: max amount of recovery path for any node x 
    for(auto &request: requests){
        for(auto path_ptr: request.get_paths()){
            Path &path = *path_ptr;
            vector<Node*> path_nodes = path.get_nodes();
            // vector<Node*> bad_path_nodes = bad_path.get_nodes();
            // for(auto ele:bad_path_nodes) cout << ele << ' ';
            // cout << endl;
            for(int i = 0; i < (int)path_nodes.size(); i++){
                int recovery_path_cnt = 0;
                Path *recovery_path_ptr = nullptr;
                for(int hop = 1; i + hop <= path.get_len(); hop++){
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

void QCAST::entangle(){
    AlgorithmBase::entangle();
    for(auto rps:recovery_paths){
        for(auto p:rps.second){
            p->entangle();
        }
    }
}


void QCAST::p4(){
    cerr<<"--------QCAST::p4--------"<<endl;
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
    for(auto &request:requests){
        for(auto path:request.get_paths()){
            //find swaping path
            vector<vector<int>> adj(graph.get_size());
            
            for(auto& c:path->get_channels()){
                if(c->is_entangled()){
                    Node *node1 = c->get_node1_ptr(), *node2 = c->get_node2_ptr();
                    adj[node1->get_id()].emplace_back(node2->get_id());
                    adj[node2->get_id()].emplace_back(node1->get_id());
                }
            }
            
            for(auto rec:recovery_paths[make_pair(&request, path)]){
                vector<Node*> recn = rec->get_nodes();
                Node *node1 = recn.back(), *node2 = recn[0];
                adj[node1->get_id()].emplace_back(node2->get_id());
                adj[node2->get_id()].emplace_back(node1->get_id());
            }
            
            vector<bool> vis(graph.get_size());
            vector<int> parent(graph.get_size());
            stack<int> st;
            for(int i=0;i<graph.get_size();i++){
                vis[i] = false;
                parent[i] = -1;
            }
            int source = request.get_source(), destination = request.get_destination();
            st.push(source);
            vis[source] = true;
            while (!st.empty()){
                int now = st.top();
                st.pop();
                for(auto neighbor: adj[now]){
                    if(vis[neighbor]) continue;
                    parent[neighbor] = now;
                    st.push(neighbor);
                    vis[neighbor] = true;
                }
            }
            cerr<<"parent["<<destination<<"] = "<<parent[destination]<<endl;
            if(parent[destination] == -1) continue;

            vector<int> path_nodes;
            int now = destination;
            while(now != -1){
                path_nodes.push_back(now); 
                now = parent[now];
            }
            reverse(path_nodes.begin(), path_nodes.end());
            
            vector<Node *> nodes;
            cerr << "path to swap in P4: ";
            for(int i:path_nodes){
                nodes.emplace_back(graph.Node_id2ptr(i));
                cerr <<  i << ' ';
            }
            cerr << '\n';

            //swap path
            bool swap_succ = true;
            for(int i=1;i<(int)nodes.size()-1;i++){
                Node* node = nodes[i];
                swap_succ &= node->swap();
            }
            if(swap_succ){
                request.add_one_throughput();
            }
        }
    }
    cerr<<"---------------------------"<<endl;
}

void QCAST::swap(){
    p4();
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

double QCAST::demoEXT(vector<double> path, int w){
    return EXT(path, w);
}