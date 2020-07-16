#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "Graph.h"
#define epsilon 0.00001
int main(int argc, char** argv){
	IloEnv env;

	try{
		IloModel model(env);

		if (argc < 2) {
	    	cout << "Error : the command should be followed by a filename" << endl;
	    	exit(1);
		}

		Graph G(argv[1]);
		

	G.MakeCplexVarFlot(env);
	int nbcst = 0;
	IloRangeArray CC(env);
	IloExpr c1(env);


	// constraintes de fortet

	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		IloExpr c1(env);
		c1+= -(*itEdge)->_x;
		c1+= G._nodes[(*itEdge)->_first]._s;
		c1+= - G._nodes[(*itEdge)->_last]._s;
		CC.add(c1 <= 0);
	}


	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		IloExpr c1(env);
		c1+= -(*itEdge)->_x;
		c1+= -G._nodes[(*itEdge)->_first]._s;
		c1+=  G._nodes[(*itEdge)->_last]._s;
		CC.add(c1 <= 0);
	}

	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		IloExpr c1(env);
		c1+= (*itEdge)->_x;
		c1+= -G._nodes[(*itEdge)->_first]._s;
		c1+= -G._nodes[(*itEdge)->_last]._s;
		CC.add(c1 <= 0);
	}


	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		IloExpr c1(env);
		c1+= (*itEdge)->_x;
		c1+= G._nodes[(*itEdge)->_first]._s;
		c1+=  G._nodes[(*itEdge)->_last]._s;
		CC.add(c1 <= 2);
	}
	//contraintes de flot v -> W, passant par le sommet u différent de v et w.

	for(int u = 0 ; u < G._nodes.size(); u++){
		for(int v = 0 ; v < G._nodes.size() ; v ++){
			for(int w  = 0 ; w < v ; w ++){
				if(u != v && u != w){
					IloExpr c1(env);
					for (list<pair<int,Edge*>>::iterator itEdge = G._nodes[u]._incList.begin(); itEdge != G._nodes[u]._incList.end(); itEdge++){
						if((*itEdge->second)._first == u){
							c1 += (*itEdge->second).flotFL[v][w] - (*itEdge->second).flotLF[v][w] ; 
						}
						else{
							c1 += - (*itEdge->second).flotFL[v][w] + (*itEdge->second).flotLF[v][w] ; 
						}
					}
					CC.add(c1 == 0);
				}
				

			}
		
		}


	}

	//contraintes de flot u -> v, sortant du sommet u
	for(int u = 0 ; u < G._nodes.size(); u++){
		for(int v = 0 ; v < u ; v ++){
			IloExpr c1(env);
			IloExpr c2(env);
			for (list<pair<int,Edge*>>::iterator itEdge = G._nodes[u]._incList.begin(); itEdge != G._nodes[u]._incList.end(); itEdge++){
				if((*itEdge->second)._first == u){
					c1 += (*itEdge->second).flotFL[u][v]  ; 
					c2 += (*itEdge->second).flotFL[u][v]  ; 
				}
				else{
					c1 +=   (*itEdge->second).flotLF[u][v] ; 
					c2 +=   (*itEdge->second).flotLF[u][v] ; 
				}
				
			}
			c1+= G._nodes[u]._s;
			c1+= G._nodes[v]._s;
			c2+= -G._nodes[u]._s;
			c2+= -G._nodes[v]._s;
			CC.add(c1 >= 1);
			CC.add(c2 >= -1);
			
			
			
		}

	}

	//contraintes de flot u -> v, entrant au  sommet v
	for(int u = 0 ; u < G._nodes.size(); u++){
		for(int v = 0 ; v < u ; v ++){
			IloExpr c1(env);
			IloExpr c2(env);
			for (list<pair<int,Edge*>>::iterator itEdge = G._nodes[v]._incList.begin(); itEdge != G._nodes[v]._incList.end(); itEdge++){
				if((*itEdge->second)._last == v){
					c1 += (*itEdge->second).flotFL[u][v]  ; 
					c2 += (*itEdge->second).flotFL[u][v]  ; 
				}
				else{
					c1 +=   (*itEdge->second).flotLF[u][v] ; 
					c2 +=   (*itEdge->second).flotLF[u][v] ; 
				}
				
			}
			c1+= G._nodes[u]._s;
			c1+= G._nodes[v]._s;
			c2+= -G._nodes[u]._s;
			c2+= -G._nodes[v]._s;
			CC.add(c1 >= 1);
			CC.add(c2 >= -1);
			
			
			
		}

	}


	// contraintes de 0 permettant d'empêcher le flot allant de u à v de retourner en u 
	for(int u = 0 ; u < G._nodes.size(); u++){
		for(int v = 0 ; v < u ; v ++){

			for (list<pair<int,Edge*>>::iterator itEdge = G._nodes[u]._incList.begin(); itEdge != G._nodes[u]._incList.end(); itEdge++){
				IloExpr c1(env);
				
				if((*itEdge->second)._last == u){
					c1 += (*itEdge->second).flotFL[u][v];
				}
				else{
					c1 += (*itEdge->second).flotLF[u][v];
				}
				CC.add(c1 == 0);
			}
			for (list<pair<int,Edge*>>::iterator itEdge = G._nodes[v]._incList.begin(); itEdge != G._nodes[v]._incList.end(); itEdge++){
				IloExpr c2(env);
				if((*itEdge->second)._last == v){
					c2 += (*itEdge->second).flotLF[u][v];
				}
				else{
					c2 += (*itEdge->second).flotFL[u][v];
				}
				CC.add(c2 == 0);
			}
			
		}

	}
	
	//contrainte empêchant le flot d'emprunter une arête coupée

	for(int u = 0; u < G._nodes.size(); u ++){
		for(int v = 0 ; v < u; v ++){
			for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
				IloExpr c1(env);
				IloExpr c2(env);
				c1+= (*itEdge)->flotFL[u][v];
				c2+= (*itEdge)->flotLF[u][v];
				c1+= (*itEdge)->_x;
				c2+= (*itEdge)->_x;
				CC.add(c1 <=1);
				CC.add(c2 <=1); 
			}
			
		}
	

	}
	

	


	


	model.add(CC);
	
    // And Finally
    IloCplex cplex(model);
	

	IloObjective obj=IloAdd(model, IloMaximize(env, 0.0));
	
	for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		obj.setLinearCoef((*itEdge)->_x, (*itEdge)->_cost);
	}
	cplex.solve();
	
    for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
      (*itEdge)->_valx = cplex.getValue((*itEdge)->_x);
	if( (*itEdge)->_valx> epsilon) cout<<(*itEdge)->_first << " "<< (*itEdge)->_last<<endl;
	
    }
	cout<< "sommet 0 : "<< cplex.getValue(G._nodes[0]._s) << endl;
   	cout<< "sommet 1 : "<< cplex.getValue(G._nodes[1]._s) << endl;
	cout<< "sommet 2 : "<< cplex.getValue(G._nodes[2]._s) << endl;
	cout<< "sommet 3 : "<< cplex.getValue(G._nodes[3]._s) << endl;
	cout<< "sommet 4 : "<< cplex.getValue(G._nodes[4]._s) << endl;
    G.Write_sol_ps_pdf();
    cplex.end();

	}
	catch(IloException& e) {
	cout << "ERROR: " << e << endl;
	}
	env.end();
	return 0;

	
	
}
