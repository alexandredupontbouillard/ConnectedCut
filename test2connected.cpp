#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "Graph.h"



int main(int argc, char** argv){

	Graph G(argv[1]);

	vector<int> l = G.getBridges();


	cout<<l.size()<<endl;

	if( l.size() ==0){
		cout<<"le graphe est 2 connexe"<<endl;

	}
	else{
	cout<<"graphe non 2 connexe"<<endl;

	}


	return 0;

}
