#include    "../AlgorithmBase/AlgorithmBase.h"
#include    "../../Network/Graph/Graph.h"
#include    "../../config.h"
#include    "gurobi_c++.h"
using namespace std;

class REPS:public AlgorithmBase{
    vector<Channel *> assign_path;
    void PFT_LP(vector<double> &t_plum, vector<map<pair<int, int>, double>> &f_plum);
    void EPS_LP(vector<double> &t_bar, vector<map<pair<int, int>, double>> &f_bar);
    void EPS();
    void ELS();
public:
    REPS(string filename, int request_time_limit, int node_time_limit, double swap_prob, double entangle_alpha);
    void path_assignment();
    void entangle();
    void swap();
    void next_time_slot();
};