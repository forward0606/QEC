#include<iostream>
#include <random>
#include <ctime>
using namespace std;


class Node{
    const double swap_prob;
    int memory_cnt, time_limit;
    int remain;                 //remain qbit
    pair<double, double> pos;   //唯一決定一個 node
public:
    Node(int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob);
    bool swap();
    bool operator==(const Node &right)const;
    bool operator!=(const Node &right)const;
    bool operator<(const Node &right)const;
    bool operator<=(const Node &right)const;
    bool operator>(const Node &right)const;
	bool operator>=(const Node &right)const;
    const Node operator--(int);                  //delete one remain memory
    double distance(const Node &right)const;     //return the distance of two node
    void refresh();
    bool is_assignable()const;
};