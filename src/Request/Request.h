#include<iostream>
#include<vector>
#include"../Node/Node.h"
#include"../Path/Path.h"
using namespace std;


class Request{
    int waiting_time, time_limit, success_cnt;
    Node* source, destination;
    vector<Path> paths;
public:
    Request(const Node& source, const Node& destination, const int& time_limit);
};
