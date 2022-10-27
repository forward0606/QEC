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
#include "Algorithm/MyAlgo/MyAlgo.h"
using namespace std;


const bool debug = false;


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
    default_setting["num_of_node"] = 100;
    default_setting["social_density"] = 0.5;
    default_setting["min_channel_cnt"] = 3;
    default_setting["max_channel_cnt"] = 7;
    default_setting["min_memory_cnt"] = 10;
    default_setting["max_memory_cnt"] = 14;
    default_setting["min_fidelity"] = 0.85;
    default_setting["max_fidelity"] = 0.99;

    default_setting["swap_prob"] = 1;
    default_setting["entangle_alpha"] = 0;
    default_setting["node_time_limit"] = 7;
    default_setting["new_request_min"] = 12;
    default_setting["new_request_max"] = 12;
    default_setting["request_time_limit"] = 7;
    default_setting["total_time_slot"] = 100;
    default_setting["service_time"] = 10;

    map<string, vector<double>> change_parameter;
    change_parameter["swap_prob"] = {0.1, 0.3, 0.5, 0.7, 0.9, 1};
    change_parameter["entangle_alpha"] = {0.02, 0.002, 0};
    change_parameter["min_fidelity"] = {0.5, 0.75, 0.85, 0.95, 0.99};
    change_parameter["service_time"] = {1, 5, 10};

    vector<string> X_names = {"swap_prob", "entangle_alpha", "min_fidelity", "service_time"};
    vector<string> Y_names = {"waiting_time", "throughputs", "finished_throughputs", "succ-finished_rate(0)", "succ-finished_rate(1)", "active_timeslot", "path_length"};
    vector<string> algo_names = {"Greedy", "QCAST", "REPS", "MyAlgo"};
    // init result
    for(string X_name : X_names) {
        for(string Y_name : Y_names){
            string filename = "ans/" + X_name + "_" + Y_name + ".ans";
            fstream file( file_path + filename, ios::out );
        }
    }
    

    int round = 10;
    for(string X_name : X_names) {
        map<string, double> input_parameter = default_setting;

        for(double change_value : change_parameter[X_name]) {
            vector<map<string, map<string, double>>> result(round);
            input_parameter[X_name] = change_value;
            
            int num_of_node = input_parameter["num_of_node"];
            double social_density = input_parameter["social_density"];
            int min_channel_cnt = input_parameter["min_channel_cnt"];
            int max_channel_cnt = input_parameter["max_channel_cnt"];
            int min_memory_cnt = input_parameter["min_memory_cnt"];
            int max_memory_cnt = input_parameter["max_memory_cnt"];
            double min_fidelity = input_parameter["min_fidelity"];
            double max_fidelity = input_parameter["max_fidelity"];

            double swap_prob = input_parameter["swap_prob"], entangle_alpha = input_parameter["entangle_alpha"];
            int node_time_limit = input_parameter["node_time_limit"];
            int new_request_min = input_parameter["new_request_min"], new_request_max = input_parameter["new_request_max"];
            int service_time = input_parameter["service_time"];
            int request_time_limit = input_parameter["request_time_limit"];
            int total_time_slot = input_parameter["total_time_slot"];

            // python generate graph

            #pragma omp parallel for
            for(int T = 0; T < round; T++){
                string round_str = to_string(T);
                ofstream ofs;
                ofs.open(file_path + "log/" + X_name + "_in_" + to_string(change_value) + "_Round_" + round_str + ".log");

                time_t now = time(0);
                char* dt = ctime(&now);
                cerr  << "時間 " << dt << endl << endl; 
                ofs  << "時間 " << dt << endl << endl; 

                string filename = file_path + "input/round_" + round_str + ".input";
                string command = "python3 main.py ";
                string parameter = to_string(num_of_node) + " " + to_string(min_channel_cnt) + " " + to_string(max_channel_cnt) + " " + to_string(min_memory_cnt) + " " + to_string(max_memory_cnt) + " " + to_string(min_fidelity) + " " + to_string(max_fidelity) + " " + to_string(social_density);
                if(system((command + filename + " " + parameter).c_str()) != 0){
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
                algorithms.emplace_back(new REPS(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha));
                algorithms.emplace_back(new MyAlgo(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha));

                ofs<<"---------------in round " <<T<<" -------------" <<endl;
                for(int t = 0; t < total_time_slot; t++){
                    ofs<<"---------------in timeslot " <<t<<" -------------" <<endl;
                    //亂數引擎, to decide how many requests received in this timeslot 
                    random_device rd;
                    default_random_engine generator = default_random_engine(rd());
                    uniform_int_distribution<int> unif(new_request_min, new_request_max);
                    int request_cnt = unif(generator);

                    cout<< "---------generating requests in main.cpp----------" << endl;
                    for(auto &algo:algorithms){
                        result[T][algo->get_name()]["total_request"] = 0;
                    }
                    for(int q = 0; q < request_cnt && t < service_time; q++){
                        Request new_request = generate_new_request(num_of_node, request_time_limit);
                        // Request new_request = generate_new_request(0, 1, request_time_limit);
                        cout<<q << ". source: " << new_request.get_source()<<", destination: "<<new_request.get_destination()<<endl;
                        for(auto &algo:algorithms){
                            result[T][algo->get_name()]["total_request"]++; 
                            algo->requests.emplace_back(new_request);
                        }
                    }
                    cout<< "---------generating requests in main.cpp----------end" << endl;
                    
                    //#pragma omp parallel for
                    for(auto &algo:algorithms){
                        ofs<<"-----------run "<< algo->get_name() << " ---------"<<endl;
                        
                        algo->run();
                        algo->next_time_slot();
                        ofs<<"total_throughputs : "<<algo->get_res("throughputs")<<endl;
                        ofs<<"-----------run "<<algo->get_name() << " ---------end"<<endl;
                    }
                    
                }
                ofs<<"---------------in round " <<T<<" -------------end" <<endl;
                ofs << endl;
                for(auto &algo:algorithms){
                    ofs<<"("<<algo->get_name()<<")total throughput = "<<algo->get_res("throughputs")<<endl;
                }
                cout<<"---------------in round " <<T<<" -------------end" <<endl;
                cout << endl;
                for(auto &algo:algorithms){
                    cout<<"("<<algo->get_name()<<")total throughput = "<<algo->get_res("throughputs")<<endl;
                }
                
                for(auto &algo:algorithms){
                    for(string Y_name : Y_names) {
                        result[T][algo->get_name()][Y_name] += algo->get_res(Y_name);
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
            
            map<string, map<string, double>> sum_res;
            for(string algo_name : algo_names){
                double finished_request_sum = 0, succ_request_sum = 0;
                double path_length_sum = 0;
                for(int T = 0; T < round; T++){
                    succ_request_sum += result[T][algo_name]["throughputs"];                    
                    finished_request_sum += result[T][algo_name]["finished_throughputs"];
                    path_length_sum += result[T][algo_name]["path_length"];
                    result[T][algo_name]["waiting_time"] /= result[T][algo_name]["total_request"];
                    result[T][algo_name]["succ-finished_rate(1)"] = result[T][algo_name]["throughputs"] / result[T][algo_name]["finished_throughputs"];
                }
                sum_res[algo_name]["succ-finished_rate(0)"] = succ_request_sum / finished_request_sum;
                sum_res[algo_name]["path_length"] = path_length_sum / finished_request_sum;

            }

            for(string Y_name : Y_names) {
                string filename = "ans/" + X_name + "_" + Y_name + ".ans";
                ofstream ofs;
                ofs.open(file_path + filename, ios::app);
                ofs << change_value << ' ';
                
                for(string algo_name : algo_names){
                    if(Y_name == "succ-finished_rate(0)" || Y_name == "path_length"){
                        ofs << sum_res[algo_name][Y_name] << ' ';
                    }else{
                        for(int T = 0; T < round; T++){
                            sum_res[algo_name][Y_name] += result[T][algo_name][Y_name];
                        }
                        ofs << sum_res[algo_name][Y_name] / round << ' ';
                    }
                }
                ofs << endl;
                ofs.close();
            }
        }
    }

    
    


    return 0;
}