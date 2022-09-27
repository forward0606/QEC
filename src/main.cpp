#include <iostream>
#include <queue>
#include<algorithm>
#include "Network/Graph/Graph.h"
using namespace std;

double dis[26];
int parent[26];

int main(){
    cout<<"Hello Dijkstra!"<<endl;

    Graph graph("input.txt", 5, 0.9, 0.0064);
    priority_queue<pair<double, int>> pq;
    int source = 0, destination = 4;
    pq.emplace(1, source);
    dis[source] = 1;
    int node;
    double distance;
    vector<int> neightbors;
    cout<<"dijkstra begin!"<<endl;
    while(!pq.empty()){
        node = pq.top().second;
        distance = pq.top().first;
        pq.pop();
        cout<<node<<" "<<distance<<endl;
        if(node == destination){
            break;
        }
        if(dis[node] < distance){
            continue;
        }
        neightbors = graph.get_neighbors_id(node);
        cout<<"neightbor:\t";
        for(int ele:neightbors){
            cout<<ele<<" ";
            if(graph.remain_resource_cnt(node, ele) == 0){
                continue;
            }
            if(dis[node] * graph.get_channel_weight(node, ele) > dis[ele]){
                dis[ele] = dis[node] * graph.get_channel_weight(node, ele);
                parent[ele] = node;
                pq.emplace(dis[ele], ele);
            }
        }
        cout<<'\n';
    }
    if(dis[destination] == 0){
        cout<<"no path to the destination"<<endl;
        return 0;
    }

    vector<int> path;
    int tmp = destination;
    while(tmp != source){
        path.emplace_back(tmp);
        tmp = parent[tmp];
    }
    path.emplace_back(tmp);
    reverse(path.begin(), path.end());
    for(int i=0;i<(int)path.size();i++){
        cout<<path[i]<<" ";
    }
    cout<<endl;
    Path *path_ptr = graph.build_path(path);
    return 0;
}