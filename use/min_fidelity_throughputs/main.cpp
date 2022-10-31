#include<iostream>
#include<vector>
using namespace std;

int main(){
	freopen("min_fidelity_throughputs.ans", "r", stdin);
	vector<double> x;
	vector<vector<double>> y(5);
	int t = 0;
	double xx, yy;
	while(cin>>xx){
		x.emplace_back(xx);
		for(int i=0;i<4;i++){
			cin>>yy;
			y[t].emplace_back(yy);
		}
		t++;
	}
	for(int i=0;i<3;i++){
		cout<<((y[2][3] - y[2][i]) * 1.0 / y[2][3]) * 100<<"\t";
	}
	cout<<endl;
	return 0;
}
