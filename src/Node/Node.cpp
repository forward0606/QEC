#include"Node.h"


Node::Node(int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob):
    swap_prob(swap_prob), memory_cnt(memory_cnt), time_limit(time_limit), remain(memory_cnt){
    pos = make_pair(pos_x, pos_y);
}

bool Node::swap(){
    //亂數引擎 
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<double> dis(0.0, 1.0);
    if(dis(gen) <= swap_prob){
        return true;
    }
    return false;
}

bool Node::operator==(const Node &right)const{
    return pos == right.pos;
}

bool operator!=(const Node &right)const{
    return !(this == right);
}

bool operator<(const Node &right)const{
    return pos < right;
}

bool operator<=(const Node &right)const{
    return pos <= right;
}

bool operator>(const Node &right)const{
    return !(this <= right);
}

bool operator>=(const Node &right)const{
    return !(this < right);
}

bool is_assignable()const{
    return remain > 0;
}

const Node operator--(int){
    if(!is_assignable()){
        exit(1);
    }
    remain--;
}

void refresh(){
    remain = memory_cnt;
}

double distance(const Node &right)const{
    double delta_x = pos.first - right.pos.first;
    double delta_y = pos.second - right.pos.second;
    return delta_x * delta_x + delta_y * delta_y;
}