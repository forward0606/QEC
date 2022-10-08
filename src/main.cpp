#include <iostream>
#include <queue>
#include<algorithm>
#include "Network/Graph/Graph.h"
#include "Algorithm/AlgorithmBase/AlgorithmBase.h"
#include "Algorithm/QCAST/QCAST.h"
using namespace std;

// double dis[26];
// int parent[26];

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
    time_t now = time(0);
    char* dt = ctime(&now);
    cerr  << "時間 " << dt << endl << endl; 


    int num_of_node = 5;
    int min_channel = 2, max_channel = 2;
    int min_memory_cnt = 3, max_memory_cnt = 3;
    double swap_prob = 1, entangle_alpha = 0;
    int node_time_limit = 7;

    int new_request_min = 1, new_request_max = 1;
    int request_time_limit = 7;
    int total_time_slot = 1;

    Graph graph("input.txt", num_of_node, min_channel, max_channel, min_memory_cnt, max_memory_cnt, node_time_limit, swap_prob, entangle_alpha, true);
    cerr<< "---------show graph in main.cpp----------" << endl;
    Node* node_ptr;
    for(int i = 0; i < num_of_node; i++){
        node_ptr = graph.Node_id2ptr(i);
        node_ptr->print();
    }
    cerr<< "---------show graph in main.cpp----------end" << endl;
    AlgorithmBase base(graph, request_time_limit, swap_prob);
    QCAST qcast(base);
    
    for(int t = 0; t < total_time_slot; t++){
        //亂數引擎, to decide how many requests received in this timeslot 
        random_device rd;
        default_random_engine generator = default_random_engine(rd());
        uniform_int_distribution<int> unif(new_request_min, new_request_max);
        int request_cnt = unif(generator);

        cerr<< "---------generating requests in main.cpp----------" << endl;
        for(int q = 0; q < request_cnt; q++){
            // Request new_request = generate_new_request(num_of_node, request_time_limit);
            Request new_request = generate_new_request(3, 4, request_time_limit);
            cerr<<q << ". source: " << new_request.get_source()<<", destination: "<<new_request.get_destination()<<endl;
            qcast.requests.push_back(new_request);
        }
        cerr<< "---------generating requests in main.cpp----------end" << endl;

        qcast.run();
        qcast.next_time_slot();
    }
    cout<<"total throughput = "<<qcast.total_throughput()<<endl;
    return 0;
}