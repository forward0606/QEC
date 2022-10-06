#include "AlgorithmBase.h"
#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
#include <random>
#include <ctime>


AlgorithmBase::AlgorithmBase(Graph graph, int time_limit, double swap_prob)
    :timeslot(0), waiting_time(0), throughputs(0), time_limit(time_limit), swap_prob(swap_prob), graph(graph){
    
}

// Request* AlgorithmBase::generate_new_request(){
//     //亂數引擎 
//     random_device rd;
//     default_random_engine generator = default_random_engine(rd());
//     uniform_int_distribution<int> unif(0, graph.get_size()-1);
    
//     return new Request(graph.Node_id2ptr(unif(generator)), graph.Node_id2ptr(unif(generator)), time_limit);
// }

double AlgorithmBase::get_swap_prob(){
    return swap_prob;
}

void AlgorithmBase::next_time_slot(){
    graph.refresh();
    graph.release();
    for(auto &request: requests){
        request.next_timeslot();
    }

    // // generate new requests for next time slot
    // //亂數引擎 
    // random_device rd;
    // default_random_engine generator = default_random_engine(rd());
    // uniform_int_distribution<int> unif(new_request_min,new_request_max);
    // int request_cnt = unif(generator);
    // for(int i = 0; i < request_cnt; i++){
    //     requests.push_back(generate_new_request());
    // }
}

void AlgorithmBase::entangle(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].entangle();
    }
}

void AlgorithmBase::swap(){
    for(int i=0;i<(int)requests.size();i++){
        requests[i].swap();
    }
}


void AlgorithmBase::run(){
    path_assignment();
    entangle();
    swap();
}