#include"Node.h"





Node::Node(int id, int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob):
    id(id), swap_prob(swap_prob), memory_cnt(memory_cnt), time_limit(time_limit), remain(memory_cnt){
    pos = make_pair(pos_x, pos_y);
    cerr<<"new node"<<id <<endl;
}


Node::~Node(){

}
int Node::get_id(){
    return id;
}
double Node::get_swap_prob(){
    return swap_prob;
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

double Node::distance(const Node &right)const{
    double delta_x = pos.first - right.pos.first;
    double delta_y = pos.second - right.pos.second;
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}

void Node::release(){
    remain = memory_cnt;
}

bool Node::is_assignable()const{
    return remain > 0;
}

int Node::get_remain()const{
    return remain;
}


bool Node::operator==(const Node &right)const{
    return pos == right.pos;
}

bool Node::operator!=(const Node &right)const{
    return !(*this == right);
}

bool Node::operator<(const Node &right)const{
    return pos < right.pos;
}

bool Node::operator<=(const Node &right)const{
    return pos <= right.pos;
}

bool Node::operator>(const Node &right)const{
    return !(*this <= right);
}

bool Node::operator>=(const Node &right)const{
    return !(*this < right);
}


const Node Node::operator--(int){
    if(!is_assignable()){
        cerr<<"error:\tmemory is not sufficient!"<<endl;
        exit(1);
    }
    Node tmp = *this;
    remain--;
    return tmp;
}


const Node Node::operator++(int){
    if(remain >= memory_cnt){
        cerr<<"error:\tmemory is out of maximum value, maybe release twice occurs"<<endl;
        exit(1);
    }
    Node tmp = *this;
    remain++;
    return tmp;
}