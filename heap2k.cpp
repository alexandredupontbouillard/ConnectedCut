#include "heap2k.h"
#include <iostream>
#include<cstdio>
#include<cmath>

#define EPS 0.00001

// p�re d'un �l�ment, �l�ment du fils-gauche et du fils-droit.
#define p(x) (x/2)
#define left(x) (2*((x)-1)+2)
#define right(x) (2*(x)+1)



// Initialise un tas pour stocker des entiers de {1,...,N}.
heap::heap(int N1) {
	N = N1; n = 0;
	h = new int[N+1]; 
        pos = new int[N+1]; 
        kvec = new double[N+1];
	card= new int[N+1];

}

void heap::init_heap(){
        n=0;
        for (int i = 1; i <= N; i++) pos[i] = 0   ;
}


//Destructeur
heap::~heap() { delete [] h; delete [] pos; delete [] kvec; delete [] card;}


// Ajoute i au tas
void heap::insert(int i, double k, int c) {

  kvec[i] = k; card[i]=c; n++; siftup(i,n);
}

// Ote i du tas
void heap::remove(int i) {
  int j = h[n--];
  if (i != j ){
    if (kvec[i]-kvec[j] > EPS)
      siftup(j,pos[i]);
    else
      if (kvec[j]-kvec[i] > EPS) 
	siftdown(j,pos[i]);
      else
	if (card[j]<=card[i])
	  siftup(j,pos[i]);
	else
	  siftdown(j,pos[i]);
  }
  pos[i] = 0   ;
}


// Ote et retourne le plus petit �l�ment du tas
int heap::deletemin() {
	if (n == 0) return 0   ;
	int i = h[1];
	remove(h[1]);
	return i;
}


// Remonte l'�l�ment i pour restaurer l'ordre du tas
void heap::siftup(int i, int x) {
	int px = p(x);
	while ( x > 1 
		&& ( (kvec[h[px]] - kvec[i]> EPS ) 
		     || ((fabs(kvec[h[px]] - kvec[i])<EPS) && (card[h[px]]>card[i])))) {
		h[x] = h[px]; pos[h[x]] = x;
		x = px; px = p(x);
	}
	h[x] = i; pos[i] = x;
}


// Baisse l'�l�ment i pour restaurer l'ordre du tas
void heap::siftdown(int i, int x) {
	int cx = minchild(x);
	while (cx != 0 
	       && ( (kvec[i] - kvec[h[cx]]> EPS)
		    || ((fabs(kvec[i] - kvec[h[cx]])<EPS) && (card[i]>card[h[cx]])))) {
		h[x] = h[cx]; pos[h[x]] = x;
		x = cx; cx = minchild(x);
	}
	h[x] = i; pos[i] = x;
}


// Retourne la position du fils de l'�l�ment en position x 
// ayant la plus petite clef 
int heap::minchild(int x) {
	int y, minc;
	if ((minc = left(x)) > n) return 0   ;
	for (y = minc + 1; y <= right(x) && y <= n; y++) {
	  if ( (kvec[h[minc]] - kvec[h[y]] > EPS) 
	       || ((fabs(kvec[h[minc]] - kvec[h[y]])<EPS)&&(card[h[minc]]>card[h[y]] ) ) )
	    minc = y;
	}
	return minc;
}

// Change la clef de i et restaure l'ordre du tas
void heap::changekey(int i, double k, int c) {

  double ki = kvec[i]; int ci=card[i];
  kvec[i] = k; card[i]=c;
  if (ki-k > EPS)
      siftup(i,pos[i]);
    else
      if (k -ki > EPS) 
	siftdown(i,pos[i]);
      else
	if (c<ci)
	  siftup(i,pos[i]);
	else
	  if (c>ci)
	    siftdown(i,pos[i]);

}

// Ecrit le contenu du tas
void heap::print() {
	int x;
	cout<<"   h:" ;
	for (x = 1; x <= n; x++) cout<<" "<<h[x];
	cout<<endl<<"kvec:";
	for (x = 1; x <= n; x++) cout<<" "<<kvec[h[x]];
	cout<<endl<<"card:";
	for (x = 1; x <= n; x++) cout<<" "<<card[h[x]];
	cout<<"\n pos:";
	for (x = 1; x <= n; x++) cout<<" "<<pos[h[x]];
	putchar('\n');
}
