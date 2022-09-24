#include "AlgorithmBase.h"

void AlgorithmBase::next_time_slot(){
    graph.refresh();
    for(auto request: requests){
        request->next_timeslot();
    }

}