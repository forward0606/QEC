#include<iostream>
#include<vector>
#include"../Node/Node.h"
#include"../Path/Path.h"
using namespace std;


class Request{
    int id;
    const int start_time;
    Node source, destination;
    Path paths[5];
public:
    Request(const int &id, const Node &source, const Node &destination, const int &start_time);
};