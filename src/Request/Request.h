#include<iostream>
#include<vector>
#include"../Network/Node/Node.h"
#include"../Path/Path.h"
using namespace std;


class Request{
    int source, destination;
    int waiting_time, time_limit, success_cnt;
    vector<Path *> paths;
public:
    Request(int source, int destination, const int& time_limit);
    ~Request(void);
    int get_waiting_time();
    int get_source();
    int get_destination();
    vector<Path *> get_paths();
    void entangle();
    void swap();
    bool next_timeslot();
    void operator+=(Path *path);
};
