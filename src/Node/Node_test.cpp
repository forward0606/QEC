#include<iostream>
#include "Node.h"
using namespace std;


int main(){
    cout<<"Hello c++!"<<endl;

    // Node(int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob);
    Node node1(5, 7, 0, 0, 0.4); 
    Node node2(10, 7, 1, 1, 0.9); 
    // remain qubit check
    cout << "remain qubit check" << endl;
    while(node1.is_assignable()){
        cout << "node remain " << node1.get_remain() << " qubit." << endl;
        node1--;
    }
    cout << "node remain " << node1.get_remain() << " qubit." << endl;
    node1.refresh();
    cout << "node remain " << node1.get_remain() << " qubit." << endl;
    // node swap check
    cout << "node swap check, p = 0.4" << endl;
    for(int t = 0; t < 10; t++){
        cout << "swap " << t;
        if(node1.swap()) cout << " :success" << endl;
        else cout << " :fail" << endl;
    }
    // node distance check
    cout << "node distance check (1.414)" << endl;
    cout << node1.distance(node2) << endl;
    // node operator check
    cout << "node operator check" << endl;
    cout <<"node1 == node2 is "<<(node1 == node2?"True":"False")<<"!"<<endl;
    cout <<"node1 != node2 is "<<(node1 != node2?"True":"False")<<"!"<<endl;
    cout <<"node1 < node2 is " <<(node1 < node2?"True":"False" )<<"!"<<endl;
    cout <<"node1 <= node2 is "<<(node1 <= node2?"True":"False")<<"!"<<endl;
    cout <<"node1 > node2 is "<<(node1 > node2?"True":"False")<<"!"<<endl;
    cout <<"node1 >= node2 is "<<(node1 >= node2?"True":"False")<<"!"<<endl;
    
    cout<<"node OK!"<<endl;

    return 0;
}