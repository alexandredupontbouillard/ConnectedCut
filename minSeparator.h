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

list<IloRange> plMinSeparator(Graph & g, IloEnv env,ModelMinSeparator & momo);
list<IloRange> plMinSeparatorRandom(Graph & g, IloEnv env);
void gloutGlout(Graph & g, IloEnv env, list<IloRange> & violatedConst);

#endif
