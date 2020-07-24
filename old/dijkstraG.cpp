#include "bipmaster.h"
#include "dijkstraG.h"

#define VERY_FAR  1073741823

/*******************   definitions for heap  *****************/

heap d_heap;

long h_current_pos,
     h_new_pos,
     h_pos,
     h_last_pos;

node *node_j,
     *node_k;

double dist_k,
       dist_min;

long   card_k,
       card_min;

int  h_degree;

#define HEAP_POWER   2
#define NILL        -1
#define NODE_IN_HEAP( node_i ) ( node_i -> heap_pos != NILL )
#define NONEMPTY_HEAP  ( d_heap.size > 0 )

#define PUT_TO_POS_IN_HEAP( node_i, pos )\
{\
d_heap.nd[pos]        = node_i;\
node_i -> heap_pos = pos;\
}

void C_bipmaster::Init_heap ( long  n, node* source ) {
h_degree = 1 << HEAP_POWER;
d_heap.size = 1;
d_heap.nd = new (node*)[n+1];
PUT_TO_POS_IN_HEAP( source, 0 )
}

void C_bipmaster::Heap_decrease_key ( node* node_i, double dist_i , long card_i) {
for ( h_current_pos =  node_i -> heap_pos;
      h_current_pos > 0;
      h_current_pos = h_new_pos
    )
      {
        h_new_pos = ( h_current_pos - 1 ) >> HEAP_POWER;

        node_j = d_heap.nd[h_new_pos];

        if ( dist_i  >  node_j -> dist )
 	   break;

        if ( ( dist_i == node_j -> dist ) && (card_i  >=  node_j -> card ) )
           break;

        PUT_TO_POS_IN_HEAP ( node_j, h_current_pos )
      }

PUT_TO_POS_IN_HEAP ( node_i, h_current_pos )
}

void C_bipmaster::Insert_to_heap ( node* node_i ) {
PUT_TO_POS_IN_HEAP ( node_i, d_heap.size )
d_heap.size ++;
Heap_decrease_key( node_i, node_i -> dist, node_i -> card);
}

node* C_bipmaster::Extract_min (  ) 

{
node* node_0;

node_0             = d_heap.nd[0];
node_0 -> heap_pos = NILL;

d_heap.size -- ;

if ( d_heap.size > 0 )
  {
     node_k =  d_heap.nd [ d_heap.size ];
     dist_k =  node_k -> dist;
     card_k =  node_k -> card;

     h_current_pos = 0;

     while ( 1 )
       {
         h_new_pos = ( h_current_pos << HEAP_POWER ) +  1;
         if ( h_new_pos >= d_heap.size ) break;

         dist_min  = d_heap.nd[h_new_pos] -> dist;
         card_min  = d_heap.nd[h_new_pos] -> card;

         h_last_pos  = h_new_pos + h_degree;
	 if ( h_last_pos > d_heap.size ) h_last_pos = d_heap.size;

         for ( h_pos = h_new_pos + 1; h_pos < h_last_pos; h_pos ++ )
            {
 	      if ( d_heap.nd[h_pos] -> dist < dist_min )
		{
		  h_new_pos = h_pos;
		  dist_min  = d_heap.nd[h_pos] -> dist;
                  card_min  = d_heap.nd[h_pos] -> card;
		}
              else 
                {
                  if ( ( d_heap.nd[h_pos] -> dist == dist_min ) && ( d_heap.nd[h_pos] -> card < card_min ) )
		    {
		       h_new_pos = h_pos;
                       card_min  = d_heap.nd[h_pos] -> card;
		    }
		}
	    }

         if ( dist_k < dist_min ) 
             break;
       
	 if ( ( dist_k == dist_min ) && ( card_k <= card_min ) )
	     break;

         PUT_TO_POS_IN_HEAP ( d_heap.nd[h_new_pos], h_current_pos )

         h_current_pos = h_new_pos;
       }

    PUT_TO_POS_IN_HEAP ( node_k, h_current_pos )
  }
return node_0;
}

/**************   end of heap functions   ****************/

int C_bipmaster::dikh (long src, long som_rech, bool sucre, list<int> &L_vire) {

double dist_new,
       dist_from,
       best,
       comp;

long   card_new,
       card_from,
       sommet_atteint, 
       trad,
       besti,
       bestcard,
       compcard;

node *node_from,
     *node_to,
     *node_last,
     *i,
     *source,
     *tradnd,
     *tradnd2;

arc  *arc_ij,
     *arc_last;

bool fin;

int  sortie,k;
vector<int> marque;

/* initialization */

marque.resize(G.nbsom+1);
for(k=1;k<=G.nbsom;k++)
  marque[k]=0;

source=nodesdij-node_min + src;

node_last = nodesdij + ndij ;
 
for ( i = nodesdij; i != node_last; i ++ )
   { 
      i -> parent   = (node*) NULL;
      i -> dist     = VERY_FAR;
      i -> card     = VERY_FAR;
      i -> heap_pos = NILL;
   }

source -> parent = source;
source -> dist   = 0;
source -> card   = 0;

Init_heap (  ndij, source );

/* main loop */

best=VERY_FAR;
besti=-1; 
bestcard=G.nbsom*2;

fin=false;
while (( NONEMPTY_HEAP ) && (!fin) )
{

 node_from = Extract_min (  );
 sommet_atteint=(int) (node_from-nodesdij+node_min);

 if (((node_from->dist)<infiniDijkstra()) && (sommet_atteint!= som_rech) ) {

  trad=sommet_atteint;
  if (trad>G.nbsom) trad=trad-G.nbsom;

  (marque[trad])++;
  if (marque[trad]==2){
     tradnd=nodesdij-node_min + trad;
     tradnd2=nodesdij-node_min + trad+G.nbsom;
     comp=tradnd->dist+tradnd2->dist;
     compcard=tradnd->card+tradnd2->card;
//     cout<<"iii "<<trad<<"  "<<compcard<<endl;
     if ( ((best-comp)>machineEps())
	  || ((fabs(comp-best)<machineEps())&&(bestcard>compcard)) ){
       best=comp;
       besti=trad;
       bestcard=compcard;
     }
  }
  else{

   if (sucre) L_vire.push_back(trad);

   arc_last = ( node_from + 1 ) -> first;
   dist_from = node_from -> dist;
   card_from = node_from -> card;
   
   
   for ( arc_ij = node_from -> first; arc_ij != arc_last; arc_ij ++ )
     {
      node_to  = arc_ij -> head;

       dist_new = dist_from  + ( arc_ij -> len );
       card_new = card_from + 1;
       
       if ( ( node_to -> dist - dist_new ) > machineEps() )               //( dist_new <  node_to -> dist )
	   { node_to -> dist   = dist_new;
	     node_to -> card   = card_new;
             node_to -> parent = node_from;
	     if ( ! NODE_IN_HEAP ( node_to ) )
         	       Insert_to_heap ( node_to );
	     Heap_decrease_key (  node_to, dist_new , card_new );
	   }
       else 
           { 
	     if ( (fabs( node_to -> dist - dist_new ) <= machineEps() ) && ( card_new <  node_to -> card ) ) 
                 { node_to -> card   = card_new;
                   node_to -> parent = node_from;
	           if ( ! NODE_IN_HEAP ( node_to ) )
         	             Insert_to_heap ( node_to );
	           Heap_decrease_key (  node_to, dist_new , card_new);
		 }
	   }     

     }
  }
 }
 else
   fin=true;
}

if  (sommet_atteint!= som_rech)
   sortie=besti;
 else {
   if ( ((node_from->dist-best)>machineEps())
       || ((fabs(best-node_from->dist)<machineEps())&&(node_from->card>bestcard)) )
     sortie=besti;
   else
     sortie=-1;
}

delete[] d_heap.nd ;

return(sortie);

}


// En sortie un chemin ijk'...lj'  sans i' a la fin
bool C_bipmaster::cree_chemin(int src,int som_rech,int recol,list<int> &l) {
  node *ptr;
  int som;
  bool sortie;

  sortie=true;

  if (recol==-1) {

    ptr=nodesdij+som_rech-node_min;

    if (ptr->dist>infiniDijkstra()) {
      sortie=false;
    }
    else{
        som=((int)(ptr-nodesdij+node_min));
         while (som != src){
	  l.push_back(som);
  	  ptr=ptr->parent;
	  som=((int)(ptr-nodesdij+node_min));
        }
    }
  }
  else{

    ptr=nodesdij+recol-node_min;
    som=((int)(ptr-nodesdij+node_min));
    while ( som != src){
       l.push_front(som);
       ptr=ptr->parent;
       som=((int)(ptr-nodesdij+node_min));
    }

    l.push_front(src);
    
    ptr=nodesdij+recol+G.nbsom-node_min;
    ptr=ptr->parent;
    som=((int)(ptr-nodesdij+node_min));
    while ( som != src){
       l.push_back(som);
       ptr=ptr->parent;
       som=((int)(ptr-nodesdij+node_min));
    }
      
    
  }

  return(sortie);

}




void C_bipmaster::dikh_sav (long src, long som_rech) {

double dist_new,
       dist_from;

long   card_new,
       card_from,
       sommet_atteint;

node *node_from,
     *node_to,
     *node_last,
     *i,
     *source;


arc  *arc_ij,
     *arc_last;

bool fin;

/* initialization */

source=nodesdij- node_min + src;
//som_rech=src+G.nbsom;

node_last = nodesdij + ndij ;
 
for ( i = nodesdij; i != node_last; i ++ )
   { 
      i -> parent   = (node*) NULL;
      i -> dist     = VERY_FAR;
      i -> card     = VERY_FAR;
      i -> heap_pos = NILL;
   }

source -> parent = source;
source -> dist   = 0;
source -> card   = 0;

Init_heap (  ndij, source );

/* main loop */

fin=false;
while (( NONEMPTY_HEAP ) && (!fin) )
{
   node_from = Extract_min (  );
   sommet_atteint=(int) (node_from-nodesdij+node_min);

 if (((node_from->dist)<infiniDijkstra()) && (sommet_atteint!= som_rech) ) {

   arc_last = ( node_from + 1 ) -> first;
   dist_from = node_from -> dist;
   card_from = node_from -> card;
   
   
   for ( arc_ij = node_from -> first; arc_ij != arc_last; arc_ij ++ )
     {
       node_to  = arc_ij -> head;

       dist_new = dist_from  + ( arc_ij -> len );
       card_new = card_from + 1;
       

       if ( ( node_to -> dist - dist_new ) > machineEps() )               //( dist_new <  node_to -> dist )
	   { node_to -> dist   = dist_new;
	     node_to -> card   = card_new;
             node_to -> parent = node_from;
	     if ( ! NODE_IN_HEAP ( node_to ) )
         	       Insert_to_heap ( node_to );
	       Heap_decrease_key (  node_to, dist_new , card_new);
	   }
       else 
           { 
	     if ( (fabs( node_to -> dist - dist_new ) <= machineEps() ) && ( card_new <  node_to -> card ) ) 
                  {
	           node_to -> card   = card_new;
                   node_to -> parent = node_from;
	           if ( ! NODE_IN_HEAP ( node_to ) )
         	             Insert_to_heap ( node_to );
	             Heap_decrease_key (  node_to, dist_new , card_new);
		  }
	   }

       
     }

 }
 else
   fin=true;
}

delete[] d_heap.nd ;
}



bool C_bipmaster::cree_chemin_sav(int src,int som_rech,list<int> &l) {
  node *ptr;
  int som;

  ptr=nodesdij+som_rech-node_min;

  if ((ptr->dist)>=VERY_FAR) 
    return(false);
  else {
    som=((int)(ptr-nodesdij+node_min));
    while ( som != src){
      l.push_back(som);
      ptr=ptr->parent;
      som=((int)(ptr-nodesdij+node_min));
    }
    
    return(true);
  }
  
}


