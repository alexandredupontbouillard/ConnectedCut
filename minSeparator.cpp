#include <lemon/dijkstra.h>
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "minSeparator.h"
#include "Graph.h"
#include <algorithm>

#include "ModelMinSeparator.h"
#define epsilon 0.00001
#define precision 1000
#define maxCst 200



using namespace::std;


list<IloRange> plMinSeparatorRandom(Graph & g, IloEnv env){ // 
	
	vector<int> ff(g._nbNodes);
	vector<int> ll(g._nbNodes);

	for(int i = 0 ; i < g._nbNodes; i ++){
		ff[i]=i;
		ll[i]=i;

	}
	random_shuffle(ff.begin(), ff.end());
	random_shuffle(ll.begin(), ll.end());
	int z;
	int k;
	list<IloRange> result;
	int nbCst = 0;
	for(int kk = 0 ; kk < g._nbNodes ; kk ++){
		k = ff[kk];
		for(int zz = 0; zz < g._nbNodes; zz ++){
			z = ll[zz];
			if(z != k){
				bool b = true;
				list<pair<int,Edge*> >::iterator it;
				for(it = g._nodes[k]._incList.begin(); it != g._nodes[k]._incList.end();it ++){
					if(it->first == z){
						b=false;
					}
				}
				if(b){
					IloEnv env1;
					
					IloModel model(env1);
					list<Edge*>::const_iterator it;

					///VARIABLES
					vector<IloNumVar>  vertice ; 
					vertice.resize(g._nbNodes);
					
					list<IloNumVar> edges;
					
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

					IloExpr c1(env1);
					c1 += vertice[k];
					CC.add(c1== 1);
					IloExpr c2(env1);
					c2 += vertice[z];
					CC.add(c2== -1);

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
					cout<<"nb contraintes : "<<size<<endl;
		   
		  			IloCplex cplex(model); 
				
				  	//OBJ

					IloObjective obj=IloAdd(model, IloMinimize(env1, 0.0));
					it2 =edges.begin(); ;
					for (it=g._edges.begin();it!=g._edges.end();it++){
						obj.setLinearCoef((*it2),1- (*it)->_valx);
						
				                it2++;	
		 		  	}
					if ( !cplex.solve() ) {
					 env1.error() << "Failed to optimize LP" << endl;
					    exit(1);
					  }
					else{
						env1.out() << "Solution value  = " << cplex.getObjValue() << endl;
					}
					
				
					if( cplex.getObjValue()<2 ){
						int nbVar= 0;
						nbCst += 1;
						IloExpr Laclhs(env);

						list<IloNumVar>::const_iterator it2 =edges.begin(); ;
						for (it=g._edges.begin();it!=g._edges.end();it++){
							if(cplex.getValue((*it2))>1- epsilon){
								Laclhs += (*it)->_x;
								nbVar++;
							}
						        it2++;	
			 		  	}
						result.push_back(IloRange(Laclhs <= nbVar - 2));
						
						
					}
					env1.end();
					if(nbCst >= maxCst ){
							return result;
						}	
				}
			}
		
		}


	}
	
	return result;


}


list<IloRange> plMinSeparator(Graph & g, IloEnv env, ModelMinSeparator momo){ // 
	
	
	list<IloRange> result;
	int nbCst = 0;
	for(int k= 0 ; k < g._nbNodes ; k ++){
		for(int z = 0; z < k; z ++){
			
			bool b = true;
			list<pair<int,Edge*> >::iterator it;
			list<IloNumVar>::iterator it2;
			for(it = g._nodes[k]._incList.begin(); it != g._nodes[k]._incList.end();it ++){
				if(it->first == z){
					b=false;
				}
			}
			if(b){
			
				momo.add(k,z);
			
			  	
				

					
				list<Edge*>::iterator itt ;

				it2 =momo.edges.begin(); 
				for (itt=g._edges.begin();itt!=g._edges.end();itt++){
					momo.obj.setLinearCoef( (*it2) , (1- (*itt)->_valx));
					
			                it2++;	
	 		  	}
				
				momo.model.add(momo.obj);
				cout<<"kokookokokokokokokok"<<endl;
				if ( !momo.cplex.solve() ) {
				 momo.env1.error() << "Failed to optimize LP" << endl;
				    exit(1);
				  }
				else{
					momo.env1.out() << "Solution value  = " << momo.cplex.getObjValue() << endl;
				}
				
			
				if( momo.cplex.getObjValue()<2 ){
					int nbVar= 0;
					nbCst += 1;
					IloExpr Laclhs(env);

					list<IloNumVar>::const_iterator it2 =momo.edges.begin(); ;
					for (itt=g._edges.begin();itt!=g._edges.end();itt++){
						if(momo.cplex.getValue((*it2))>1- epsilon){
							Laclhs += (*itt)->_x;
							nbVar++;
						}
					        it2++;	
		 		  	}
					result.push_back(IloRange(Laclhs <= nbVar - 2));
					
					
				}
				
				if(nbCst >= maxCst ){
						return result;
					}
				momo.deleta(k,z);	
			}
		
		
		}


	}
	
	return result;


}





