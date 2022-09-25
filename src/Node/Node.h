#ifndef __NODE_H
#define __NODE_H

#include <iostream>
#include <random>
#include <ctime>
#include <cmath>
using namespace std;


class Node{
    const double swap_prob;
    int memory_cnt, time_limit;
    int remain;                 //remain qubit
    pair<double, double> pos;   //唯一決定一個 node
public:
    Node(int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob);
    ~Node(void){};
    bool swap();
    bool operator==(const Node &right)const;
    bool operator!=(const Node &right)const;
    bool operator<(const Node &right)const;
    bool operator<=(const Node &right)const;
    bool operator>(const Node &right)const;
	bool operator>=(const Node &right)const;
    const Node operator--(int);                  //delete one remain memory
    const Node operator++(int);                  //release one remain memory
    double distance(const Node &right)const;     //return the distance of two node
    void release();                              //clean assign status
    bool is_assignable()const;                   //return if we can use this node to build an entangle or not
    int get_remain()const;                       //return the number of qubit is not used 
};

#endif