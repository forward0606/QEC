#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <omp.h>
#include "Network/Graph/Graph.h"
#include "Algorithm/AlgorithmBase/AlgorithmBase.h"
#include "Algorithm/Greedy/Greedy.h"
#include "Algorithm/QCAST/QCAST.h"
#include "Algorithm/REPS/REPS.h"
using namespace std;

#define ALGCO_GREEDY 0
#define ALGCO_QCAST 1
#define ALGCO_REPS 2
#define ALGO_CNT 3

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

    int round = 1;

    double swap_prob = 1, entangle_alpha = 0.002;
    int node_time_limit = 7;

    int new_request_min = 1, new_request_max = 5;
    int request_time_limit = 7;
    int total_time_slot = 10;

    bool debug = false;
    // python generate graph
    // #pragma omp parallel for
    for(int T = 0; T < round; T++){
        stringstream ss;
        string round_str;
        ss << T;
        ss >> round_str;
        ofstream ofs;
        ofs.open("Round " + round_str + " log.txt");

        time_t now = time(0);
        char* dt = ctime(&now);
        cerr  << "時間 " << dt << endl << endl; 
        ofs  << "時間 " << dt << endl << endl; 

        string filename = "input.txt";
        // string command = "python3 main.py ";
        // if(system((command + filename).c_str()) != 0){
        //     cerr<<"error:\tsystem proccess python error"<<endl;
        //     exit(1);
        // }
        if(debug) filename = "debug_graph.txt";
        int num_of_node;
        ifstream graph_input;
        graph_input.open (filename);
        graph_input >> num_of_node;
        graph_input.close();
        //Graph graph("input.txt", num_of_node, min_channel, max_channel, min_memory_cnt, max_memory_cnt, node_time_limit, swap_prob, entangle_alpha, true);
        Greedy greedy(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
        QCAST qcast(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
        REPS reps(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
        
        ofs<<"---------------in round " <<T<<" -------------" <<endl;
        for(int t = 0; t < total_time_slot; t++){
            ofs<<"---------------in timslot " <<t<<" -------------" <<endl;
            //亂數引擎, to decide how many requests received in this timeslot 
            random_device rd;
            default_random_engine generator = default_random_engine(rd());
            uniform_int_distribution<int> unif(new_request_min, new_request_max);
            int request_cnt = unif(generator);

            cerr<< "---------generating requests in main.cpp----------" << endl;
            for(int q = 0; q < request_cnt; q++){
                Request new_request = generate_new_request(num_of_node, request_time_limit);
                // Request new_request = generate_new_request(4, 3, request_time_limit);
                cerr<<q << ". source: " << new_request.get_source()<<", destination: "<<new_request.get_destination()<<endl;
                greedy.requests.push_back(new_request);
                qcast.requests.push_back(new_request);
                reps.requests.push_back(new_request);
            }
            cerr<< "---------generating requests in main.cpp----------end" << endl;
            
            //#pragma omp parallel for
            for(int algo_id = 0; algo_id < ALGO_CNT; algo_id++){
                if(algo_id == ALGCO_GREEDY){
                    ofs<<"-----------run greedy---------"<<endl;
                    greedy.run();
                    ofs<<"-----------run greedy---------end"<<endl;
                    greedy.next_time_slot();
                }else if(algo_id == ALGCO_QCAST){
                    ofs<<"-----------run qcast----------"<<endl;
                    qcast.run();
                    ofs<<"-----------run qcast----------end"<<endl;
                    qcast.next_time_slot();
                }else if(algo_id == ALGCO_REPS){
                    ofs<<"-----------run REPS----------"<<endl;
                    reps.run();
                    ofs<<"-----------run REPS----------end"<<endl;
                    reps.next_time_slot();
                }
            }
            ofs<<"(greedy)total throughput = "<<greedy.total_throughput()<<endl;
            ofs<<"(QCAST)total throughput = "<<qcast.total_throughput()<<endl;
            ofs<<"(REPS)total throughput = "<<reps.total_throughput()<<endl;
            ofs<<"---------------in timslot " <<t<<" -------------end" <<endl;
        }
        ofs<<"---------------in round " <<T<<" -------------end" <<endl;
        ofs << endl;
        ofs<<"(greedy)total throughput = "<<greedy.total_throughput()<<endl;
        ofs<<"(QCAST)total throughput = "<<qcast.total_throughput()<<endl;
        ofs<<"(REPS)total throughput = "<<reps.total_throughput()<<endl;

        now = time(0);
        dt = ctime(&now);
        cerr  << "時間 " << dt << endl << endl; 
        ofs  << "時間 " << dt << endl << endl; 
        ofs.close();
    }
    return 0;
}