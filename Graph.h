#ifndef GRAPH_H
#define GRAPH_H

#include<iostream>
#include<list>
#include<vector>
#include<string>
#include"heap2k.h"


#ifdef USING_CPLEX
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
#endif

using namespace std;

#define EPS 0.00001

//////////////////////////////
//////// EDGE ////////////////
//////////////////////////////


class Edge{
 public:
  int _id;
  int _first; /* allways the smallest index of endnodes */
  int _last;
  double _cost;
  double _valx;
  double _valy;

#ifdef USING_CPLEX
  IloNumVar _x;
  IloNumVar _y;
  IloNumVarArray _z;
  IloNumVarArray _zrev;
  vector<vector<IloNumVar>> flotFL;
  vector<vector<IloNumVar>> flotLF; 
#endif
	
  //pour la séparation gloutonne
  bool aCheval; // vrai si l'arête est adjacente à un Wi

  Edge();
  Edge(const int & id,
       const int & first,
       const int & last,
       const double & cost);
  ~Edge();

  void Print();



#ifdef USING_CPLEX
  void MakeCplexVar(const int & nbnodes,
		    IloEnv & env);
  void MakeCplexVarW1W2(IloEnv & env);
  void MakeCplexVarflot(IloEnv env,int nbNodes);
#endif

};


//////////////////////////////
/////// VERTEX ///////////////
//////////////////////////////


class Node {
 public:
  vector<Edge*> _incVec;
  list<pair<int,Edge*> > _incList;
  int _id;
  double _coordx;
  double _coordy;

  Node();
  Node(const int & id, const int & nbnodes, const double & coordx, const double & coordy);
  ~Node();



  void addEdge(Edge * e, const int & adjnode);
  Edge* & operator[](const int & i);
  bool visited;
  bool visitedCycle;
  void Print();



  // pour la séparation gloutonne

  bool adjW1;
  bool adjW2;
  double val();

  #ifdef USING_CPLEX
  IloNumVar _s;
  #endif

  #ifdef USING_CPLEX
  
  void MakeCplexVarflot(IloEnv env);
  #endif

};


//////////////////////////////
//////// GRAPH ///////////////
//////////////////////////////


class Graph{
 public:
  string _instance_name;
  vector<Node> _nodes;
  list<Edge*> _edges;
  int _nbNodes;

  Graph(const int & nbnodes);
  Graph(const char*);
  ~Graph();

  void AddEdge(const int & id,
	       const int & first,
	       const int & last,
	       const double & cost);
  bool is2connected();
  void Print();
  void Write_ps_pdf();
  void Write_sol_ps_pdf();
  vector<int> getBridges();
  bool connected();
  void initBFS();
  //Graph copy(vector<int> new_cost)();

  double Dijkstra(int s, int t, vector<int>&T, vector<double>&D, vector<int>&card, heap &H);


  void gloutInit();


#ifdef USING_CPLEX
  void MakeCplexVar(IloEnv & env);
  void MakeCplexVarW1W2(IloEnv & env);
  void MakeCplexVarFlot(IloEnv & env);
#endif


};

#endif
