#include    <iostream>
#include    <cmath>
#include    "../AlgorithmBase/AlgorithmBase.h"
using namespace std;

class QCAST:public AlgorithmBase{
    double EXT(vector<int> path, int w);
    map<pair<int, int>, double> combination;
    double C(n, m);
public:
    QCAST();
    void path_assignment();
};