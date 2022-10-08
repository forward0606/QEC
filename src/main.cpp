#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>
#include "Network/Graph/Graph.h"
#include "Algorithm/AlgorithmBase/AlgorithmBase.h"
#include "Algorithm/QCAST/QCAST.h"
#include "Algorithm/Greedy/Greedy.h"
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
    cout  << "時間 " << dt << endl << endl; 

    double swap_prob = 1, entangle_alpha = 0.02;
    int node_time_limit = 7;

    int new_request_min = 1, new_request_max = 5;
    int request_time_limit = 7;
    int total_time_slot = 5;

    bool debug = true;
    // python generate graph
    string filename = "input.txt";
    if(debug) filename = "debug_graph.txt";
    string command = "python3 main.py ";
    if(system((command + filename).c_str()) != 0){
        cerr<<"error:\tsystem proccess python error"<<endl;
        exit(1);
    }
    int num_of_node;
    ifstream graph_input;
    graph_input.open (filename);
    graph_input >> num_of_node;

    //Graph graph("input.txt", num_of_node, min_channel, max_channel, min_memory_cnt, max_memory_cnt, node_time_limit, swap_prob, entangle_alpha, true);
    Greedy greedy(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
    QCAST qcast(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);

    
    for(int t = 0; t < total_time_slot; t++){
        //亂數引擎, to decide how many requests received in this timeslot 
        random_device rd;
        default_random_engine generator = default_random_engine(rd());
        uniform_int_distribution<int> unif(new_request_min, new_request_max);
        int request_cnt = unif(generator);

        cerr<< "---------generating requests in main.cpp----------" << endl;
        for(int q = 0; q < request_cnt; q++){
            Request new_request = generate_new_request(num_of_node, request_time_limit);
            //Request new_request = generate_new_request(3, 4, request_time_limit);
            cerr<<q << ". source: " << new_request.get_source()<<", destination: "<<new_request.get_destination()<<endl;
            greedy.requests.push_back(new_request);
            qcast.requests.push_back(new_request);
        }
        cerr<< "---------generating requests in main.cpp----------end" << endl;

        greedy.run();
        qcast.run();
        greedy.next_time_slot();
        qcast.next_time_slot();
        cout<<"-------------------------in round "<<t<<"-------------" <<endl;
        cout<<"(greedy)total throughput = "<<greedy.total_throughput()<<endl;
        cout<<"(QCAST)total throughput = "<<qcast.total_throughput()<<endl;
    }
    cout << endl;
    cout<<"(greedy)total throughput = "<<greedy.total_throughput()<<endl;
    cout<<"(QCAST)total throughput = "<<qcast.total_throughput()<<endl;
    return 0;
}