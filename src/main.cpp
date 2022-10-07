#include <iostream>
#include <queue>
#include<algorithm>
#include "Network/Graph/Graph.h"
#include "Algorithm/AlgorithmBase/AlgorithmBase.h"
#include "Algorithm/QCAST/QCAST.h"
using namespace std;

double dis[26];
int parent[26];

Request generate_new_request(int num_of_node, int time_limit){
    //亂數引擎 
    random_device rd;
    default_random_engine generator = default_random_engine(rd());
    uniform_int_distribution<int> unif(0, num_of_node-1);
    int node1 = unif(generator), node2 = unif(generator);
    while(node1 == node2) node2 = unif(generator);
    
    return Request(node1, node2, time_limit);
}

Request generate_new_request(int node1, int node2, int time_limit){//demo
    return Request(node1, node2, time_limit);
}


int main(){
    int num_of_node = 5;
    int min_channel = 1, max_channel = 1;
    int min_memory_cnt = 10, max_memory_cnt = 14;
    int swap_prob = 1, entangle_alpha = 1;
    int node_time_limit = 7;

    int new_request_min = 1, new_request_max = 1;
    int request_time_limit = 7;
    int total_time_slot = 1;

    Graph graph("input.txt", num_of_node, min_channel, max_channel, min_memory_cnt, max_memory_cnt, node_time_limit, swap_prob, entangle_alpha);
    AlgorithmBase base(graph, request_time_limit, swap_prob);
    QCAST qcast(base);
    // vector<double> path;
    // path.emplace_back(1);
    // path.emplace_back(0.5);
    // cout<<"EXT:"<<qcast.demoEXT(path, 2)<<endl;
    for(int t = 0; t < total_time_slot; t++){
        //亂數引擎, to decide how many requests received in this timeslot 
        random_device rd;
        default_random_engine generator = default_random_engine(rd());
        uniform_int_distribution<int> unif(new_request_min, new_request_max);
        //int request_cnt = unif(generator);

        // cout<<"request:\n";
        // for(int q = 0; q < request_cnt; q++){
        //     Request new_request = generate_new_request(num_of_node, request_time_limit);
        //     cout<<new_request.get_source()<<" "<<new_request.get_destination()<<endl;
        //     qcast.requests.push_back(new_request);
        // }
        Request new_request = generate_new_request(0, 2, request_time_limit);           //demo
        qcast.requests.push_back(new_request);
        new_request = generate_new_request(2, 4, request_time_limit);           //demo
        qcast.requests.push_back(new_request);
        new_request = generate_new_request(0, 4, request_time_limit);           //demo
        qcast.requests.push_back(new_request);
        qcast.run();

        qcast.next_time_slot();
    }

    return 0;
}