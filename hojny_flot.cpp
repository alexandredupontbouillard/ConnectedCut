#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "Graph.h"
#include "statStruct.h"
#include <ctime>
#include <chrono>
#define fResultName "result.csv"
#define epsilon 0.00001
int main(int argc, char** argv){
	IloEnv env;

	try{
		IloModel model(env);

		if (argc < 2) {
	    	cout << "Error : the command should be followed by a filename" << endl;
	    	exit(1);
		}
		char * bl = argv[1];
		Graph G(bl);
		

	int nbCst= 0;
	statStruct stat;
    	stat.init();
	
	stat.formulation = "hojnyFlot";
	IloRangeArray CC(env);
	IloExpr c1(env);
	
	vector<pair<IloNumVar,IloNumVar>> sommets(G._nbNodes);
	vector<IloNumVar> edge(G._edges.size()) ;

	vector<pair<IloNumVar,IloNumVar>> puits(G._nbNodes);  // first = le noeud i appartient au 1er élément de la partition, second = 2ème

	vector<pair<IloNumVar,IloNumVar>> flot(G._edges.size());  // arête uv  -->  pair<uv,vu>
	//initialisation des variables
	
	for(int i = 0 ; i < G._nbNodes ; i ++){
		sommets[i] = make_pair(IloNumVar(env, 0.0, 1.0, IloNumVar::Int,""),IloNumVar(env, 0.0, 1.0, IloNumVar::Int,""))  ;

	}

	for(int i = 0 ;i < edge.size(); i++){
	
		edge[i] = IloNumVar(env, 0.0, 1.0, IloNumVar::Int,"");

	}

	for(int i = 0 ; i < puits.size(); i ++ ){

		puits[i] = make_pair (IloNumVar(env, 0.0, 1.0, IloNumVar::Int,""),IloNumVar(env, 0.0, 1.0, IloNumVar::Int,""));

	}
	for(int i = 0 ; i < flot.size(); i ++){

		flot[i] = make_pair(IloNumVar(env, 0.0, IloInfinity, IloNumVar::Float,""), IloNumVar(env, 0.0,IloInfinity , IloNumVar::Float,"")); 
		

	}


	// contraintes de sommets

	for(int i = 0; i < sommets.size(); i ++){
		IloExpr c1(env);
		c1+= sommets[i].first + sommets[i].second ;
		CC.add(c1 == 1);
		nbCst++;

	}


	
	//contraintes arêtes 

	int i = 0;
	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		IloExpr c1(env);
		c1 += sommets[(*itEdge)->_first].first - sommets[(*itEdge)->_last].first- edge[i];
		CC.add(c1 <= 0);
		
		IloExpr c2(env);
		c2 += sommets[(*itEdge)->_last].first - sommets[(*itEdge)->_first].first  - edge[i];
		CC.add(c2 <= 0);

		IloExpr c3(env);
		c3+= sommets[(*itEdge)->_first].first + sommets[(*itEdge)->_last].first + edge[i];
		CC.add(c3 <= 2);
		nbCst = nbCst+3;
		i++;
	}

	i = 0;
	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		IloExpr c1(env);
		c1 += sommets[(*itEdge)->_first].second - sommets[(*itEdge)->_last].second- edge[i];
		CC.add(c1 <= 0);

		IloExpr c2(env);
		c2 += sommets[(*itEdge)->_last].second - sommets[(*itEdge)->_first].second  - edge[i];
		CC.add(c2 <= 0);

		IloExpr c3(env);
		c3+= sommets[(*itEdge)->_first].second + sommets[(*itEdge)->_last].second + edge[i];
		CC.add(c3 <= 2);
		nbCst=nbCst+3;
		i++;
	}

	// contraintes 2a

	IloExpr c2(env);
	IloExpr c3(env);

	for(int i = 0; i < G._nbNodes ; i ++){
		c2 += puits[i].first;
		c3 += puits[i].second;


	}
	CC.add(c2 == 1);
	CC.add(c3==1);
	nbCst=nbCst+2;
	//contrainte 2b
	

	for(int i = 0 ; i < sommets.size(); i ++){
		IloExpr c4(env);
		IloExpr c5(env);

		c4 +=puits[i].first- sommets[i].first;
		
		c5 +=puits[i].second - sommets[i].second;
		
		CC.add(c4 <= 0);
		CC.add(c5 <= 0);
		nbCst=nbCst+2;
	}

	// contraintes 2c

	for(int i = 0 ; i< flot.size(); i ++){
		
		IloExpr c6(env);
		c6 += flot[i].first + flot[i].second + edge[i] * (G._nbNodes -1);
		CC.add(c6 <= (G._nbNodes -1));	
		nbCst++;
	}

	//contraintes 2d
	
	for(int i = 0; i < G._nbNodes ; i ++){

		IloExpr c7(env);
		
		c7 += (G._nbNodes-1) * (puits[i].first + puits[i].second   );

		list<pair<int,Edge*> >::iterator it;
		for(it =  G._nodes[i]._incList.begin() ; it != G._nodes[i]._incList.end();it++){
			
			if(it->second->_first == i){
				c7 +=  flot[it->second->_id].first - flot[it->second->_id].second;
			}
			else{
				c7 += flot[it->second->_id].second - flot[it->second->_id].first;
			}
			

		}
		CC.add(c7 >=1);
		nbCst++;
	}

	
	
	


	model.add(CC);
	
    // And Finally
	
    IloCplex cplex(model);
	    cplex.setParam(IloCplex::Param::TimeLimit, 3600);

	IloObjective obj=IloAdd(model, IloMaximize(env, 0.0));
	i = 0;	
	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		
		obj.setLinearCoef(edge[i], (*itEdge)->_cost);
		i++;
	}
	auto start = chrono::steady_clock::now();
	stat.start = clock();
	cplex.solve();
	stat.end = clock();
	auto end = chrono::steady_clock::now();
	
	i = 0;
    for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
      (*itEdge)->_valx = cplex.getValue(edge[i]);
	if( cplex.getValue(edge[i])> epsilon) cout<<(*itEdge)->_first << " "<< (*itEdge)->_last<<endl;
	i++;
    }
	    env.out() << "Solution value  = " << cplex.getObjValue() << endl;

	
   	stat.nbNodes = cplex.getNnodes();
	stat.optimalityGap = cplex.getMIPRelativeGap();
	stat.nbNoeudInstance = G._nbNodes;
	stat.nbAreteInstance  = G._edges.size();
	stat.nbCst = nbCst;
	stat.name = bl;
	stat.totalTime = chrono::duration_cast<chrono::seconds>(end - start).count();
	stat.printInfo();

	stat.writeFile(fResultName);
	
    cplex.end();
	
	}
	catch(IloException& e) {
	cout << "ERROR: " << e << endl;
	}
	env.end();
	return 0;
//
	
	
}
