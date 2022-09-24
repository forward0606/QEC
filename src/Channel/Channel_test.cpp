#include "Channel.h"
using namespace std;


int main(){
    cout<<"Hello c++!"<<endl;

    // Node(int memory_cnt, int time_limit, double pos_x, double pos_y, double swap_prob);
    Node node1(2, 7, 0, 0, 0.4); 
    Node node2(3, 7, 1, 1, 0.9);

    // Channel(Node* node1, Node* node2, double entangle_prob);
    vector<Channel> channels;
    for(int id = 0; id < 3; id++){ // 3 channel in same edge
        channels.emplace_back(&node1, &node2, 0.2);
    }
    // error: self cycle
    // Channel badchannel(&node1, &node1, 0.2); test ok!
    
    // Channel assignable check
    cout << "Channel assignable check" << endl;
    cout << "node1 remains " << node1.get_remain() << " qubit, node2 remains " << node2.get_remain() << " qubit." << endl;
    for(int id = 0; id < 3; id++){
        if(channels[id].is_assignable()){
            channels[id].assign();
            cout << "assign channel " << id << endl;
        }else{
            //channels[id].assign(); // error: assign but not assignable 
            cout << "CANNOT assign channel " << id << endl;
        } 
        cout << "node1 remains " << node1.get_remain() << " qubit, node2 remains " << node2.get_remain() << " qubit." << endl;
    }
    cout << endl;
    
    // channel entangle check
    cout << "channel entangle check" << endl;
    //channels[2].entangle();  // error: entangle when not used
    for(int t = 0; t < 10; t++){
        cout << "channel entangled "; 
        if(channels[1].entangle()) {
            cout << "success" << endl;
        }else{
            cout << "failed" << endl;
        }
        // error: entangle when channel failed or succ
        channels[1].refresh();
        node1.refresh();
        node2.refresh();
        channels[1].assign();
    }
    return 0;
}