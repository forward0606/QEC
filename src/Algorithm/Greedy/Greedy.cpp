#include "Greedy.h"

Greedy::Greedy(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha)
    :AlgorithmBase(filename, "Greedy", request_time_limit, node_time_limit, swap_prob, entangle_alpha){
    if(DEBUG) cerr<<"new QCAST"<<endl;
}

void Greedy::path_assignment(){
    bool flag = true;
    while(flag){
        flag = false;
        for(int reqno = 0; reqno < (int)requests.size(); reqno++){
            Request &request = requests[reqno];
            vector<int> path = BFS(request.get_source(), request.get_destination());
            if(path.size() == 0) continue;
            assign_resource(path, reqno);
            flag = true;
        }
    }
}