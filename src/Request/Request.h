#include<iostream>
#include<vector>
#include"../Node/Node.h"
#include"../Path/Path.h"
using namespace std;


class Request{
    int waiting_time, time_limit, success_cnt;
    Node *source, *destination;
    vector<Path *> paths;
public:
    Request(Node *source, Node *destination, const int& time_limit);
    ~Request(void);
    int get_waiting_time();
    void entangle();
    void swap();
    bool next_timeslot();
    bool operator+=(Path *path);
};
