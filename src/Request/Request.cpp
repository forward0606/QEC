#include"Request.h"

Request::Request(int source, int destination, const int& time_limit)
    :source(source), destination(destination), waiting_time(0), time_limit(time_limit), success_cnt(0){

}

Request::~Request(void){
    for(int i=0;i<(int)paths.size();i++){
        delete paths[i];
        paths[i] = nullptr;
    }
}

int Request::get_waiting_time(){
    return waiting_time;
}
int Request::get_source(){
    return source;
}
int Request::get_destination(){
    return destination;
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

void Request::operator+=(Path *path){
    paths.emplace_back(path);
}