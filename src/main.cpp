#include <iostream>
#include "Edge/Edge.h"
using namespace std;


int main(){
    cout<<"Hello c++!"<<endl;

    // Node(int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob);
    Node node1(2, 7, 0, 0, 0.4); 
    Node node2(3, 7, 1, 1, 0.9);

    // Edge(Node* node1_ptr, Node* node2_ptr, int channel_cnt, double entangle_alpha);
    Edge edge(&node1, &node2, 3, 3);


    return 0;
}