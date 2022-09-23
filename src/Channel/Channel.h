#include<iostream>
using namespace std;

struct Channel{
    bool used, entangled;
    int id;
    Node node1, node2;
public:
    Channel(int id, Node node1, Node node2);
};