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
#endif

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

  void Print();

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

  void Print();
  void Write_ps_pdf();
  void Write_sol_ps_pdf();

  double Dijkstra(int s, int t, vector<int>&T, vector<double>&D, vector<int>&card, heap &H);


#ifdef USING_CPLEX
  void MakeCplexVar(IloEnv & env);
#endif


};

#endif
