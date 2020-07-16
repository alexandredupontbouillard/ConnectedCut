#ifndef ___HEAP___
#define ___HEAP___
#include<cstdlib>

// Fichier en-tête d'une structure de tas,.
// Gère un tas d'au plus N éléments (int) distincts 
//                                  pris parmis les nombres entiers de 1 à N,
//                              et  associés à une clef1 flottante (double) et une clef 2 entiere
//                                             telles qu'un element a est plus grand qu'un element a'
//                                             si cle1>cle1' ou si cle1==cle1' et cle2>cle2


#define epsprim 10E-4

using namespace std;

class heap {
	int	N;			// nombre max d'éléments dans le tas
	int	n;			// nombre d'éléments dans le tas
	int	*h;			// tableau des éléments du tas de h[1] à h[n]
	int	*pos;			// pos[i] donne la position d'un élément i dans h
	double	*kvec;			// kvec[i] est la clef 1 de l'élément i
	int     *card;                  // card[i] est la clef 2 de l'élément i

	int	minchild(int);		// retourne le plus petit fils d'un élément
	void	siftup(int,int);	// déplace un élément vers le haut 
                                        // pour restaurer l'ordre du tas
	void	siftdown(int,int);	// déplace un élément vers le bas 
                                        // pour restaurer l'ordre du tas

public:	
	heap(int);                      // constructeur d'un tas avec la taillemax du tas en paramètre

   	void init_heap();  //initialise un tas avec la taillemax du tas en param
	~heap();
	int	findmin();		// retourne l'élément de plus petite clef
	double	key1(int);		// retourne la clef 1 d'un élément
	int     key2(int);              // retourne la clef 2 d'un élément
	bool	member(int);		// retourne true si l'élément est dans le tas
	bool	empty();		// retourne true si le tas est vide
	void	insert(int,double,int);	// insert un élément avec ses 2 clefs
	void	remove(int);		// ôte un élément du tas
	int 	deletemin();		// retourne et détruit le plus petit élément
	void	changekey(int,double,int);// change les clefs d'un élément
	void	print();		// affiche le tas
};

// Retourne l'élément de plus petite clef
inline int heap::findmin() { return n == 0 ? 0 : h[1]; }

// Retourne la clef 1 de i.
inline double heap::key1(int i) { return kvec[i]; }

// Retourne la clef 2 de i.
inline int heap::key2(int i) { return card[i]; }


// Retourne true si i est dans le tas, et false sinon.
inline bool heap::member(int i) { return pos[i] != 0; }

// Retourne true si le tas est vide, et false sinon.
inline bool heap::empty() { return n == 0; };

#endif
