#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include    "../../config.h"
#include    "gurobi_c++.h"
using namespace std;

class REPS:public AlgorithmBase{
    vector<Channel *> assign_path;
    void PFT(vector<double> &t_plum, vector<vector<vector<double>>> &f_plum);
    void EPS();
    void ELS();
public:
    REPS(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
    void entangle();
    void swap();
    void next_time_slot();
};