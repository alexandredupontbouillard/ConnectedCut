#include<fstream>
#include<sstream>
#include"Graph.h"
#include<cstdlib>
#include<cmath>


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
#endif
