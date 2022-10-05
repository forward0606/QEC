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
    
    return Request(unif(generator), unif(generator), time_limit);
}

int main(){
    int num_of_node = 500;
    int min_channel = 3, max_channel = 7;
    int min_memory_cnt = 10, max_memory_cnt = 14;
    int swap_prob = 1, entangle_alpha = 1;
    int node_time_limit = 7;

    int new_request_min = 0, new_request_max = 5;
    int request_time_limit = 7;
    int total_time_slot = 100;

    Graph graph("input.txt", num_of_node, min_channel, max_channel, min_memory_cnt, max_memory_cnt, node_time_limit, swap_prob, entangle_alpha);
    AlgorithmBase base(graph, request_time_limit, swap_prob);
    QCAST qcast(base);

    for(int t = 0; t < total_time_slot; t++){
        //亂數引擎, to decide how many requests received in this timeslot 
        random_device rd;
        default_random_engine generator = default_random_engine(rd());
        uniform_int_distribution<int> unif(new_request_min, new_request_max);
        int request_cnt = unif(generator);

        for(int q = 0; q < request_cnt; q++){
            Request new_request = generate_new_request(num_of_node, request_time_limit);
            qcast.requests.push_back(new_request);
        }

        qcast.next_time_slot();
    }

    return 0;
}