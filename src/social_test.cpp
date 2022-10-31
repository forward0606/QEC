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
    default_setting["area_alpha"] = 0.1;
    default_setting["memory_cnt_avg"] = 12;
    default_setting["channel_cnt_avg"] = 5;
    default_setting["resource_ratio"] = 1;
    default_setting["min_fidelity"] = 0.7;
    default_setting["max_fidelity"] = 0.95;

    default_setting["swap_prob"] = 1;
    default_setting["entangle_alpha"] = 0;
    default_setting["node_time_limit"] = 7;
    default_setting["new_request_cnt"] = 5;
    default_setting["request_time_limit"] = 7;
    default_setting["total_time_slot"] = 100;
    default_setting["service_time"] = 100;

    map<string, vector<double>> change_parameter;
    change_parameter["swap_prob"] = {0.3, 0.5, 0.7, 0.9, 1};
    change_parameter["entangle_alpha"] = {0.02, 0.002, 0.0002, 0};
    change_parameter["min_fidelity"] = {0.5, 0.7, 0.75, 0.85, 0.95};
    change_parameter["resource_ratio"] = {0.5, 1, 2, 10};
    change_parameter["area_alpha"] = {0.001, 0.01, 0.1}; 
    change_parameter["social_density"] = {0, 0.25, 0.5, 0.75, 1}; 
    change_parameter["new_request_cnt"] = {1, 2, 3, 4, 5};
    change_parameter["num_of_node"] = {100, 200, 300, 400, 500};

    vector<string> X_names = {"min_fidelity"};
    vector<string> Y_names = {"encode_cnt", "throughputs", "waiting_time", "unencode_cnt", "encode_ratio", "divide_cnt", "finished_throughputs", "encode_use_one_path_rate", "encode_num", "use_memory", "total_memory", "use_memory_ratio",\
                            "use_channel", "total_channel", "use_channel_ratio"};
    //vector<string> algo_names = {"social:0", "social:0.25", "social:0.50", "social:0.75", "social:1.00"};
    vector<string> algo_names = {"min_f0.5", "min_f0.7", "min_f0.75", "min_f0.85", "min_f0.95"};
    // // init result
    // for(string X_name : X_names) {
    //     for(string Y_name : Y_names){
    //         string filename = "ans/" + X_name + "_" + Y_name + ".ans";
    //         fstream file( file_path + filename, ios::out );
    //     }
    // }
    

    int round = 10;
    for(string X_name : X_names) {
        map<string, double> input_parameter = default_setting;

        for(double change_value : change_parameter[X_name]) {
            vector<map<string, map<string, double>>> result(round);
            vector<map<string, map<int, double>>> path_length_encode_avg(round);
            vector<map<string, map<int, double>>> encode_ratio(round);
            input_parameter[X_name] = change_value;
            
            int num_of_node = input_parameter["num_of_node"];
            //double social_density = input_parameter["social_density"];
            double area_alpha = input_parameter["area_alpha"];
            double resource_ratio = input_parameter["resource_ratio"];
            int min_memory_cnt = input_parameter["memory_cnt_avg"] * resource_ratio - 2;
            int max_memory_cnt = input_parameter["memory_cnt_avg"] * resource_ratio + 2;
            int min_channel_cnt = input_parameter["channel_cnt_avg"] * resource_ratio - 2;
            int max_channel_cnt = input_parameter["channel_cnt_avg"] * resource_ratio + 2;
            //double min_fidelity = input_parameter["min_fidelity"];
            double max_fidelity = input_parameter["max_fidelity"];
            double social_density = input_parameter["social_density"];
            double swap_prob = input_parameter["swap_prob"], entangle_alpha = input_parameter["entangle_alpha"];
            int node_time_limit = input_parameter["node_time_limit"];
            int new_request_cnt = input_parameter["new_request_cnt"];
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
                vector<MyAlgo*> algorithms;

                for(double min_fidelity:change_parameter["min_fidelity"]){
                    string filename = file_path + "input/round_" + round_str + ".input";
                    string command = "python3 main.py ";
                    string parameter = to_string(num_of_node) + " " + to_string(min_channel_cnt) + " " + to_string(max_channel_cnt) + " " + to_string(min_memory_cnt) + " " + to_string(max_memory_cnt) + " " + to_string(min_fidelity) + " " + to_string(max_fidelity) + " " + to_string(social_density) + " " + to_string(area_alpha);
                    if(system((command + filename + " " + parameter).c_str()) != 0){
                        cerr<<"error:\tsystem proccess python error"<<endl;
                        exit(1);
                    }
                    algorithms.emplace_back(new MyAlgo(filename, request_time_limit, node_time_limit, swap_prob, entangle_alpha));
                }
                

                ofs<<"---------------in round " <<T<<" -------------" <<endl;
                for(int t = 0; t < total_time_slot; t++){
                    ofs<<"---------------in timeslot " <<t<<" -------------" <<endl;

                    cout<< "---------generating requests in main.cpp----------" << endl;
                    for(int q = 0; q < new_request_cnt && t < service_time; q++){
                        Request new_request = generate_new_request(num_of_node, request_time_limit);
                        cout<<q << ". source: " << new_request.get_source()<<", destination: "<<new_request.get_destination()<<endl;
                        for(int algo_idx = 0;algo_idx < (int)algorithms.size();algo_idx++){
                            auto algo = algorithms[algo_idx];
                            result[T][algo_names[algo_idx]]["total_request"]++; 
                            algo->requests.emplace_back(new_request);
                        }
                    }
                    cout<< "---------generating requests in main.cpp----------end" << endl;
                    
                    #pragma omp parallel for
                    for(int i = 0; i < (int)algorithms.size(); i++){
                        auto &algo = algorithms[i];
                        ofs<<"-----------run "<< algo_names[i] << " ---------"<<endl;
                        
                        algo->run();
                        ofs<<"total_throughputs : "<<algo->get_res("throughputs")<<endl;
                        ofs<<"-----------run "<<algo_names[i] << " ---------end"<<endl;
                    }
                    
                }
                ofs<<"---------------in round " <<T<<" -------------end" <<endl;
                ofs << endl;
                for(int algo_idx = 0;algo_idx < (int)algorithms.size();algo_idx++){
                    auto algo = algorithms[algo_idx];
                    ofs<<"("<<algo_names[algo_idx]<<")total throughput = "<<algo->get_res("throughputs")<<endl;
                }
                cout<<"---------------in round " <<T<<" -------------end" <<endl;
                cout << endl;
                for(int algo_idx = 0;algo_idx < (int)algorithms.size();algo_idx++){
                    auto algo = algorithms[algo_idx];
                    ofs<<"("<<algo_names[algo_idx]<<")total throughput = "<<algo->get_res("throughputs")<<endl;
                }
                
                for(int algo_idx = 0;algo_idx < (int)algorithms.size();algo_idx++){
                    auto algo = algorithms[algo_idx];
                    for(string Y_name : Y_names) {
                        result[T][algo_names[algo_idx]][Y_name] = algo->get_res(Y_name);
                    }
                    auto path_length_encode = algo->path_length_encode;
                    auto path_length_cnt = algo->path_length_cnt;
                    for(auto e:path_length_encode){
                        path_length_encode_avg[T][algo_names[algo_idx]][e.first] = e.second*1.0 / path_length_cnt[e.first];
                    }
                    auto encode_cnt = algo->num_of_path_count;
                    double number_of_count = 0;
                    for(auto e:encode_cnt){
                        number_of_count += e.second;
                    }
                    for(auto e:encode_cnt){
                        encode_ratio[T][algo_names[algo_idx]][e.first] = e.second * 1.0 / number_of_count;
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
            
            // map<string, map<string, double>> sum_res;
            // for(string algo_name : algo_names){
            //     for(int T = 0; T < round; T++){
            //         result[T][algo_name]["encode_ratio"] = result[T][algo_name]["encode_cnt"] / (result[T][algo_name]["encode_cnt"] + result[T][algo_name]["unencode_cnt"]);
            //         result[T][algo_name]["divide_cnt"] = result[T][algo_name]["divide_cnt"] / result[T][algo_name]["total_request"];
            //         result[T][algo_name]["use_memory_ratio"] = result[T][algo_name]["use_memory"] / result[T][algo_name]["total_memory"];
            //         result[T][algo_name]["use_channel_ratio"] = result[T][algo_name]["use_channel"] / result[T][algo_name]["total_channel"];
            //         result[T][algo_name]["encode_use_one_path_rate"] /= result[T][algo_name]["encode_num"];
            //     }
            // }

            // for(string Y_name : Y_names) {
            //     string filename = "ans/" + X_name + "_" + Y_name + ".ans";
            //     ofstream ofs;
            //     ofs.open(file_path + filename, ios::app);
            //     ofs << change_value << ' ';
                
            //     for(string algo_name : algo_names){
            //         for(int T = 0; T < round; T++){
            //             sum_res[algo_name][Y_name] += result[T][algo_name][Y_name];
            //         }
            //         ofs << sum_res[algo_name][Y_name] / round << ' ';
            //     }
            //     ofs << endl;
            //     ofs.close();
            // }

            
            // string filename = "ans/GG.ans";
            // ofstream ofs;
            // ofs.open(file_path + filename);

            // for(int T = 1; T < round; T++){
            //     for(int algi=1;algi<5;algi++){
            //         for(auto e:path_length_encode_avg[T][algo_names[algi]]){
            //             path_length_encode_avg[0][algo_names[algi]][e.first] += e.second;
            //         }
            //     }
            // }
            
            // for(int path_length=3;path_length<9;path_length++){
            //     ofs<<path_length<<" ";
            //     for(int algi=1;algi<5;algi++){
            //         ofs<<path_length_encode_avg[0][algo_names[algi]][path_length] / round<<" ";
            //     }
            //     ofs<<endl;
            // }
            // ofs << endl;
            // ofs.close();

            filename = "ans/GG2.ans";
            ofs.open(file_path + filename);

            for(int T = 1; T < round; T++){
                for(int algi=0;algi<5;algi++){
                    for(auto e:encode_ratio[T][algo_names[algi]]){
                        encode_ratio[0][algo_names[algi]][e.first] += e.second;
                    }
                }
            }
            
            for(int i=1;i<=5;i++){
                ofs<<path_length<<" ";
                for(int algi=0;algi<5;algi++){
                    ofs<<encode_ratio[0][algo_names[algi]][i] / round<<" ";
                }
                ofs<<endl;
            }
            ofs << endl;
            ofs.close();
        }
    }
    return 0;
}

// 5.95375 5.99068 13.6879 20.6011
// 7.33758 8.7139 20.234 29.5734 
// 10.3517 12.6671 27.2966 38.8212 
// 12.859 14.7601 36.7725 48.1671 
// 10.9736 19.7771 46.1463 58.287 
// 3.75 11.7778 52.1444 66.6111 