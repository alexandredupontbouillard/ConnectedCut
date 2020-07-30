#ifndef MINSEPARATOR_H
#define MINSEPARATOR_H


#include <lemon/dijkstra.h>
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "Graph.h"
#include "ModelMinSeparator.h"


#ifdef USING_CPLEX
#include <ilcplex/ilocplex.h>

#endif

using namespace::std;

void plMinSeparator(Graph & g, IloEnv env,ModelMinSeparator & momo,list<IloRange> & result);
list<IloRange> plMinSeparatorRandom(Graph & g, IloEnv env);
void gloutGlout(Graph & g, IloEnv env, list<IloRange> & violatedConst);

#endif
