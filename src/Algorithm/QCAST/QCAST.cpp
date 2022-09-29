#include "QCAST.h"

QCAST::QCAST()
    :AlgorithmBase(){

}

void QCAST::path_assignment(){
    
}
double QCAST::C(n, m){
    if(n < m){
        cerr<<"err:\tint QCAST combination m > n"<<endl;
        exit(1);
    }
    if(n == m || m == 0){return 1;}
    if(combination.find(get_pair(n, m)) != combination.end()){
        return combination[get_pair(n, m)];
    }
    combination[get_pair(n, m)] = C(n-1, m) + C(n-1, m-1);
    return combination[get_pair(n, m)];
}
double QCAST::EXT(vector<int> path, int w){
    vector<vector<double>> q(path.size()); //q[i][j] the prob of j links success ith edge on path 
    for(int i=0;i<q.size();i++){
        q[i].resize(w)+1;
    }
    for(int k=0;k<q.size();k++){
        for(int i=0;i<=w;i++){
            q[k][i] = C(w, i) * pow(path[k].prob, i) * pow(1-path[k].prob, w-i);
        }
    }
}