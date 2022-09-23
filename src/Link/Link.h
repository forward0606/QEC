#include<iostream>
#include"../Node/Node.h"
using namespace std;

class Link{
    int id;
	Node node1, node2;
public:
    Link(int id, Node node1, Node node2);
    bool swap();

};