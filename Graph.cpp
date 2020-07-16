#include<fstream>
#include<sstream>
#include"Graph.h"
#include<cstdlib>
#include<cmath>
#include <string>
#include <iostream>
using namespace::std;

//////////////////////////////
//////// EDGE ////////////////
//////////////////////////////


Edge::Edge():_id(-1),_first(-1),_last(-1){}

Edge::Edge(const int & id,
	   const int & first,
	   const int & last,
	   const double & cost):_id(id),
				_first(first),
				_last(last),
				_cost(cost){}

Edge::~Edge(){}

void Edge::Print(){
  cout << "Edge "<<_id << " "<< _first<< " "<<_last<<" "<<_cost<< endl;
}

#ifdef USING_CPLEX
void Edge::MakeCplexVar(const int & nbnodes,
			IloEnv & env){
  char name[15];
  sprintf(name,"x_%d_%d",_first,_last);
  _x = IloNumVar(env, 0.0, 1.0, IloNumVar::Int,name);
  sprintf(name,"y_%d_%d",_first,_last);
  _y = IloNumVar(env, 0.0, 1.0, IloNumVar::Int,name);
  _z = IloNumVarArray(env,nbnodes,0.0,1.0,IloNumVar::Float);
  _zrev = IloNumVarArray(env,nbnodes,0.0,1.0,IloNumVar::Float);
  for (int i = 0; i < nbnodes; i++){
    sprintf(name,"z_%d_%d_%d",_first,_last,i);
    _z[i].setName(name);
    sprintf(name,"z_%d_%d_%d",_last,_first,i);
   _zrev[i].setName(name);
  }
}
void Edge::MakeCplexVarW1W2(IloEnv & env){

	char name[15];
	sprintf(name,"x_%d_%d",_first,_last);
	_x = IloNumVar(env, 0.0, 1.0, IloNumVar::Int,name);
}
void Edge::MakeCplexVarflot(IloEnv env,int nbNodes){
	char name[15];
	sprintf(name,"x_%d_%d",_first,_last);
	_x = IloNumVar(env, 0.0, 1.0, IloNumVar::Int,name);
	flotFL.resize(nbNodes);
	flotLF.resize(nbNodes);
	for(int i = 0 ; i < nbNodes ; i ++){
		flotFL[i].resize(i);
		flotLF[i].resize(i);
		for(int j = 0; j < i; j ++){
			char name[30];
			char name1[30];
			sprintf(name,"flotLF_%d_%d_%d_%d",i,j,_first,_last);
			sprintf(name,"flotFL_%d_%d_%d_%d",i,j,_first,_last);
			flotLF[i][j] = IloNumVar(env, 0.0, 1.0, IloNumVar :: Float, name);

			flotFL[i][j] = IloNumVar(env, 0.0, 1.0, IloNumVar :: Float, name);
		}
	}
}

#endif

//////////////////////////////
///////// NODE ///////////////
//////////////////////////////



Node::Node():_id(-1),_coordx(-1),_coordy(-1){
  _incList.clear();
}

Node::Node(const int & id, const int & nbnodes, const double & coordx, const double & coordy):_id(id),_coordx(coordx),_coordy(coordy){
  _incVec = vector<Edge*>(nbnodes,NULL);
  _incList.clear();
  visited = false;
}

Node::~Node(){
}

void Node::addEdge(Edge * e, const int & adjnode){
  _incVec[adjnode] = e;
  _incList.push_back(make_pair(adjnode,e));
}

Edge* & Node::operator[](const int & i){
  return _incVec[i];
}
void Node::MakeCplexVarflot(IloEnv env){
	char name[15];
	sprintf(name,"s_%d",_id);
	_s = IloNumVar(env, 0.0, 1.0, IloNumVar::Int,name);
}



//////////////////////////////
/////// GRAPH ///////////////
//////////////////////////////


Graph::Graph(const int & nbnodes):_nbNodes(nbnodes){
  _nodes = vector<Node>(_nbNodes);
  for (int i = 0; i < _nbNodes; i++){
    _nodes[i]= Node(i,_nbNodes,-1,-1);
  }
}

Graph::Graph(const char* filename){
  int i,nbedge,k,l;
  double x,y;
  char c;

  _instance_name=filename;

  ifstream fic(filename);
  
  fic>>_nbNodes;
	
  if(_nbNodes !=0){
	  fic>>nbedge;

	  _nodes = vector<Node>(_nbNodes);
	  for (i = 0; i < _nbNodes; i++){
	    fic>>c;fic>>k;
	    fic>>x;fic>>y;
	    _nodes[i]= Node(i,_nbNodes,x,y);
	  }

	  for (i=0;i<nbedge;i++){
	    fic>>c;fic>>k;fic>>l;fic>>x;
	    AddEdge(i,k,l,x);
	  }
	
  }
  else{
	
	string  first;
	string second;

        
	fic>>first;
	
	fic>>second;
	bool b = true;
	cout<<first<<endl;
	cout<<second<<endl;
	cout<<"yo"<<endl;
	
	cout<<"la ça va"<<endl;
	/*
	while(not ( ( first== "p" and second=="edge") or (first=="edge" and second=="p") )){
		cout<<"first : "<<first<<endl;
		cout<<"second : " << second<<endl;
		if(b){
			b = false;
			fic>>first;
		}
		else{
			b = true;
			fic>>second;
		}

	}
	cout<<"la ça va"<<endl;
	fic>>_nbNodes;
	for (i = 0; i < _nbNodes; i++){
	    _nodes[i]= Node(i,_nbNodes,0,0);
	  }
	
	int nbEdge;
	fic>>nbEdge;
	int f;
	int l;
	for (i = 0; i < nbEdge; i++){
	    fic>>c;
	    fic>>f;
	    fic>>l;
	    AddEdge(i, f,l,1);
	  }
	*/


	

  }
  fic.close();

}


Graph::~Graph(){
  for (list<Edge*>::iterator itEdge = _edges.begin(); itEdge != _edges.end(); itEdge++){
    delete *itEdge;
  }
}

void Graph::AddEdge(const int & id,
		    const int & first,
		    const int & last,
		    const double & cost){
  Edge * newEdge = new Edge(id,first,last,cost);
  _edges.push_back(newEdge);
  _nodes[first].addEdge(newEdge,last);
  _nodes[last].addEdge(newEdge,first);
}

void Graph::Print(){
  int i,j;
  list<Edge*>::const_iterator it;

  cout << "Printing graph" << endl;
  cout << "Nb Nodes :" << _nbNodes << endl;

  for (i=0;i<_nbNodes;i++){
     cout <<"Node "<<_nodes[i]._id<<" ("<<_nodes[i]._coordx<<","<<_nodes[i]._coordy<<") :";
     for (j=0;j<_nbNodes;j++){
       if (_nodes[i]._incVec[j]!=NULL)
	 if (i!=_nodes[i]._incVec[j]->_first) cout<<_nodes[i]._incVec[j]->_first;
	                                else cout<<_nodes[i]._incVec[j]->_last;
       
     }
     cout<<endl;
  }

  cout << "Edges :"<<endl;
 
  for (it=_edges.begin();it!=_edges.end();it++){
    (*it)->Print();
  }

}
vector<int> Graph::getBridges(){ // after this function, the graph gets corrupted, so you may copy the initial graph before using this
	list<pair<int,Edge*> >::iterator it;
	vector<int> explore;
	vector<int> result;
	const int & nb_noeud = _nodes.size();
	Node n(nb_noeud, nb_noeud+2,0, 0);
	Node n1(nb_noeud+1, nb_noeud+2,0, 0);
	_nodes.push_back(n);
	_nodes.push_back(n1);
	_nodes[0]._incVec.resize(nb_noeud+2);
	
	AddEdge(_edges.size(),0,nb_noeud,0);
		
	AddEdge(_edges.size(),0,nb_noeud+1,0);
	AddEdge(_edges.size(),nb_noeud,nb_noeud+1,0);
	
	explore.push_back(nb_noeud);
	int actual_node;
	int neighbour;
	initBFS();
	int indice=1;
	vector<int> order;
	vector<int> depth(_nodes.size());
	vector<int> low(_nodes.size());
	_nodes[explore[0]].visited = true;
	while(explore.size() > 0){
		actual_node = explore[0];
		depth[actual_node] = indice;
		indice++;
		order.push_back(actual_node);
		explore.erase(explore.begin()); 
		list<pair<int,Edge*> >::iterator it;
		for(it = _nodes[actual_node]._incList.begin(); it != _nodes[actual_node]._incList.end(); it++){
			neighbour = it->first;
			if(not _nodes[neighbour].visited){
				_nodes[neighbour].visited = true;
				
				explore.insert(explore.begin(),neighbour);	
				
					
			}		
		}
	}
	int min;
	
	for(int i = order.size()-1 ; i>=1; i=i-1){
		min = depth[order[i]];
		for(it = _nodes[order[i]]._incList.begin(); it != _nodes[order[i]]._incList.end(); it++){
			if(depth[it->first] < depth[order[i]]){
				if(min > depth[it->first]){
					min = depth[it->first];
					

				}							
			}else{
				if(min > low[it->first]){
					
					min = low[it->first];
					
				}		
			}
		}
		low[order[i]] = min;
		for(it = _nodes[order[i]]._incList.begin(); it != _nodes[order[i]]._incList.end(); it++){
			if(depth[it->first] > depth[order[i]]){
				if(low[order[i]] < low[it->first] ){
					result.push_back((*it->second)._id);
					
					
				}
			}
		}
	}

	return result;
}

bool Graph::connected(){ //return true if it's not a 3 cut
	vector<int> explore;
	explore.push_back(_nodes[0]._id);
	int actual_node;
	int neighbour;
	initBFS();
	while(explore.size() > 0){
		actual_node = explore[0];

		explore.erase(explore.begin()); 
		list<pair<int,Edge*> >::iterator it;
		for(it = _nodes[actual_node]._incList.begin(); it != _nodes[actual_node]._incList.end(); it++){
			neighbour = it->first;
			if(not _nodes[neighbour].visited){
				_nodes[neighbour].visited = true;
				explore.push_back(neighbour);	
				
					
			}		
		}
	}

	int part2;
	for(int i = 0 ; i < _nodes.size();i++){
		if(not	_nodes[i].visited){
			part2 = i;
			
			
		}
	}
	_nodes[part2].visited  =true;
	vector<int> explore2;
	explore2.push_back(part2);
	while(explore2.size() > 0){
		actual_node = explore2[0];

		explore2.erase(explore2.begin()); 
		list<pair<int,Edge*> >::iterator it;
		for(it = _nodes[actual_node]._incList.begin(); it != _nodes[actual_node]._incList.end(); it++){
			neighbour = it->first;
			if(not _nodes[neighbour].visited){
				_nodes[neighbour].visited = true;
				explore2.push_back(neighbour);	
				
					
			}		
		}
	}
	for(int i = 0 ; i < _nodes.size();i++){
		if(not	_nodes[i].visited){
			return false;
			
		}
	}
	return true;
	
}

// pour initialiser les parcours BFS permettant de tester la connexité
void Graph::initBFS(){
	
	vector<int> explore;
	explore.push_back(_nodes[0]._id);
	int actual_node;
	int neighbour;

	while(explore.size() > 0){
		actual_node = explore[0];

		explore.erase(explore.begin()); 
		list<pair<int,Edge*> >::iterator it;
		for(it = _nodes[actual_node]._incList.begin(); it != _nodes[actual_node]._incList.end(); it++){
			neighbour = it->first;
			if( _nodes[neighbour].visited){
				_nodes[neighbour].visited = false;
				explore.push_back(neighbour);	
						
			}		
		}
	}

}
/*
Graph Graph::copy(vector<int> & new_cost){ // return the graph necessary for the cut  constraint separation
	Graph result(_nodes.size()*2);
	list<Edge*>::iterator it;
	
	int i = 0;
	for(it = _edges.begin(); it != _edges.end(); it++){
		if(new_cost[i]>= epsilon){
			result.AddEdge(i,(*it)->_first,(*it)->_last,new_cost[i]);
		}
		i++;
		
	}

	return result;
	
}*/


void Graph::Write_ps_pdf(){
  int i;
  list<Edge*>::const_iterator it;
  double scx,scy;

  scx=565.0/1000;
  scy=565.0/1000;
  //  scy=812.0/maxy;

  ostringstream nomSortie1;
  nomSortie1.str("");
  nomSortie1<<_instance_name<<".ps";
  ofstream fic(nomSortie1.str().c_str());

  cout<<nomSortie1.str().c_str()<<endl;
  
  fic<<"/Arial-Bold findfont 9 scalefont setfont"<<endl;

  for (i=0;i<_nbNodes;i++){
    fic<<15+scx*_nodes[i]._coordx<<" "<<15+scy*_nodes[i]._coordy<<" 2.5 0 360 arc"<<endl;
    fic<<"fill"<<endl;
    fic<<"stroke"<<endl;
    fic<<20+scx*_nodes[i]._coordx<<" "<<15+scy*_nodes[i]._coordy<<" moveto ("<<i<<") show stroke"<<endl;
  }

  for (it=_edges.begin();it!=_edges.end();it++){
    fic<<15+scx*_nodes[(*it)->_first]._coordx<<" "<<15+scy*_nodes[(*it)->_first]._coordy<<" moveto"<<endl;
    fic<<15+scx*_nodes[(*it)->_last]._coordx<<" "<<15+scy*_nodes[(*it)->_last]._coordy<<" lineto"<<endl;
    fic<<"stroke"<<endl;
  }

    ostringstream nomSortie3;
    ostringstream nomSortie2;
    nomSortie2.str("");
    nomSortie2<<_instance_name<<".pdf";
    nomSortie3.str("");
    nomSortie3<<"ps2pdf "<<nomSortie1.str()<<" "<<nomSortie2.str();
    cout<<nomSortie3.str()<<endl;
    if (system(nomSortie3.str().c_str())==-1)
      cerr<<"Error with ps2pdf, perhaps you need to install this program or use the ps"<<endl;

}

void Graph::Write_sol_ps_pdf(){
  int i;
  list<Edge*>::const_iterator it;
  double scx,scy;

  scx=565.0/1000;
  scy=565.0/1000;
  //  scy=812.0/maxy;

  ostringstream nomSortie1;
  nomSortie1.str("");
  nomSortie1<<_instance_name<<".ps";
  ofstream fic(nomSortie1.str().c_str());

  cout<<nomSortie1.str().c_str()<<endl;
  

  // EDGE set to 1
  for (it=_edges.begin();it!=_edges.end();it++){
    if ((*it)->_valx>=1-EPS){
      fic<<15+scx*_nodes[(*it)->_first]._coordx<<" "<<15+scy*_nodes[(*it)->_first]._coordy<<" moveto"<<endl;
      fic<<15+scx*_nodes[(*it)->_last]._coordx<<" "<<15+scy*_nodes[(*it)->_last]._coordy<<" lineto"<<endl;
      fic<<"stroke"<<endl;
    }
  }


  // Edge set to 0
   fic<<"0.75 0.75 0.75 setrgbcolor"<<endl;
   for (it=_edges.begin();it!=_edges.end();it++){
 
     if ((*it)->_valx<=EPS){
       fic<<15+scx*_nodes[(*it)->_first]._coordx<<" "<<15+scy*_nodes[(*it)->_first]._coordy<<" moveto"<<endl;
       fic<<15+scx*_nodes[(*it)->_last]._coordx<<" "<<15+scy*_nodes[(*it)->_last]._coordy<<" lineto"<<endl;
       fic<<"stroke"<<endl;
     }    
   }


  // Fractionnal edge

  fic<<"0 0 0 setrgbcolor"<<endl;
  fic<<"[3 3] 0 setdash ";  //Pour produire des dashed line "[3 3] 0 setdash" fait que la 
                                //ligne suivante est dashed avec 3 units de tiret puis 3 unit de blanc etc. Le 0 est l'offset du premier tiret
  for (it=_edges.begin();it!=_edges.end();it++){

    if ( ((*it)->_valx>EPS) && ((*it)->_valx<1-EPS) ){
      fic<<15+scx*_nodes[(*it)->_first]._coordx<<" "<<15+scy*_nodes[(*it)->_first]._coordy<<" moveto"<<endl;
      fic<<15+scx*_nodes[(*it)->_last]._coordx<<" "<<15+scy*_nodes[(*it)->_last]._coordy<<" lineto"<<endl;
      fic<<"stroke"<<endl;
    }
  
  }


  // NODES
  for (i=0;i<_nbNodes;i++){
    fic<<15+scx*_nodes[i]._coordx<<" "<<15+scy*_nodes[i]._coordy<<" 2.5 0 360 arc"<<endl;
    fic<<"fill"<<endl;
    fic<<"stroke"<<endl;
  }

  ostringstream nomSortie3;
  ostringstream nomSortie2;
  nomSortie2.str("");
  nomSortie2<<_instance_name<<"_sol.pdf";
  nomSortie3.str("");
  nomSortie3<<"ps2pdf "<<nomSortie1.str()<<" "<<nomSortie2.str();
  cout<<nomSortie3.str()<<endl;
  if (system(nomSortie3.str().c_str())==-1)
    cerr<<"Error with ps2pdf, perhaps you need to install this program or use the ps"<<endl;


}


// H (un tableau quoi) T card and D must be allocated before (with that there is no use to reallocate
// every time you use dijkstra
// t can be -1 if you want the whole spanning shortest arborescence from s
// retourne la distance de s à t ou -1 s'il n'y a pas de t
// T contient l'indice du pere ou -1 si pas de pere
double Graph::Dijkstra(int s, int t, vector<int>&T, vector<double>&D, vector<int>&card, heap &H){

  int i, u,v;
  int stop;
  list<pair<int,Edge*> >::const_iterator itv;
  H.init_heap();

  for (i=0;i<_nbNodes;i++){
    T[i]=-1;
  }

  T[s]=-2;
  D[s]=0;
  card[s]=0;
  H.insert(s+1,0,0);

  stop=0;
  while ((!stop) && (!H.empty()) ){
 
    u=H.deletemin()-1;
 
    if (u==t) {
      stop=1;
    }
    else{

      itv=_nodes[u]._incList.begin();
      while ( (!stop) && (itv!=_nodes[u]._incList.end()) ){

	v=(*itv).first; 
 
	if (T[v]==-1){
	 
	  D[v]=D[u]+(*itv).second->_cost;
	  card[v]=card[u]+1;
	  T[v]=u;
	  H.insert(v+1,D[v],card[v]);
     
	}
	else{

	  if (D[v]-(D[u]+(*itv).second->_cost)>EPS){
 
	    D[v]=D[u]+(*itv).second->_cost;
	    card[v]=card[u]+1;
	    T[v]=u;
	    H.changekey(v+1,D[v],card[v]);

	  }
	  else{

	    if ( (fabs(D[v]-(D[u]+(*itv).second->_cost))<EPS)
		  && (card[v]>card[u]+1) ){
	      D[v]=D[u]+(*itv).second->_cost;
	      card[v]=card[u]+1;
	      T[v]=u;
	      H.changekey(v+1,D[v],card[v]);

	    }
	  }
	}
      
	itv++;	
      
      }
    }

  }
  if (t!=-1)
    return D[t];
  else
    return -1;

}


#ifdef USING_CPLEX
void Graph::MakeCplexVar(IloEnv & env){
  for (list<Edge*>::iterator itEdge = _edges.begin(); itEdge != _edges.end(); itEdge++){
    (*itEdge)->MakeCplexVar(_nbNodes,env);
  }
}

void Graph::MakeCplexVarW1W2(IloEnv & env){
  for (list<Edge*>::iterator itEdge = _edges.begin(); itEdge != _edges.end(); itEdge++){
    (*itEdge)->MakeCplexVarW1W2(env);
  }
}
void Graph::MakeCplexVarFlot(IloEnv & env){
	for (list<Edge*>::iterator itEdge = _edges.begin(); itEdge != _edges.end(); itEdge++){
    (*itEdge)->MakeCplexVarflot(env,_nbNodes);
  }
  	for(int i = 0 ; i < _nodes.size() ; i++){
		_nodes[i].MakeCplexVarflot(env);
	}
}
#endif
