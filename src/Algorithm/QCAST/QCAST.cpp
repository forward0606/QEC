#include "QCAST.h"
#include <queue>


QCAST::QCAST(AlgorithmBase base)
    :AlgorithmBase(base){

}


double QCAST::EXT(vector<double> path, int w){
    vector<vector<double>> Q(path.size()); //Q[i][j] the prob of exactly j links success in ith edge on path 
    vector<vector<double>> P(path.size());
    for(int i=0;i<(int)path.size();i++){
        Q[i].resize(w+1);
        P[i].resize(w+1);
    }
    for(int k=0;k<(int)path.size();k++){
        for(int i=0;i<=w;i++){
            Q[k][i] = C(w, i) * pow(path[k], i) * pow(1-path[k], w-i);
        }
    }
    for(int i=0;i<=w;i++){      //P0i = q0i
        P[i][0] = Q[i][0];
    }
    for(int k=1;k<(int)path.size();k++){
        for(int i=0;i<=w;i++){
            P[k][i] = 0;
            for(int l = i;l<=w;l++){
                P[k][i] += P[k-1][i] * Q[k][l];
            }
            for(int l = i+1;l<=w;l++){
                P[k][i] += Q[k][i] * P[k-1][l];
            }
        }
    }

    //Et = q^h * sum(i * P[h][i])
    double ext = 0;
    for(int i=1;i<=w;i++){
        ext += i * P.back()[i];
    }
    return ext * pow(get_swap_prob(), path.size());
}

double QCAST::C(int n, int m){
    if(n < m){
        cerr<<"err:\tint QCAST combination m > n"<<endl;
        exit(1);
    }
    if(n == m || m == 0){return 1;}
    if(combination.find(make_pair(n, m)) != combination.end()){
        return combination[make_pair(n, m)];
    }
    combination[make_pair(n, m)] = C(n-1, m) + C(n-1, m-1);
    return combination[make_pair(n, m)];
}

int QCAST::find_width(vector<int> path){
    if(path.size() < 2){
        return 0x3f3f3f3f;
    }
    int width = graph.remain_resource_cnt(path[0], path[1]);
    for(int i=2;i<(int)path.size();i++){
        width = min(width, graph.remain_resource_cnt(path[i-1], path[i]));
    }
    return width;
}

void QCAST::assign_resource(vector<int> path, int reqno){}



struct CandPath{
    int id, source, destination;
    vector<double> p;
    vector<int> path;
    bool useable;
    CandPath(int id, int s, int d, vector<double> p, vector<int> path):
        id(id), source(s), destination(d), p(p), path(path), useable(true){}
    CandPath(int id, int s, int d):
        id(id), source(s), destination(d), useable(true){
            path.emplace_back(s);
    }
    CandPath(){};
    CandPath(const CandPath& copy){
        *this = copy;
    }
    CandPath& operator=(const CandPath& copy){
        id = copy.id;
        source = copy.source;
        destination = copy.destination;
        p = copy.p;
        path = copy.path;
        useable = copy.useable;
        return *this;
    }
};

void QCAST::path_assignment(){
    vector<CandPath> candiate;
    for(int i=0;i<(int)requests.size();i++){
        candiate.emplace_back(i, requests[i].get_source(), requests[i].get_destination());
    }
    int find_path_cnt = 0;
    vector<int> neightbors;
    while(find_path_cnt < 100){
        //find the path with biggest EXT
        int mxi = -1, mxv = 0, width;
        for(int i = 0;i<(int)candiate.size();i++){
            if(!candiate[i].useable)    continue;
            width = find_width(candiate[i].path);
            if(width == 0 || candiate[i].path.size() >= 200){
                candiate[i].useable = false;
                continue;
            }
            double ext = EXT(candiate[i].p, width);
            if(ext > mxv){
                mxv = ext;
                mxi = i;
            }
        }
        if(mxi == -1){
            //all path is used
            break;
        }
        if(candiate[mxi].destination == candiate[mxi].path.back()){
            //find new paths
            find_path_cnt += find_width(candiate[mxi].path);
            assign_resource(candiate[mxi].path, candiate[mxi].id);
        }else{
            //go one step next
            int node = candiate[mxi].path.back();
            neightbors = graph.get_neighbors_id(node);
            for(int ele:neightbors){
                //cout<<ele<<" ";
                if(graph.remain_resource_cnt(node, ele) == 0){
                    continue;
                }
                candiate[mxi].path.emplace_back(ele);
                candiate[mxi].p.emplace_back(graph.get_channel_weight(node, ele));
                candiate.emplace_back(candiate[mxi]);
                candiate[mxi].path.pop_back();
                candiate[mxi].p.pop_back();
            }
        }
        candiate[mxi].useable = false;
        //remove used path
        int l = 0;
        for(int i = 0;i<(int)candiate.size();i++){
            if(candiate[i].useable){
                candiate[l] = candiate[i];
                l++;
            }
        }
        candiate.resize(l);
    }
}