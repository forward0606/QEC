#include    "REPS.h"

REPS::REPS(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha){
    if(DEBUG) cerr<<"new REPS"<<endl;
}


void REPS::PFT(vector<double> &t_plum, vector<vector<vector<double>>> &f_plum){
    t_plum.clear();
    f_plum.clear();
    
     //do LP
    try {      
        // Create an environment
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "pft.log");
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
                model.addConstr(expr == 0, "1c_" + to_string(i) + ", " + to_string(u));
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
                model.addConstr(expr <= (x[make_pair(u, v)] * p), "1d");
            }
        }

        // Add constraint: 1(e) 
        for(int u=0;u<(int)graph.get_size();u++){
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                int c = graph.get_channel_size(u, v);
                model.addConstr(x[make_pair(u, v)] <= c, "1e");
            }
        }

        // Add constraint: 1(f) 
        for(int u=0;u<(int)graph.get_size();u++){
            expr = 0;
            int m = graph.Node_id2ptr(u)->get_memory_cnt();
            neighbor = graph.get_neighbors_id(u);
            for(int v:neighbor){
                expr += x[make_pair(u, v)];
            }
            model.addConstr(expr <= m, "1f");
        }



        // model.addConstr(x + 2 * y + 3 * z <= 4, "c0");

        // // Add constraint: x + y >= 1
        // model.addConstr(x + y >= 1, "c1");

        // // Optimize model
        // model.optimize();

        // cout << x.get(GRB_StringAttr_VarName) << " "
        //     << x.get(GRB_DoubleAttr_X) << endl;
        // cout << y.get(GRB_StringAttr_VarName) << " "
        //     << y.get(GRB_DoubleAttr_X) << endl;
        // cout << z.get(GRB_StringAttr_VarName) << " "
        //     << z.get(GRB_DoubleAttr_X) << endl;

        // cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch(...) {
    cout << "Exception during optimization" << endl;
  }
}
void REPS::EPS(){}
void REPS::ELS(){}
void REPS::path_assignment(){
    //PFT Using Progressive Rounding
}
void REPS::entangle(){}
void REPS::swap(){
    //EPS based on Randomized Rounding

    //Entanglement Link Selection (ELS)
}
void REPS::next_time_slot(){}
