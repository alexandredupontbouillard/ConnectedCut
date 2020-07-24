
#include<fstream>
#include<sstream>
#include"ModelMinSeparator.h"
#include<cstdlib>
#include<cmath>
#include <string>
#include <iostream>
#include "Graph.h"
using namespace::std;



ModelMinSeparator::ModelMinSeparator(Graph & g):  constrPos(g._nbNodes), constrNeg(g._nbNodes){
	IloEnv env;
	IloModel model(env1);
	
	
	list<Edge*>::const_iterator it;

	///VARIABLES
	vector<IloNumVar>  vertice ; 
	vertice.resize(g._nbNodes);

	

	for(int i = 0 ; i < vertice.size() ; i ++){
		vertice[i] = IloNumVar(env1, - 1.0, 1.0, ILOFLOAT);
	}

	for (it=g._edges.begin();it!=g._edges.end();it++){
		edges.push_back(IloNumVar(env1, 0.0, 1.0, ILOFLOAT));
	}

	//CONSTRAINTS
	int size = 2;
	IloRangeArray CC(env1);


	list<IloNumVar>::const_iterator it2 =edges.begin();



	for (it=g._edges.begin();it!=g._edges.end();it++){
		IloExpr c1(env1);
		c1 += (*it2) + vertice[(*it)->_first] - vertice[(*it)->_last];
		
		CC.add(c1>= 0);
		
		IloExpr c2(env1);
		c2 += (*it2) - vertice[(*it)->_first] + vertice[(*it)->_last];
		CC.add(c2>= 0);
		size++;
		size++;
		

		it2++;
	}

	 model.add(CC);
	

	for(int i = 0 ; i < g._nbNodes; i ++){
		constrPos[i] = IloRange( IloExpr(vertice[i]) == 1);
		constrNeg[i] = IloRange( IloExpr(vertice[i]) == -1);
	}


	cplex = IloCplex(model); 

	

}

void ModelMinSeparator :: add(int  a, int  b){
	IloRangeArray CC(env1);

	CC.add(constrPos[a]);

	CC.add(constrNeg[b]);

	

	



}


void ModelMinSeparator :: deleta(int a, int b){

	//cplex.delete(constrPos[a]);
	//cplex.delete(constrNeg[b]);
	constrPos[a].removeFromAll();

	constrNeg[b].removeFromAll();

}

void modifyObj(){




}
