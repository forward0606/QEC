#include<iostream>
using namespace std;

class Channel{
    bool used, entangled;
    Node node1, node2;
public:
    Channel(Node node1, Node node2);
};