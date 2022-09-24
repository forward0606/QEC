#include<iostream>
using namespace std;

class Channel{
    Node *node1_ptr, *node2_ptr;
    double entangle_prob;
    bool used, entangled;
public:
    Channel(Node* node1, Node* node2, double entangle_prob);
    bool entangle();
    bool is_assignable();
    void assign();
};
