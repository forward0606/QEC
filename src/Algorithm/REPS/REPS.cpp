#include    "REPS.h"
#include <set>
#include <cmath>
using namespace std;

REPS::REPS(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha){
    //f_hat.resize(graph.get_size());
    if(DEBUG) cerr<<"new REPS"<<endl;
}


void REPS::PFT_LP(vector<double> &t_plum, vector<map<pair<int, int>, double>> &f_plum){
    
    //return value is store in t_plum and f_plum
    t_plum.clear();
    f_plum.clear();
    
    //do LP
    try {      
        // Create an environment
        GRBEnv env = GRBEnv(true);
        env.set("OutputFlag", "0");
        env.start();

        // Create an empty model
        GRBModel model = GRBModel(env);

        // Create variables
        
        
        vector<map<pair<int, int>, GRBVar>> f(requests.size());    //fi(u, v)
        vector<int> neighbor;
        for(int i=0;i<(int)requests.size();i++){
            for(int u = 0;u<(int)graph.get_size();u++){
                neighbor = graph.get_neighbors_id(u);
                for(int v:neighbor){
                    f[i][make_pair(u, v)] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "f" + to_string(i) + "("+to_string(u)+", "+to_string(v) + ")");
                }
            }
        }


        vector<GRBVar> t;   //ti
        for(int i=0;i<(int)requests.size();i++){
            t.emplace_back(model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "t"+to_string(i)));
        }

        map<pair<int, int>, GRBVar> x; //x(u, v)
        for(int u = 0;u<(int)graph.get_size();u++){
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                x[make_pair(u, v)] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "x("+to_string(u)+", "+to_string(v) + ")");
            }
        }

        // Set objective: 
        // maximize sum(ti)
        GRBLinExpr expr = 0;
        for (auto ti:t)
            expr += ti;
        
        model.setObjective(expr, GRB_MAXIMIZE);

        // Add constraint: 1(a) 
        for(int i=0;i<(int)requests.size();i++){
            GRBLinExpr expr = 0;
            int u = requests[i].get_source();
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                expr += f[i][make_pair(u, v)];
                expr -= f[i][make_pair(v, u)];
            }
            model.addConstr(expr == t[i], "1a_" + to_string(i));
        }

        // Add constraint: 1(b) 
        for(int i=0;i<(int)requests.size();i++){
            expr = 0;
            int u = requests[i].get_destination();
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                expr += f[i][make_pair(u, v)];
                expr -= f[i][make_pair(v, u)];
            }
            model.addConstr(expr == GRBLinExpr(t[i], -1.0), "1b_"+ to_string(i));
        }

        // Add constraint: 1(c) 
        for(int i=0;i<(int)requests.size();i++){
            expr = 0;
            for(int u=0;u<graph.get_size();u++){
                if(u == requests[i].get_source())continue;
                if(u == requests[i].get_destination())continue;
                neighbor = graph.get_neighbors_id(u);
                for(int v:neighbor){
                    expr += f[i][make_pair(u, v)];
                    expr -= f[i][make_pair(v, u)];
                }
                model.addConstr(expr == 0, "1c(" + to_string(i) + ", " + to_string(u) + ")");
            }
        }

        // Add constraint: 1(d) 
        for(int u=0;u<(int)graph.get_size();u++){
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                expr = 0;
                for(int i=0;i<(int)requests.size();i++){
                    expr += f[i][make_pair(u, v)];
                    expr += f[i][make_pair(v, u)];
                }
                double p = graph.get_channel_weight(u, v);
                model.addConstr(expr <= (x[make_pair(u, v)] * p), "1d(" + to_string(u) + ", " + to_string(v) + ")");
            }
        }

        // Add constraint: 1(e) 
        for(int u=0;u<(int)graph.get_size();u++){
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                int c = graph.remain_resource_cnt(u, v);
                model.addConstr(x[make_pair(u, v)] <= c, "1e(" + to_string(u) + ", " + to_string(v) + ")");
            }
        }

        // Add constraint: 1(f) 
        for(int u=0;u<(int)graph.get_size();u++){
            expr = 0;
            int m = graph.Node_id2ptr(u)->get_remain();
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                expr += x[make_pair(u, v)];
            }
            model.addConstr(expr <= m, "1f(" + to_string(u) + ")");
        }

        // Optimize model
        model.optimize();

        //get t
        for(int i=0;i<(int)requests.size();i++){
            t_plum.emplace_back(t[i].get(GRB_DoubleAttr_X));
        }

        //get fi
        f_plum.resize(requests.size());
        for(int i=0;i<(int)requests.size();i++){
            for(int u = 0;u<(int)graph.get_size();u++){
                neighbor = graph.get_neighbors_id(u);
                for(int v:neighbor){
                    f_plum[i][make_pair(u, v)] = f[i][make_pair(u, v)].get(GRB_DoubleAttr_X);
                }
            }
        }
        cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }catch(...) {
        cout << "Exception during optimization" << endl;
    }
}

void REPS::EPS_LP(vector<vector<double>> &t_bar, vector<vector<map<pair<int, int>, double>>> &f_bar){
    //return value is store in t_plum and f_plum
    t_bar.clear();
    f_bar.clear();
    
    //do LP
    try {

        // Create an environment
        GRBEnv env = GRBEnv(true);
        env.set("OutputFlag", "0");
        env.start();

        // Create an empty model
        GRBModel model = GRBModel(env);

        // Create variables
        vector<vector<map<pair<int, int>, GRBVar>>> f(requests.size());    //fik(u, v)
        for(int i=0;i<(int)requests.size();i++){
            f[i].resize(requests[i].get_paths().size());
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                Path *pik = requests[i].get_paths()[k];
                vector<Node *> nodes = pik->get_nodes();
                int u, v;
                u = nodes[0]->get_id();
                for(int j=1;j < (int)nodes.size();j++){
                    v = nodes[j]->get_id();
                    f[i][k][make_pair(u, v)] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "f" + to_string(i) + " " + to_string(k) + "("+to_string(u)+", "+to_string(v) + ")");
                    f[i][k][make_pair(v, u)] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "f" + to_string(i) + " " + to_string(k) + "("+to_string(v)+", "+to_string(u) + ")");
                    u = v;
                }
            }
        }


        vector<vector<GRBVar>> t(requests.size());   //tik
        for(int i=0;i<(int)requests.size();i++){
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                t[i].emplace_back(model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "t"+to_string(i) + " " + to_string(k)));
            }
        }

        // Set objective: 
        // maximize sum(ti)
        GRBLinExpr expr = 0;
        for (auto ti:t){
            for(auto tik:ti){
                expr += tik;
            }
        }
        model.setObjective(expr, GRB_MAXIMIZE);


        // Add constraint: 2(a) 
        for(int i=0;i<(int)requests.size();i++){
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                expr = 0;
                Path *pik = requests[i].get_paths()[k];
                vector<Node *> nodes = pik->get_nodes();
                int u, v;
                u = nodes[0]->get_id();
                v = nodes[1]->get_id();
                expr += f[i][k][make_pair(u, v)];
                expr -= f[i][k][make_pair(v, u)];
                model.addConstr(expr == t[i][k], "2a_" + to_string(i));
            }
        }


        // Add constraint: 2(b) 
        for(int i=0;i<(int)requests.size();i++){
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                expr = 0;
                Path *pik = requests[i].get_paths()[k];
                vector<Node *> nodes = pik->get_nodes();
                int u, v;
                u = nodes.back()->get_id();
                v = nodes[nodes.size()-2]->get_id();
                expr += f[i][k][make_pair(u, v)];
                expr -= f[i][k][make_pair(v, u)];
                model.addConstr(expr == GRBLinExpr(t[i][k], -1.0), "2b_" + to_string(i));
            }
        }
        


        // Add constraint: 2(c) 
        for(int i=0;i<(int)requests.size();i++){
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                expr = 0;
                Path *pik = requests[i].get_paths()[k];
                vector<Node *> nodes = pik->get_nodes();
                int u, v;
                u = nodes[1]->get_id();
                for(int j=2;j<(int)nodes.size()-1;j++){
                    v = nodes[j]->get_id();
                    expr += f[i][k][make_pair(u, v)];
                    expr -= f[i][k][make_pair(v, u)];
                    u = v;
                }
                model.addConstr(expr == 0, "2c_" + to_string(i));
            }
        }
        

        // Add constraint: 2(d) 
        for(int u=0;u<(int)graph.get_size();u++){
            vector<int> neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                if(u > v)continue;
                expr = 0;
                int entangle_succ = 0;
                for(int i=0;i<(int)requests.size();i++){
                    for(int k=0;k<(int)requests[i].get_paths().size();k++){
                        Path *pik = requests[i].get_paths()[k];
                        vector<Node *> nodes = pik->get_nodes();
                        int ut, vt;
                        ut = nodes[0]->get_id();
                        for(int j=1;j < (int)nodes.size();j++){
                            vt = nodes[j]->get_id();
                            if(ut == u && vt == v){
                                expr += f[i][k][make_pair(u, v)];
                                if(pik->get_channels()[j-1]->is_entangled()){
                                    entangle_succ++;
                                }
                            }else if(vt == u && ut == v){
                                expr += f[i][k][make_pair(v, u)];
                                if(pik->get_channels()[j-1]->is_entangled()){
                                    entangle_succ++;
                                }
                            }
                            ut = vt;
                        }
                    }
                }
                model.addConstr(expr <=entangle_succ, "2d(" + to_string(u) + ", " + to_string(v) + ")");
            }
        }
        
        // model.update();
        // model.write("debug.lp");
        model.optimize();
        
        cout << "EPS_Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
        
        //get f
        f_bar.resize(requests.size());
        for(int i=0;i<(int)requests.size();i++){
            f_bar[i].resize(requests[i].get_paths().size());
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                Path *pik = requests[i].get_paths()[k];
                vector<Node *> nodes = pik->get_nodes();
                int u, v;
                u = nodes[0]->get_id();
                for(int j=1;j < (int)nodes.size();j++){
                    v = nodes[j]->get_id();
                    f_bar[i][k][make_pair(u, v)] = f[i][k][make_pair(u, v)].get(GRB_DoubleAttr_X);
                    u = v;
                }
            }
        }

        //get t
        t_bar.resize(requests.size());
        for(int i=0;i<(int)requests.size();i++){
            t_bar[i].resize((int)requests[i].get_paths().size());
            for(int k=0;k<(int)requests[i].get_paths().size();k++){
                t_bar[i][k] = t[i][k].get(GRB_DoubleAttr_X);
            }
        }

    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }catch(...) {
        cout << "Exception during optimization" << endl;
    }
     
}

void REPS::EPS(){
    cout<<"EPS begin!"<<endl;
    vector<vector<double>> t_bar;
    vector<vector<map<pair<int, int>, double>>> f_bar;
    EPS_LP(t_bar, f_bar);
    
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<double> dis(0.0, 1.0);
    
    double width;
    vector<int> path_nodes;
    vector<tuple<double, vector<int>>> p; // width(double), req_no, path
    
    EPS_P.clear();
    EPS_P.resize(requests.size());
    for(int i=0;i<(int)t_bar.size();i++){
        for(int k=0;k<(int)t_bar[i].size();k++){
            p.clear();
            if(dis(gen) > t_bar[i][k]){
                continue;
            }
            while(true){
                bool set_flag;
                tie(path_nodes, width, set_flag) = DFS(i, f_bar[i][k], false);
                if(width == -1) break;
                p.push_back(tie(width, path_nodes));
            }
            double choose_path_prob = dis(gen) * t_bar[i][k];
            for(int i=0;i<(int)p.size();i++){
                tie(width, path_nodes) = p[i];
                if(choose_path_prob - width <= 1e-6){
                    EPS_P[i].emplace_back(path_nodes);
                    break;
                }
                choose_path_prob -= width;
            }
        }
    }
    cout<<"EPS finished!"<<endl;
}


void REPS::path_assignment(){
    //PFT Using Progressive Rounding
    vector<double> t_plum;
    vector<map<pair<int, int>, double>> f_plum;
    PFT_LP(t_plum, f_plum);
    vector<int> neighbor;
    bool flag = true;
    double width;
    vector<int> path_nodes;
    vector<tuple<double, int, vector<int> >> p; // width(double), req_no, path

    while(flag){
        p.clear();
        flag = false;
        for(int i = 0; i < (int)requests.size(); i++){
            while(true){
                bool set_flag; // true if assign path in DFS
                tie(path_nodes, width, set_flag) = DFS(i, f_plum[i]);
                if(width == -1) break;
                if(set_flag) flag = true;
                if(width > 1e-6) p.push_back(tie(width, i, path_nodes));
            }
        }
        sort(p.begin(), p.end());
        int req_no;
        for(int i = p.size()-1; i >= 0; i--){
            tie(width, req_no, path_nodes) = p[i];
            width = find_width(path_nodes);
            while(width-- > 1e-6){
                // for(int j = 0; j < (int)path_nodes.size()-1; j++){
                //     f_hat[req_no][make_pair(path_nodes[j], path_nodes[j+1])] ++;
                // }
                requests[req_no] += graph.build_path(path_nodes);
            }
        }
        PFT_LP(t_plum, f_plum);
    }
}


void REPS::swap(){
    //EPS based on Randomized Rounding
    EPS();
    //Entanglement Link Selection (ELS)
    ELS();
    map<pair<int, int>, vector<Channel*>> remain_channels;
    for(auto &request: requests){
        vector<Path*> path = request.get_paths();
        for(int path_id = 0; path_id < (int)path.size(); path_id++){
            vector<Channel*> channels = path[path_id]->get_channels();
            for(auto channel_ptr: channels){
                if(channel_ptr->is_entangled()){
                    Node *node1 = channel_ptr->get_node1_ptr(), *node2 = channel_ptr->get_node2_ptr();
                    remain_channels[make_pair(node1->get_id(), node2->get_id())].emplace_back(channel_ptr);
                }
            }
        }
        request.clear_paths();
    }
    for(int i = 0; i < (int)requests.size(); i++){
        for(auto path_nodes: ELS_P[i]){
            requests[i] += find_swap_path(path_nodes, remain_channels);
        }
    }
    AlgorithmBase::swap();
}

tuple<vector<int>, double, bool> REPS::DFS(int req_no, map<pair<int, int>, double>&f_plum_i, bool is_assign_path /*= true*/){
    int source = requests[req_no].get_source();
    int destination = requests[req_no].get_destination();
    vector<int> vis;
    vis.resize(graph.get_size(), false);
    vector<int> path_nodes;
    path_nodes.clear();
    int now = source;
    double mn = 0x3f3f3f3f3f3f3f3f;
    while(now != destination){
        vis[now] = true;
        path_nodes.push_back(now);
        vector<int> neighbors = graph.get_neighbors_id(now);
        bool flag = true;
        for(int neighbor: neighbors){
            if(vis[neighbor] || f_plum_i[make_pair(now, neighbor)] < 1e-6) continue;
            flag = false;
            mn = min(mn, f_plum_i[make_pair(now, neighbor)]);
            now = neighbor;
            break;
        }
        if(flag) break;
    }
    
    if(mn == 0x3f3f3f3f3f3f3f3f) { // no path
        path_nodes.clear();
        return make_tuple(path_nodes, -1, false); 
    }
    path_nodes.push_back(now); // destination
    for(int i = 0; i < (int)path_nodes.size()-1; i++){
        int u = path_nodes[i], v = path_nodes[i+1];
        f_plum_i[make_pair(u, v)] -= mn;
        //f_hat[req_no][make_pair(u, v)] += (int)mn;
    }
    if(!is_assign_path){
        return make_tuple(path_nodes, mn, false);
    }
    int width = (int)mn;
    bool set_flag = true;
    while(width--){
        set_flag = false;
        requests[req_no] += graph.build_path(path_nodes);
    }
    
    
    return make_tuple(path_nodes, mn-(int)mn, set_flag);
}

void REPS::ELS(){
    cout<<"ELS begin!"<<endl;
    ELS_P.clear();
    ELS_P.resize(requests.size());
    // line 3
    vector<vector<int>> y;
    y.resize(graph.get_size());
    for(int i = 0; i < graph.get_size(); i++){
        for(int j = 0; j < graph.get_size(); j++){
            y[i].push_back(0);
        }
    }
    set<int> T;
    for(int i = 0; i < (int)requests.size(); i++){
        T.insert(i);
    }

    // line 4
    while(!T.empty()){
        for(int req_no = 0; req_no < (int)requests.size(); req_no++){
            int l = 0; // stay EPS_P which has no (u, v) that y[u][v] >= e[u][v]
            for(int k = 0; k < (int)EPS_P[req_no].size(); k++){
                bool stay_path = true;
                for(int i = 0; i < (int)EPS_P[req_no][k].size()-1; i++){
                    int u = EPS_P[req_no][k][i], v = EPS_P[req_no][k][i+1];
                    if(y[u][v] >= graph.get_channel_entangle_succ_cnt(u, v)) stay_path = false; //remove path
                }
                if(stay_path) {
                    EPS_P[req_no][l] = EPS_P[req_no][k];
                    l++;
                }
            }
            EPS_P[req_no].resize(l);
        }
        // line 6
        int req_no = 0, mn = 0x3f3f3f3f;
        for(int i = 0; i < (int)requests.size(); i++){
            if(T.count(i) != 0 && (int)ELS_P[i].size() < mn){
                mn = ELS_P[i].size();
                req_no = i;
            }
        }

        // line 7
        if(EPS_P[req_no].size() != 0){
            double mn_q = 0x3f3f3f3f3f3f3f3f;
            int r = 0;
            for(int k = 0; k < (int)EPS_P[req_no].size(); k++){
                double sum = 0;
                for(int node_id: EPS_P[req_no][k]){
                    Node *node_ptr = graph.Node_id2ptr(node_id);
                    sum += (-log(node_ptr->get_swap_prob()));
                }
                if(sum < mn_q){
                    mn_q = sum;
                    r = k;
                }
            }
            ELS_P[req_no].push_back(EPS_P[req_no][r]); 
            for(int i = 0; i < (int)EPS_P[req_no][r].size()-1; i++){
                int u = EPS_P[req_no][r][i], v = EPS_P[req_no][r][i+1];
                y[u][v] ++;
                y[v][u] ++;
            }
            continue;
        }
        T.erase(req_no);
    }

    // line 14
    for(int i = 0; i < (int)requests.size(); i++){
        T.insert(i);
    }
    // line 15
    while(!T.empty()){
        // line 16
        int req_no = 0, mn = 0x3f3f3f3f;
        for(int i = 0; i < (int)requests.size(); i++){
            if(T.count(i) != 0 && (int)ELS_P[i].size() < mn){
                mn = ELS_P[i].size();
                req_no = i;
            }
        }
        // line 17
        vector<int> parent(graph.get_size(), -1);
        vector<bool> vis(graph.get_size(), false);
        queue<int> q;
        q.push(requests[req_no].get_source());
        int now;
        while(!q.empty()){
            now = q.front();
            q.pop();
            vis[now] = true;
            if(now == requests[req_no].get_destination()) break;
            vector<int> neighbors = graph.get_neighbors_id(now);
            for(int neighbor: neighbors){
                if(vis[now] || y[now][neighbor] >= graph.get_channel_entangle_succ_cnt(now, neighbor)) continue;
                parent[neighbor] = now;
                q.push(neighbor);
            }
        }
        if(now != requests[req_no].get_destination()){
            T.erase(req_no);
            continue;
        }
        // line 22
        vector<int> path_nodes;
        while(now != -1){
            //cout << "now = " << now << endl;
            path_nodes.push_back(now);
            now = parent[now];
        }
        reverse(path_nodes.begin(), path_nodes.end());
        ELS_P[req_no].push_back(path_nodes);
        for(int i = 0; i < (int)path_nodes.size(); i++){
            int u = path_nodes[i], v = path_nodes[i+1];
            y[u][v] ++;
            y[v][u] ++;
        }
    }
    cout<<"ELS finished!"<<endl;
}
