#include"Request.h"

Request::Request(Node* source, Node* destination, const int& time_limit)
    :source(source), destination(destination), time_limit(time_limit){

}

void Request::entangle(){
    for(auto &path:paths){
        path->entangle();
    }
}

void Request::swap(){
    for(auto &path:paths){
        if(path->get_entangle_succ()) path->swap();
        if(path->get_swap_succ()) success_cnt++;
    }
}

bool Request::next_timeslot(){
    waiting_time++;
    if(success_cnt > 0){
        time_limit--;
    }
    return (time_limit == 0) && (success_cnt > 0);
}

bool Request::operator+=(Path *path){
    paths.emplace_back(path);
}