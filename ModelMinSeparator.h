#ifndef MODELMINSEPARATOR_H
#define MODELMINSEPARATOR_H

#include<iostream>
#include<list>
#include<vector>
#include<string>
#include"heap2k.h"
#include"Graph.h"

#ifdef USING_CPLEX
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
#endif



class ModelMinSeparator{
 public :
	 vector<IloRange> constrPos;
	 vector<IloRange> constrNeg;
	 IloCplex cplex;
	 list<IloNumVar> edges;
	 IloModel model;
	 IloEnv env1;



	ModelMinSeparator(Graph & G);
	
	 void add(const int  a,const int  b);
	 void deleta(const int a,const int b);
	 
	 
 
 


};




#endif
