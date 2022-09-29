#include <fstream>
#include <random>
#include <ctime>
#include "Graph.h"

Graph::Graph(string filename, int num_of_node, double swap_prob, double entangle_alpha)
	:min_channel(3), max_channel(7),
     min_memory_cnt(10), max_memory_cnt(14), time_limit(7), swap_prob(swap_prob), entangle_alpha(entangle_alpha){
    neighbor.resize(num_of_node);
    generate(filename, num_of_node);
}

Graph::~Graph(void){

}

Channel* Graph::assign_channel(Node node1, Node node2){
    
    //I'm sorry for this code. = =|||
    if(node1 > node2){
        for(auto& channel: channels[make_pair(node2, node1)]){
            if(channel.is_assignable()){
                channel.assign();
                return &channel;
            }
        }
    }else{
        for(auto& channel: channels[make_pair(node1, node2)]){
            if(channel.is_assignable()){
                channel.assign();
                return &channel;
            }
        }
    }
    
    cerr << "err:\tassign channel but no channel is assignable." << endl;
    exit(1);
    return nullptr;
}

vector<int> Graph::get_neighbors_id(int node1_id){
    return neighbor[node1_id];
}

Node* Graph::Node_id2ptr(int id){
    if(id >= nodes.size() || id < 0){
        cerr<<"err:\t in Graph::Node_id2ptr() id is out of range"<<endl;
        exit(1);
    }
    return &nodes[id];
}

/*
c++ call system to run python (waxman)
python generate graph, and then write to file
c++ read file to get the nodes and edges
*/
void Graph::generate(string filename, int num_of_node){
	string command = "python3 main.py ";
    if(system((command + to_string(num_of_node) + " " + filename).c_str()) != 0){
        cerr<<"error:\tsystem proccess python error"<<endl;
        exit(1);
    }
	
    //亂數引擎 
    random_device rd;
    default_random_engine generator = default_random_engine(rd());
    uniform_int_distribution<int> memgen(min_memory_cnt,max_memory_cnt);
    uniform_int_distribution<int> changen(min_channel,max_channel);
    
    ifstream graph_input;
    graph_input.open (filename);

    // input of nodes
    double pos_x, pos_y;
    for(int i = 0; i < num_of_node; i++){
		graph_input >> pos_x >> pos_y;
		int memory_cnt = memgen(generator) ;
        nodes.emplace_back(memory_cnt, time_limit, pos_x, pos_y, swap_prob);
	}
    
    // input of edges
    //Node node1, node2;
    int node_id1, node_id2;
    while(graph_input >> node_id1 >> node_id2){
        neighbor[node_id1].emplace_back(node_id2);
        neighbor[node_id2].emplace_back(node_id1);
        if(nodes[node_id1] > nodes[node_id2]){
            swap(node_id1, node_id2);
        }
        Node &node1 = nodes[node_id1];
        Node &node2 = nodes[node_id2];
        
        int channel_cnt = changen(generator);
        if(node1 == node2){
            cerr<<"error:\texist an edge with same node!"<<endl;
            exit(1);
	    }
        double entangle_prob = exp(-entangle_alpha * (node1.distance(node2))); // e^( -alpha * dis(node1, node2) )
        for(int i = 0; i < channel_cnt; i++){
            channels[make_pair(node1, node2)].emplace_back(&node1, &node2, entangle_prob);
        }
        //edges[make_pair(node1, node2)] = &(Edge(&node1, &node2, channel_cnt, entangle_alpha));
    }
    cerr<<"new graph!"<<endl;
}

double Graph::get_channel_weight(int node1_id, int node2_id){
    if(nodes[node1_id] > nodes[node2_id]){
        swap(node1_id, node2_id);
    }
    Node node1 = nodes[node1_id], node2 = nodes[node2_id];
    double sum = 0;
    for(Channel channel : channels[make_pair(node1, node2)]){
        sum += channel.get_weight();
    }
    return sum / channels[make_pair(node1, node2)].size();
}


void Graph::refresh(){ // refresh all channel entangle status
    for(auto &chans:channels){
        for(auto &e:chans.second){
            e.refresh();
        }
    }
}

void Graph::release(){ //clean all assigned resource(node and channel)
    for(int i = 0; i < (int)nodes.size(); i++){
        nodes[i].release();
    }
    for(auto &chans:channels){
        for(auto &e:chans.second){
            e.release();
        }
    }
}

void Graph::set_weight(int node1_id, int node2_id, double value){
    if(nodes[node1_id] > nodes[node2_id]){
        swap(node1_id, node2_id);
    }
    const Node &node1 = nodes[node1_id];
    const Node &node2 = nodes[node2_id];
    for(Channel channel: channels[make_pair(node1, node2)]){
        channel.set_weight(value);
    }
}


int Graph::remain_resource_cnt(int node1_id, int node2_id){
    if(nodes[node1_id] > nodes[node2_id]){
        swap(node1_id, node2_id);
    }
    const Node &node1 = nodes[node1_id];
    const Node &node2 = nodes[node2_id];
    int cnt = 0;
    for(Channel channel: channels[make_pair(node1, node2)]){
        if(channel.is_assignable()){
            cnt++;
        }
    }
    return min(cnt, min(node1.get_remain(), node2.get_remain()));
}


Path* Graph::build_path(vector<int> nodes_id){
    vector<Node*> path_nodes;
    vector<Channel*> path_channels;
    for(auto node_id: nodes_id){
        path_nodes.push_back(&(nodes[node_id]));
    }
    for(int i = 0; i < (int)nodes_id.size()-1; i++){
        Node &node1 = nodes[nodes_id[i]];
        Node &node2 = nodes[nodes_id[i+1]];
        path_channels.push_back(assign_channel(node1, node2));
    }
    return new Path(path_nodes, path_channels);
}