#include <ilcplex/ilocplex.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "minSeparator.h"	
#include "Graph.h"
#include "ModelMinSeparator.h"

#define epsilon 0.00001
#define fResultName "result.csv"
using namespace::std;

// ILOSTLBEGIN

#include "statStruct.h"

struct poolStruct{
  IloRange ineq;
  enum constType type;
};



/* Define the initial of the model */

void StateModel(IloModel model, Graph & G){
  IloEnv env = model.getEnv();

  /* Variables instanciation */

  G.MakeCplexVar(env);

  /*********************** Remark ***********************/
  /* x and y variables will be defined only for i < j   */
  /* z variables will be all defined, the order matters */
  /******************************************************/

  /* Adding constraints */

  /* Cardinality constraint on Y variables */
  IloExpr cardY(env);
  for (list<Edge *>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
    cardY += (*itEdge)->_y;
  }
  model.add(cardY == G._nbNodes-1);
  cardY.end();


  /* Constraints positionning u wrt to an edge {v,w} */
  for (list<Edge *>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
    int v = (*itEdge)->_first;
    int w = (*itEdge)->_last;
    for (IloInt u = 0; u < G._nbNodes; u++) {
      if ((u != v) && (u != w)){
	model.add((*itEdge)->_y - (*itEdge)->_z[u] - (*itEdge)->_zrev[u] == 0);
      }
    }
  }


  /* Either we take edge {v,w} or there is u such that */
  /* {v,u} is taken and w is on the side of u */

  for (IloInt v = 0; v < G._nbNodes-1; v++){
    for (IloInt w = v+1; w < G._nbNodes; w++){ 
      IloExpr v1(env); /* v first in z variables */
      IloExpr v2(env); /* w first in z variables */
      if (G._nodes[v][w]){
	v1 += G._nodes[v][w]->_y;
	v2 += G._nodes[v][w]->_y;
      }
      for (IloInt u = 0; u < G._nbNodes; u++){
	if ((u != v) && (u != w)){

	  if (G._nodes[v][u]){
	    if (v < u) {
	      v1 += G._nodes[v][u]->_z[w];
	    } else {
	      v1 += G._nodes[u][v]->_zrev[w];
	    }
	  }

	  if (G._nodes[w][u]){
	    if (w < u) {
	      v2 += G._nodes[w][u]->_z[v];
	    } else {
	      v2 += G._nodes[u][w]->_zrev[v];
	    }
	  }

	}
      }

      model.add(v1 == 1);
      model.add(v2 == 1);
      v1.end();
      v2.end();
    }
  }
  

  /*Objective function */

  IloExpr obj(env);
  for (list<Edge *>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
    obj += (*itEdge)->_cost * (*itEdge)->_x;
  }


  model.add(IloMaximize(env,obj));
}


void StateModeW1W2(IloModel model, Graph & G){
  IloEnv env = model.getEnv();
  G.MakeCplexVarW1W2(env);
  /* Variables instanciation */

		
	 IloExpr obj(env);
  for (list<Edge *>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
    obj += (*itEdge)->_cost * (*itEdge)->_x;
  }


  model.add(IloMaximize(env,obj));
}


bool ComparePtEdgeXY(Edge * edg1, Edge * edg2){
  IloNum XYedg1 = edg1->_valx + edg1->_valy;
  IloNum XYedg2 = edg2->_valx + edg2->_valy;
  if (XYedg1 > XYedg2 + EPS){
    return true;
  } else {
    return false;
  }
}


void W1W2Sep(Graph &G, IloEnv env,
		list<poolStruct> & violatedConst){
	
	vector<int> v(G._edges.size());
	Graph test3Cut(G._nbNodes);
	int i = 0;
	for ( list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
            if( (*itEdge)->_valx <= epsilon){
		test3Cut.AddEdge(i,(*itEdge)->_first,(*itEdge)->_last,0 );
		i++;
		}
	}
	

	if(not test3Cut.connected()){
		
		IloExpr Laclhs(env);
		int f = 0;
		for( list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
			if( (*itEdge)->_valx >= epsilon){
				Laclhs += (*itEdge)->_x;
				f++;
			}
		}
		 poolStruct newConst;
	         newConst.ineq = IloRange(Laclhs <= f - 2);
	         newConst.type = W1W2I;
	         violatedConst.push_back(newConst);
	}
	

}

void createGp(Graph &G, Graph &Gp){
  int n,i,j,k;

  k=0;
  n=G._nbNodes;
  for (i=0;i<n;i++){
    Gp._nodes[i]._id=i;
    Gp._nodes[i+n]._id=i+n;
    for (j=i+1;j<n;j++){
      if (G._nodes[i][j]!=NULL){
	Gp.AddEdge(k,i,j,-1);
	Gp.AddEdge(k,i+n,j+n,-1);
	Gp.AddEdge(k,i,j+n,-1);
	Gp.AddEdge(k,j,i+n,-1);
	k++;
      }
    }
  }
}


// Recherche le plus petit cycle a l'interieur de l'inegalite
void extract_elementary_cycle(list<pair<pair<int,int>,int> > &L,int n){
  int i;
  list<pair<pair<int,int>,int> >::iterator itL;
 
  vector<pair<bool, list<pair<pair<int,int>,int> >::iterator> > mark;
  mark.resize(n);
  for (i=0;i<n;i++) mark[i].first=false;

  #ifdef _OUTPUT_
  cout<<"avt: ";
  for (itL=L.begin();itL!=L.end();itL++)
    cout<<"("<<(*itL).second<<", ("<<(*itL).first.first<<","<<(*itL).first.second<<")) ";
  cout<<endl;
  #endif

  itL=L.begin();
  mark[(*itL).first.first].first=1;
  mark[(*itL).first.first].second=itL;
  while ( (itL!=L.end()) && (!mark[(*itL).first.second].first) ){
      mark[(*itL).first.second].first=1;
      mark[(*itL).first.second].second=itL;
      itL++;
  }
  L.erase(L.begin(),mark[(*itL).first.second].second);
  itL++;
  L.erase(itL,L.end());

  #ifdef _OUTPUT_
  cout<<"apr: ";
  for (itL=L.begin();itL!=L.end();itL++)
    cout<<"("<<(*itL).second<<", ("<<(*itL).first.first<<","<<(*itL).first.second<<")) ";
  cout<<endl;
  #endif

  
}


void cycleSep(Graph & G, Graph &Gp,
		IloEnv env,
	      list<poolStruct> & violatedConst){
  
  int i,j,u,n;
  list<Edge*>::iterator it;
  list<pair<pair<int,int>,int> > L; // liste d'arete suivi de 0 si dans C\F et 1 i dans F
  list<pair<pair<int,int>,int> >::iterator itL;
  double val;
 	for(int ii = 0; ii < G._nodes.size(); ii++){

		G._nodes[i].visitedCycle = false;

	}
  n=G._nbNodes;

  vector<int>T=vector<int>(2*n,0);
  vector<double>D=vector<double>(2*n,0);
  vector<int>card=vector<int>(2*n,0);
  heap H(2*n);
 
  n=G._nbNodes;

  /* fill the edges of Gp */
  for (it = G._edges.begin(); it != G._edges.end(); it++){
    val=(*it)->_valx;
    if (val<EPS) val=0;
    Gp._nodes[(*it)->_first][(*it)->_last]->_cost=val;
    Gp._nodes[((*it)->_first)+n][((*it)->_last)+n]->_cost=val;
    Gp._nodes[((*it)->_first)+n][(*it)->_last]->_cost=1-val;
    Gp._nodes[(*it)->_first][((*it)->_last)+n]->_cost=1-val;
  }
 
  /* Loop on the node looking for a path from i to i */
  //cout<<"Launch cycle separation main loop\n"<<endl;
  for (i=0;i<n;i++){
	if(not G._nodes[i].visitedCycle){
		/* Launch Dijkstra */
	 	
		val=Gp.Dijkstra(i,i+n,T,D,card,H);

		/* Translate from path in Gp to set C and F of the constraint */
		//cout<<"violation: "<<val<<endl;
		// for (j=0;j<2*n;j++)
		//   cout<<"T["<<j<<"]="<<T[j]<<endl;
		// cout<<endl;

		if ( (T[i+n]!=-1)&&(1-val>EPS) ){  // The support graph contains a path from i to i+n
		  u=i+n;
		  L.clear();
		  while(u!=i){
			
			
		if ((u<n)&&(T[u]<n))
		  L.push_back(make_pair(make_pair(u,T[u]),0));
			//G._nodes[u].visitedCycle = true;
		if ((u>=n)&&(T[u]>=n))
		  L.push_back(make_pair(make_pair(u-n,T[u]-n),0));
			//G._nodes[u-n].visitedCycle = true;
		
		if ((u<n)&&(T[u]>=n))
		  L.push_back(make_pair(make_pair(u,T[u]-n),1));
		//G._nodes[u].visitedCycle = true;
		if ((u>=n)&&(T[u]<n))
		  L.push_back(make_pair(make_pair(u-n,T[u]),1));
		//G._nodes[u-n].visitedCycle = true;
		u=T[u];
		  }
		
		 
		  // for (itL=L.begin();itL!=L.end();itL++)
		  // 	cout<<"("<<(*itL).first.first<<","<<(*itL).first.second<<","<<(*itL).second<<") ";
		  // cout<<endl;
		  // cin>>u;

		  extract_elementary_cycle(L,G._nbNodes);

		  /* Translate into a cplex cste */

		  IloExpr Laclhs(env);
		  IloInt F;
		  F=0;
			itL=L.begin();
			G._nodes[(*itL).first.first][(*itL).first.second]->_valx = 100;
		  for (itL=L.begin();itL!=L.end();itL++){
			if ((*itL).second==0)
			  Laclhs -= G._nodes[(*itL).first.first][(*itL).first.second]->_x;
			else{
			  Laclhs += G._nodes[(*itL).first.first][(*itL).first.second]->_x;
			  F++;
			}
			//cout<<"yo"<<endl;
			
			//cout<<"ya"<<endl;	
		  }
			
		  poolStruct newConst;
		  newConst.ineq = IloRange(Laclhs <= F - 1);
		  newConst.type = CYCLE;
		  violatedConst.push_back(newConst);
		  #ifdef _OUTPUT_
		  cout<< newConst.ineq << endl;
		  #endif
			 
		}
		
	}
 
  }


}

ILOLAZYCONSTRAINTCALLBACK6(GenLazyCut,
			   Graph &, G,
			   Graph &, Gp,
			   list<poolStruct>&, violatedConst,
			   statStruct&, stat,
			   bool, linRelax,
				ModelMinSeparator&, momo){


  //cout << "******* Seperation phase ********" << endl;
  clock_t t;
  /* Checking the pool of violated constraints */
  if (!violatedConst.empty()){
    while (!violatedConst.empty()){
      add(violatedConst.front().ineq,IloCplex::UseCutPurge);
      stat.nbCuts[violatedConst.front().type]++;
      // cout << "Adding " << violatedConst.front().type << " constraint" << endl;
      violatedConst.pop_front();
    }
  } else {

    /* Retrieving x and y values */
    for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
      (*itEdge)->_valx = getValue((*itEdge)->_x);
	
    }


	    t = clock();

	//list<IloRange> l = plMinSeparatorRandom(G,getEnv());
	
	list<IloRange> l ;
	 plMinSeparator(G,getEnv(),momo,l);
	t = clock()-t;
	stat.temps[W1W2] += t;
	
	while(!l.empty()){
		add(l.front(),IloCplex::UseCutPurge);
		l.pop_front();
		stat.nbCuts[W1W2]++;
	}




    /* Check Cycle inequalities */
    t = clock();
    cycleSep(G,Gp,getEnv(),violatedConst);
    t = clock()-t;
    stat.temps[CYCLE]+=t;



	
    




    /*cout << "Violated constraints pool size = " << violatedConst.size() << endl;
    if (linRelax && violatedConst.empty()){
      cout << "Solution Value : " << getObjValue() << endl;
      cout << endl;

      
	
    }*/

    while (!violatedConst.empty()){
      // cout << "Adding constraint : " << violatedConst.front().ineq << endl;
      add(violatedConst.front().ineq,IloCplex::UseCutPurge);
      stat.nbCuts[violatedConst.front().type]++;
      violatedConst.pop_front();
    }


  }

}

ILOUSERCUTCALLBACK6(GenUserCut,
			   Graph &, G,
			   Graph &, Gp,
			   list<poolStruct>&, violatedConst,
			   statStruct&, stat,
		           bool, linRelax,
				ModelMinSeparator&, momo){
	clock_t t;
	if (!violatedConst.empty()){
	    while (!violatedConst.empty()){
	      add(violatedConst.front().ineq,IloCplex::UseCutPurge);
	      stat.nbCuts[violatedConst.front().type]++;
	      // cout << "Adding " << violatedConst.front().type << " constraint" << endl;
	      violatedConst.pop_front();
	    }
	  } else {

	    /* Retrieving x values */
	    for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
	      (*itEdge)->_valx = getValue((*itEdge)->_x);
		
	    }


	t = clock();

	//list<IloRange> l = plMinSeparatorRandom(G,getEnv());

	list<IloRange> l;
    plMinSeparator(G,getEnv(),momo,l);
	t = clock()-t;
	stat.temps[W1W2] +=t;	
	

	


	while(!l.empty()){
		add(l.front(),IloCplex::UseCutPurge);
		l.pop_front();
		stat.nbCuts[W1W2]++;
	}
	


	    /* Check Cycle inequalities */
	    t = clock();
	    cycleSep(G,Gp,getEnv(),violatedConst);
	    t = clock()-t;
	    stat.temps[CYCLE]+= t;

	
	

	
	
	
	

	 while (!violatedConst.empty()){
	      // cout << "Adding constraint : " << violatedConst.front().ineq << endl;
	      add(violatedConst.front().ineq,IloCplex::UseCutPurge);
	      stat.nbCuts[violatedConst.front().type]++;
	      violatedConst.pop_front();
	    }


	}


}






ILOLAZYCONSTRAINTCALLBACK6(SimpleLazyCut,
			   Graph &, G,
			   Graph &, Gp,
			   list<poolStruct>&, violatedConst,
			   statStruct&, stat,
			   bool, linRelax,
				ModelMinSeparator&, momo){


  //cout << "******* Seperation phase ********" << endl;
  clock_t t;
  /* Checking the pool of violated constraints */
  if (!violatedConst.empty()){
    while (!violatedConst.empty()){
      add(violatedConst.front().ineq,IloCplex::UseCutPurge);
      stat.nbCuts[violatedConst.front().type]++;
      // cout << "Adding " << violatedConst.front().type << " constraint" << endl;
      violatedConst.pop_front();
    }
  } else {

    /* Retrieving x and y values */
    for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
      (*itEdge)->_valx = getValue((*itEdge)->_x);
	
    }


	cout<<"separation entière O(n)"<<endl;
	t = clock();
	W1W2Sep( G, getEnv(),violatedConst);
	
	t = clock()-t;
	stat.temps[W1W2I] += t;
		
	cout<<"separation entière O(n) finie"<<endl;

    /* Check Cycle inequalities */
    t = clock();
    cycleSep(G,Gp,getEnv(),violatedConst);
    t = clock()-t;
    stat.temps[CYCLE]+=t;



	




    while (!violatedConst.empty()){
      // cout << "Adding constraint : " << violatedConst.front().ineq << endl;
      add(violatedConst.front().ineq,IloCplex::UseCutPurge);
      stat.nbCuts[violatedConst.front().type]++;
      violatedConst.pop_front();
    }


  }

}

ILOUSERCUTCALLBACK6(GloutUserCut,
			   Graph &, G,
			   Graph &, Gp,
			   list<poolStruct>&, violatedConst,
			   statStruct&, stat,
		           bool, linRelax,
				ModelMinSeparator&, momo){
	clock_t t;
	if (!violatedConst.empty()){
		while (!violatedConst.empty()){
		  add(violatedConst.front().ineq,IloCplex::UseCutPurge);
		  stat.nbCuts[violatedConst.front().type]++;
		  // cout << "Adding " << violatedConst.front().type << " constraint" << endl;
		  violatedConst.pop_front();
		}
	  } 
	else {

		/* Retrieving x values */
		for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		  (*itEdge)->_valx = getValue((*itEdge)->_x);
		
		}



			t = clock();

		list<IloRange> l;
		gloutGlout(G,getEnv(), l);
		t = clock()-t;
		stat.temps[GLOUTON] +=t;





		while(!l.empty()){
			add(l.front(),IloCplex::UseCutPurge);
			l.pop_front();
			stat.nbCuts[GLOUTON]++;
		}

		/* Check Cycle inequalities */
		t = clock();
		cycleSep(G,Gp,getEnv(),violatedConst);
		t = clock()-t;
		stat.temps[CYCLE]+= t;



	






		 while (!violatedConst.empty()){
			  // cout << "Adding constraint : " << violatedConst.front().ineq << endl;
			  add(violatedConst.front().ineq,IloCplex::UseCutPurge);
			  stat.nbCuts[violatedConst.front().type]++;
			  violatedConst.pop_front();
		}


	}


}


ILOUSERCUTCALLBACK6(GloutExactUserCut,
			   Graph &, G,
			   Graph &, Gp,
			   list<poolStruct>&, violatedConst,
			   statStruct&, stat,
		           bool, linRelax,
				ModelMinSeparator&, momo){
	clock_t t;
	if (!violatedConst.empty()){
		while (!violatedConst.empty()){
		  add(violatedConst.front().ineq,IloCplex::UseCutPurge);
		  stat.nbCuts[violatedConst.front().type]++;
		  // cout << "Adding " << violatedConst.front().type << " constraint" << endl;
		  violatedConst.pop_front();
		}
	  } 
	else {

		/* Retrieving x values */
		for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
		  (*itEdge)->_valx = getValue((*itEdge)->_x);
		
		}

		

// w1w2 inequalities
		t = clock();

		list<IloRange> l;
		gloutGlout(G,getEnv(), l);
		t = clock()-t;
		stat.temps[GLOUTON] +=t;

		if(l.size()==0){

			t = clock();
		    l = plMinSeparatorRandom(G,getEnv());
		
			//list<IloRange> l = plMinSeparator(G,getEnv(),momo);
			t = clock()-t;
			stat.temps[W1W2] +=t;	
		
			while(!l.empty()){
				add(l.front(),IloCplex::UseCutPurge);
				l.pop_front();
				stat.nbCuts[W1W2]++;
		
			}


		}
		else{
			while(!l.empty()){
				add(l.front(),IloCplex::UseCutPurge);
				l.pop_front();
				stat.nbCuts[GLOUTON]++;
			}



		}






		/* Check Cycle inequalities */
		t = clock();
		cycleSep(G,Gp,getEnv(),violatedConst);
		t = clock()-t;
		stat.temps[CYCLE]+= t;



		



		






		 while (!violatedConst.empty()){
			  // cout << "Adding constraint : " << violatedConst.front().ineq << endl;
			  add(violatedConst.front().ineq,IloCplex::UseCutPurge);
			  stat.nbCuts[violatedConst.front().type]++;
			  violatedConst.pop_front();
		}


	}


}





void PrintSolution(IloCplex cplex,
		   Graph & G,
		   statStruct & stat){
  cout << "Solution Status : " << cplex.getStatus() << endl;
  cout << "Solution Value : " << cplex.getObjValue() << endl;
  cout << endl;
  cout << "Number of generated cycle inequalities : " << stat.nbCuts[0] << endl;
  cout << "Number of generated lacroix inequalities : " << stat.nbCuts[1] << endl;
  cout << endl;
  for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
    if ((*itEdge)->_valx >= EPS) {
      cout << (*itEdge)->_x.getName() << " = " << (*itEdge)->_valx << endl;
    }
  }
  for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
    if ((*itEdge)->_valy >= EPS) {
      cout << (*itEdge)->_y.getName() << " = " << (*itEdge)->_valy << endl;
    }
  }

}





int main(int argc, char** argv){
  
  IloEnv env;
     
  try{
    IloModel model(env);

    if (argc < 2) {
      cout << "Error : the command should be followed by a filename" << endl;
      exit(1);
    }
	    
    statStruct stat;
    stat.init();
    char * bl = argv[1];
    Graph G(bl);
    Graph Gp(2*G._nbNodes);
    list<poolStruct> violatedConst;
    char * w1w2 = argv[2]; 
    createGp(G,Gp);

    StateModel(model, G);

    IloCplex cplex(env);
    cplex.setParam(IloCplex::Param::TimeLimit, 3600);
    //cplex.use(CheckFeas(env,G,Gp,violatedConst,stat));

   stat.start = clock();
	ModelMinSeparator momo(G);

	if( strcmp(w1w2,"0")==0){     // lazy PL, User PL
		cplex.use(GenLazyCut(env,G,Gp,violatedConst,stat,true,momo));
	   
	    
	    cplex.use(GenUserCut(env,G,Gp,violatedConst,stat,true,momo));
		stat.formulation = "w1w2";

	}else if( strcmp(w1w2,"0")==1){  // lazy BFS, User Glout
		stat.formulation = "w1w2 GLOUT";
		cplex.use(SimpleLazyCut(env,G,Gp,violatedConst,stat,true,momo));
	   
	    
	    cplex.use(GloutUserCut(env,G,Gp,violatedConst,stat,true,momo));

	}else{  //lazy BFS, User Glout puis PL si glout ne génère rien

		stat.formulation = "w1w2 GLOUT+PL";
		cplex.use(SimpleLazyCut(env,G,Gp,violatedConst,stat,true,momo));
	   
	    
	    cplex.use(GloutExactUserCut(env,G,Gp,violatedConst,stat,true,momo));

	}

    cplex.extract(model);

    char* file = new char[15];
    strcpy(file,"test.lp");
    cplex.exportModel(file);
    delete[] file;


    cplex.setParam(IloCplex::Cliques,-1);
    cplex.setParam(IloCplex::Covers,-1);
    cplex.setParam(IloCplex::DisjCuts,-1);
    cplex.setParam(IloCplex::FlowCovers,-1);
    cplex.setParam(IloCplex::FlowPaths,-1);
    cplex.setParam(IloCplex::FracCuts,-1);
    cplex.setParam(IloCplex::GUBCovers,-1);
    cplex.setParam(IloCplex::ImplBd,-1);
    cplex.setParam(IloCplex::MIRCuts,-1);
    cplex.setParam(IloCplex::ZeroHalfCuts,-1);
    cplex.setParam(IloCplex::MCFCuts,-1);
    cplex.setParam(IloCplex::MIPInterval,1);
    cplex.setParam(IloCplex::HeurFreq,-1);
    cplex.setParam(IloCplex::ClockType,1);
    cplex.setParam(IloCplex::RINSHeur,-1);

    if ((argc >= 3) && (argv[2][0] == '0')){
      cplex.setParam(IloCplex::NodeLim,1);
    }

    //     cplex.setParam(IloCplex::BndStrenInd,0);
    //     cplex.setParam(IloCplex::BtTol,0.0);
    //     cplex.setParam(IloCplex::BndStrenInd,0);

    //     TimeLimit = (5.0 * (double)(ceil ((double)NbNodes / 100.0))) * 3600.0;
    //     cout << "Time Limit of " << TimeLimit / 3600.0 << " hours" << endl;
    //     cplex.setParam(IloCplex::TiLim,172800.0);



    cplex.solve();
    
    for (list<Edge*>::iterator itEdge = G._edges.begin(); itEdge != G._edges.end(); itEdge++){
      (*itEdge)->_valx = cplex.getValue((*itEdge)->_x);
	if((*itEdge)->_valx > epsilon){
		cout<<(*itEdge)->_first << "   " << (*itEdge)->_last << endl;
	}
	
    }
	
    env.out() << "Solution value  = " << cplex.getObjValue() << endl;
    //G.Write_sol_ps_pdf();

	stat.nbNodes = cplex.getNnodes();
	stat.optimalityGap = cplex.getMIPRelativeGap();
	stat.end = clock();
	stat.computeTime();
	stat.name = bl;
	stat.nbCst= 0;
	stat.nbNoeudInstance = G._nbNodes;
	stat.nbAreteInstance  = G._edges.size();
	stat.printInfo();
	stat.writeFile(fResultName);
    
    cplex.end();

	//récupération des données
	
	
	
  }catch(IloException& e) {
    cout << "ERROR: " <<endl;
  }

  env.end();
	
  return 0;
}

