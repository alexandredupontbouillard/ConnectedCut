#ifndef INFOS_H
#define INFOS_H

#include <ilcplex/ilocplex.h>
#include <string>
using namespace::std;

enum constType{ //enumerer les types de contraintes
  CYCLE,
  LACROIX,
  W1W2,  
  W1W2I,
  GLOUTON
};

struct statStruct{
  /* nb of generated cuts in the order of enum constType */
  int nbCuts[5];
  float temps[5];
  int nbNodes;
  clock_t start;
  string formulation;
  clock_t end;
  float totalTime;
  float optimalityGap;
  int nbCst;
  int nbAreteInstance;
  int nbNoeudInstance;
  float bestObj;
  float bestInteger;
  char * name;
  void init(){

	//initialiser chacun des compteurs de contraintes générées

    nbCuts[0] = 0; /* Cycle inequalities */
    nbCuts[1] = 0; /* Lacroix inequalities */
    nbCuts[2] = 0; /* W1W2 inequalities */
    nbCuts[3] = 0; /* W1W2I inequalities */
    nbCuts[4] = 0;

	//initialiser le temps associé à chaque contrainte
    temps[0]=0;
    temps[1]=0;
    temps[2]=0;
    temps[3]=0;
    temps[4] = 0;
  }
void computeTime();
void printInfo();
void writeFile(string s);
 
};

#endif	
