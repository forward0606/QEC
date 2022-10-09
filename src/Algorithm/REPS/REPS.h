#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include "../../config.h"
using namespace std;

class REPS:public AlgorithmBase{
    map<pair<Request*,Path*>,vector<Path*>> recovery_paths;
    double EXT(vector<double> path, int w);
    map<pair<int, int>, double> combination;
    double C(int n, int m);
    void p4();
public:
    REPS(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
    void find_recovery_path(int R);
    void entangle();
    void swap();
    void next_time_slot();
    // Path* BFS(int source, int destination); // no connect return NULL
    double demoEXT(vector<double> path, int w);
};