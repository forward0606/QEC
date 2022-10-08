#ifndef __REQUEST_H
#define __REQUEST_H

#include<iostream>
#include<vector>
#include"../Network/Node/Node.h"
#include"../Path/Path.h"
#include "../config.h"
using namespace std;


class Request{
    int source, destination;
    int waiting_time, time_limit;
    int throughput;
    vector<Path *> paths;                                       //休學
public:
    Request(int source, int destination, const int& time_limit);
    ~Request(void);
    int get_waiting_time();
    int get_source();
    int get_destination();
    int get_throughput();
    vector<Path *> get_paths();
    
    void add_one_throughput();
    void entangle();
    void swap();
    bool next_timeslot();
    void operator+=(Path *path);
    void print();
};

#endif