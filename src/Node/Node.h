#include<iostream>
using namespace std;


class Node{
    double swap_prob = 1;
    int memory, time_limit;
    int pos_x, pos_y;   //唯一決定一個 node
public:
    Node(int memory_cnt, int time_limit, int pos_x, int pos_y);
    bool swap();
    bool operator==(const Node &right)const;
    bool operator!=(const Node &right)const;
    bool operator<(const Node &right)const;
    bool operator<=(const Node &right)const;
    bool operator>(const Node &right)const;
	bool operator>=(const Node &right)const;
};