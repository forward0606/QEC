#include"Request.h"

Request::Request(int source, int destination, const int& time_limit)
    :source(source), destination(destination), waiting_time(0), time_limit(time_limit), throughput(0){
    if(DEBUG)cerr<<"new Request"<<endl;
}

Request::~Request(void){
    if(DEBUG)cerr<<"delete Request"<<endl;
    for(int i=0;i<(int)paths.size();i++){
        paths[i]->release();
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

int Request::get_throughput(){
    return throughput;
}

vector<Path *> Request::get_paths(){
    return paths;
}

void Request::add_one_throughput(){
    throughput++;
}


void Request::entangle(){
    for(auto &path:paths){
        path->entangle();
    }
}

void Request::swap(){
    for(auto &path:paths){
        if(path->get_entangle_succ()) path->swap();
        if(path->get_swap_succ()) throughput++;
    }
}

bool Request::next_timeslot(){
    for(auto path_ptr:paths){
        path_ptr->release();
        delete path_ptr;
    }
    paths.clear();
    waiting_time++;
    if(throughput > 0){
        time_limit--;
    }
    return (time_limit == 0) && (throughput > 0);
}

void Request::operator+=(Path *path){
    paths.emplace_back(path);
}