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
#define MAXCST 200



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
					if(nbCst >= MAXCST){
							return result;
						}	
				}
			}
		
		}


	}
	
	return result;


}

void plMinSeparator(Graph & g, IloEnv env, ModelMinSeparator& momo,list<IloRange> & result){ // 
	
	
	
	int nbCst = 0;

	#ifdef _OUTPUT_
	cout<<"Deb plMinSep"<<endl;
	#endif

	
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
				cout<<endl<<"plmin: ";
				for (itt=g._edges.begin();itt!=g._edges.end();itt++){
				  cout<<" | "<<(*it2)<<" "<<(1- (*itt)->_valx);
				  momo.obj.setLinearCoef( (*it2) , (1- (*itt)->_valx));
					
			                it2++;	
	 		  	}				
				//cout<<"kikiikiki"<<endl;
				//	momo.model.add(momo.obj);
				//cout<<"kokookokokokokokokok"<<endl;
				
 				 momo.cplex.exportModel("sortie.lp");
  // int y;
  // cin>>y;
				
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
				
				if(nbCst >= MAXCST ){
						return ;
					}
				momo.deleta(k,z);	
			}
		
		
		}


	}

        #ifdef _OUTPUT_
	cout<<"Fin plMinSep"<<endl;
	#endif

	
	


}



void gloutGlout(Graph & g, IloEnv env, list<IloRange> & violatedConst){

	list<int> w1;
	list<int> w2;
	list<int> adjW1;
	list<int> adjW2;
	bool b1; // l'ensemble w1 ne peut pas être amélioré localement
	int  cost = 0;

	
	bool b2; //l'ensemble w2 ne peut pas être amélioré localement
	bool klm = true;
	int nbCst = 0;
	for(int k= 0 ; k < g._nbNodes  ; k ++){
		for(int z = 0; z < k; z ++){
			
			bool b = true ;
			list<pair<int,Edge*> >::iterator it;
			
			for(it = g._nodes[k]._incList.begin(); it != g._nodes[k]._incList.end();it ++){
				if(it->first == z){
					b=false;
				}
			}
			if(b and klm){
				w1.push_back(k);
				w2.push_back(z);
				list<pair<int,Edge*> >::iterator it ;
				g.gloutInit();

				for(it = g._nodes[k]._incList.begin() ; it != g._nodes[k]._incList.end(); it ++){

					adjW1.push_back((*it).first);
					g._nodes[(*it).first].adjW1 = true;
					(*it).second->aCheval = true;
					cost += (*it).second->_valx;
		


				}
				for(it = g._nodes[z]._incList.begin() ; it != g._nodes[z]._incList.end(); it ++){
		
					adjW2.push_back((*it).first);	
					g._nodes[(*it).first].adjW2 = true;
					(*it).second->aCheval = true;
					cost += (*it).second->_valx;
				}

				b1 = true;
				b2 = true;

				while( b1 or b2){
					
					if(b1){
						b1=false;  // si un noeud est ajouté dans W1 ce booléen devra repasser à true
						list<int>::iterator itt;
						int minIndice= -1;
						float min =0;
						int v;
						for(itt = adjW1.begin() ; itt != adjW1.end(); itt ++){
							




							if(    g._nodes[*itt].adjW1 and not g._nodes[*itt].adjW2     ) {
						       



							 // si le noeud n'est pas adjacent à w1 ET w2 en même temps (sinon il est forcément dans le séparateur)
								


								v = g._nodes[*itt].val();
							

								if( min >= v) {
									minIndice  = *itt;
									min = v;
									
								}

								

							}

						
						}
						if(minIndice != -1){ // un noeud peut être ajouté dans W1
					
							b1 = true;
							adjW1.remove(minIndice);
							w1.push_back(minIndice);

							
							for(it = g._nodes[minIndice]._incList.begin() ; it != g._nodes[minIndice]._incList.end(); it ++){
								
								if((*it).second->aCheval){
									cost = cost - (*it).second->_valx;
									(*it).second->aCheval = false;
								}
								else{
									
									(*it).second->aCheval = true;
									cost = cost + (*it).second->_valx;
									
									if(not g._nodes[(*it).first].adjW1){
										g._nodes[(*it).first].adjW1 = true;
										adjW1.push_back((*it).first);

									}
								}
							

							}


						}


					}
					else if (b2){


						b2=false;  // si un noeud est ajouté dans W2 ce booléen devra repasser à true
						list<int>::iterator itt;
						int minIndice= -1;
						float min =0;
						int v;
						for(itt = adjW2.begin() ; itt != adjW2.end(); itt ++){
							




							if(   g._nodes[*itt].adjW2 and not g._nodes[*itt].adjW1) {
						       



							 // si le noeud n'est pas adjacent à w1 ET w2 en même temps (sinon il est forcément dans le séparateur)
								


								v = g._nodes[*itt].val();
							

								if( min >= v) {
									minIndice  = *itt;
									min = v;
									
								}

								

							}

						
						}
						if(minIndice != -1){ // un noeud peut être ajouté dans W2
					
							b2 = true;
							adjW2.remove(minIndice);
							w2.push_back(minIndice);

							
							for(it = g._nodes[minIndice]._incList.begin() ; it != g._nodes[minIndice]._incList.end(); it ++){
								
								if((*it).second->aCheval){
									cost = cost - (*it).second->_valx;
									(*it).second->aCheval = false;

								}
								else{
									
									(*it).second->aCheval = true;
									cost = cost + (*it).second->_valx;
									g._nodes[(*it).first].adjW2 = true;
								}
							

							}


						}



					}

				}

				
				if(cost <2){ //la contrainte associée est violée on va donc construire la contrainte puis l'ajouter dans violatedConst
					int n = 0;
					IloExpr cst(env);
					list<Edge*>::iterator l;
					for(l = g._edges.begin(); l != g._edges.end(); l ++){

						if( (*l)->aCheval){
							n++;
							cst+= (*l)->_x;

						}
						

					}
					violatedConst.push_back(IloRange(cst <= n-2));
					nbCst ++;
					if(nbCst == MAXCST){
						klm=false;

					}
					

				}

				




			
				w1.clear();
				w2.clear();
				adjW1.clear();
				adjW2.clear();
	
				
					
			}
		
		
		}


	}

	
	


	



}





