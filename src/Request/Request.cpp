#include"Request.h"

Request::Request(int source, int destination, const int& time_limit)
    :source(source), destination(destination), waiting_time(0), time_limit(time_limit), throughput(0), status(REQUEST_UNFINISHED){
    if(DEBUG)cerr<<"new Request"<<endl;
}

Request::~Request(void){
    if(DEBUG)cerr<<"delete Request"<<endl;
    for(int i=0;i<(int)paths.size();i++){
        paths[i]->release();
        delete paths[i];
        paths[i] = nullptr;
    }
    paths.clear();
}

void Request::set_path(int path_id, Path *p){
    if(path_id >= (int)paths.size()){
        cerr<<"can't set this path!"<<endl;
        exit(1);
    }
    paths[path_id] = p;
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

void Request::clear_paths(){
    for(int i=0;i < (int)paths.size();i++){
        if(paths[i] != nullptr){
            delete paths[i];
            paths[i] = nullptr;
        }
    }
    paths.clear();
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
        if(path == nullptr)continue;
        if(path->get_entangle_succ()) path->swap();
        // if(path->get_swap_succ()) throughput++;
    }
}

void Request::send(){
    int pid = -1, mx = 0;
    for(int i=0;i<(int)paths.size();i++){
        if(paths[i] == nullptr)continue;
        if(!paths[i]->get_swap_succ())continue;
        if(paths[i]->fidelity() > mx){
            mx = paths[i]->fidelity();
            pid = i;
        }
    }
    if(pid == -1){
        return;
    }
    if(paths[pid]->send_data()){
        status = REQUEST_SUCC;
    }else{
        status = REQUEST_FAIL;
    }
}
bool Request::is_finished(){
    return status != REQUEST_UNFINISHED;
}
bool Request::is_success(){
    if(status == REQUEST_UNFINISHED){
        cerr<<"the request is unfinshed!"<<endl;
        exit(1);
    }
    return status == REQUEST_SUCC;
}
bool Request::next_timeslot(){
    for(auto path_ptr:paths){
        if(path_ptr != nullptr){
            path_ptr->release();
            delete path_ptr;
        }
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