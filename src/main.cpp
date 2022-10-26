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
// #include "Algorithm/REPS/REPS.h"
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
    string file_path = "../data/";

    map<string, double> default_setting;
    default_setting["swap_prob"] = 1;
    default_setting["entangle_alpha"] = 0;
    default_setting["node_time_limit"] = 7;
    default_setting["new_request_min"] = 5;
    default_setting["new_request_max"] = 12;
    default_setting["request_time_limit"] = 7;
    default_setting["total_time_slot"] = 1;

    map<string, vector<double>> change_parameter;
    change_parameter["swap_prob"] = {1.0, 0.7, 0.8, 0.9};
    change_parameter["entangle_alpha"] = {0.02, 0.002, 0};

    vector<string> X_names = {"swap_prob", "entangle_alpha"};
    vector<string> Y_names = {"waiting_time", "throughtputs"};
    // init result
    for(string X_name : X_names) {
        for(string Y_name : Y_names){
            string filename = X_name + "_" + Y_name + ".txt";
            fstream file( file_path + filename, ios::out );
        }
    }
    

    int round = 1;
    for(string X_name : X_names) {
        map<string, double> input_parameter = default_setting;
        map<string, map<string, double>> result;

        for(double change_value : change_parameter[X_name]) {
            input_parameter[X_name] = change_value;
            
            double swap_prob = input_parameter["swap_prob"], entangle_alpha = input_parameter["entangle_alpha"];

            int node_time_limit = input_parameter["node_time_limit"];

            int new_request_min = input_parameter["new_request_min"], new_request_max = input_parameter["new_request_max"];
            int request_time_limit = input_parameter["request_time_limit"];
            int total_time_slot = input_parameter["total_time_slot"];

            bool debug = false;
            // python generate graph

            // #pragma omp parallel for
            for(int T = 0; T < round; T++){
                stringstream ss;
                string round_str;
                ss << T;
                ss >> round_str;
                ofstream ofs;
                ofs.open(file_path + "Round " + round_str + " log.txt");

                time_t now = time(0);
                char* dt = ctime(&now);
                cerr  << "時間 " << dt << endl << endl; 
                ofs  << "時間 " << dt << endl << endl; 

                string filename = file_path + "input" + round_str + ".txt";
                string command = "python3 main.py ";
                if(system((command + filename).c_str()) != 0){
                    cerr<<"error:\tsystem proccess python error"<<endl;
                    exit(1);
                }
                if(debug) filename = "debug_graph.txt";
                int num_of_node;
                ifstream graph_input;
                graph_input.open (filename);
                graph_input >> num_of_node;
                graph_input.close();
                // Graph graph("input.txt", num_of_node, min_channel, max_channel, min_memory_cnt, max_memory_cnt, node_time_limit, swap_prob, entangle_alpha, true);
                // Greedy greedy(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
                // QCAST qcast(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
                // REPS reps(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha);
                vector<AlgorithmBase*> algorithms;
                algorithms.emplace_back(new Greedy(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha));
                algorithms.emplace_back(new QCAST(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha));
                //algorithms.emplace_back(new REPS(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha));

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
                        // Request new_request = generate_new_request(0, 1, request_time_limit);
                        cerr<<q << ". source: " << new_request.get_source()<<", destination: "<<new_request.get_destination()<<endl;
                        for(auto &algo:algorithms){
                            algo->requests.emplace_back(new_request);
                        }
                    }
                    cerr<< "---------generating requests in main.cpp----------end" << endl;
                    
                    //#pragma omp parallel for
                    for(auto &algo:algorithms){
                        ofs<<"-----------run "<< algo->get_name() << " ---------"<<endl;
                        
                        algo->run();
                        algo->next_time_slot();
                        ofs<<"total_throughputs : "<<algo->total_throughput()<<endl;
                        ofs<<"-----------run "<<algo->get_name() << " ---------end"<<endl;
                    }
                    
                }
                ofs<<"---------------in round " <<T<<" -------------end" <<endl;
                ofs << endl;
                for(auto &algo:algorithms){
                    ofs<<"("<<algo->get_name()<<")total throughput = "<<algo->total_throughput()<<endl;
                }
                cout<<"---------------in round " <<T<<" -------------end" <<endl;
                cout << endl;
                for(auto &algo:algorithms){
                    cout<<"("<<algo->get_name()<<")total throughput = "<<algo->total_throughput()<<endl;
                }
                
                for(auto &algo:algorithms){
                    for(string Y_name : Y_names) {
                        result[algo->get_name()][Y_name] += algo->get_res(Y_name);
                    }
                }
                now = time(0);
                dt = ctime(&now);
                cerr  << "時間 " << dt << endl << endl; 
                ofs  << "時間 " << dt << endl << endl; 
                ofs.close();
            
                for(auto &algo:algorithms){
                    delete algo;
                }
                algorithms.clear();
            
            }



            for(string Y_name : Y_names) {
                string filename = X_name + "_" + Y_name + ".txt";
                ofstream ofs;
                ofs.open(file_path + filename, ios::app);
                ofs << change_value << ' ';
                ofs.close();
            }
        }
    }

    
    


    return 0;
}