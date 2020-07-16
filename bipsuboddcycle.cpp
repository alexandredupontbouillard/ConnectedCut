#include"classoddcycle.h"
#include"bipsub.h"
#include<list>
#include<algorithm>


bool C_bipsub::lance_separation_oddcycle(vector<int> &v_som_dans_cte,int nbctesouhaite, int nbitermax, int nbctepool, int &nbcteoc){
  int nbsom=maitre()->G.nbsom;
  int i;
  list<list<int> > Lcteoc;
  C_oddcycle *cteoc;
  list<list<int> >::const_iterator it;  
  list<int>::const_iterator iti;
  bool okoc=true;
  vector<int> v_trouvecycle;

    v_trouvecycle.resize(nbsom+1);
    v_trouvecycle=v_som_dans_cte;
 
    maj_struct_dijkstra();
 
    for (i=1;i<=nbsom;i++)
      if (v_som_dans_cte[i]!=0){
	vire_sommet_struct_dijkstra(i);
	vire_sommet_struct_dijkstra(i+nbsom);
      }

    nbcteoc=separe_oddcycle(Lcteoc,nbctesouhaite,nbitermax,v_trouvecycle,nbctepool);


//       nbcteoc=separe_oddcycle(Lcteoc,3*maitre()->nbctepiter,nbsom+1,v_trouvecycle,nbctepool);
//       nbcteoc=separe_oddcycle(Lcteoc,maitre()->nbctepiter-nbcteocpool,3*maitre()->nbctepiter,v_trouvecycle,nbctepool);


    if (nbcteoc>0){
      ABA_BUFFER<ABA_CONSTRAINT*> constraintoc(master_,nbcteoc);

      it=Lcteoc.begin();
      while (it!=Lcteoc.end()){
         cteoc= new C_oddcycle(master_,*it);
         constraintoc.push(cteoc);
//         cteoc->ecrit_cte_fic();
	 v_som_dans_cte[(*it).front()]=6;
         it++;    
      }

      if ((addCons(constraintoc,maitre()->Pooloddcycle),0,0)!=nbcteoc)
               //(addCons(constraintoc)!=nbcteoc)         
         okoc=false;     
    }

    cout<<"On ajoute "<<nbcteoc<<" ctes de cycles impairs"<<endl; 


    return(okoc);

}


int C_bipsub::separe_oddcycle(list<list<int> > &Lcte, int nbctesouhaite, int nbitermax, vector<int> &v_trouvecycle, bool pastout){
       // si pastout est faux (0) alors on explore systématiquement
  C_bipmaster *maitre;
  int sommetS,nbsom;               // Sommet courant
  int tttrouve;                    // Test fin de recherche

  list<int> chemin;                // dernier pcc cycle impair
  list<int>::iterator it,it2;
  list<list<int> >::iterator itl;
  list<int> L_vire;

  vector<int> chapeau;

  int i,j,sav,itS,temp,temp2,tttt;
  double vall;
  bool stopit,arretgdeboucle;
  int recol;
  int tc1,tc2;
  int sortiedij;
  bool sucre;
  maitre=(C_bipmaster *) master_;
  nbsom=maitre->G.nbsom;
  
  chapeau.resize(nbsom+1);

  tc1=0;
  for (i=1;i<=nbsom;i++)
    if ((v_trouvecycle[i]==0)&&(valx(i)==1)){
       tc1++;
       chapeau[tc1]=i;
    }

  tc2=tc1;
  for (i=1;i<=nbsom;i++)
    if ((v_trouvecycle[i]==0)&&(valx(i)<1)){
       tc2++;
       chapeau[tc2]=i;
    }

  for (i=1;i<=tc1;i++){
    j=(int)(rand()%tc1)+1;
    sav=chapeau[i];
    chapeau[i]=chapeau[j];
    chapeau[j]=sav;
  }

  if (tc1+1<tc2){
   for (i=tc1+1;i<=tc2;i++){
    j=(int)(rand()%(tc2-tc1-1))+tc1+1;
    sav=chapeau[i];
    chapeau[i]=chapeau[j];
    chapeau[j]=sav;
   }
  }

  arretgdeboucle=false;
  tttt=500;
  itS=1;
  sucre=false;

  while ((!arretgdeboucle)&&(itS<=nbitermax)&&(itS<=tc2)){  // Boucle de recherche

      sommetS=chapeau[itS];

      if (itS>=tttt) {
        cout<<"Gde boucle itS="<<itS<<endl;
        tttt+=500;
      }

      if (v_trouvecycle[sommetS]==0) {

        chemin.clear();
	L_vire.clear();

        recol=maitre->dikh(sommetS,sommetS+nbsom, sucre, L_vire); 
        sortiedij=maitre->cree_chemin(sommetS,sommetS+nbsom,recol,chemin);

        for (it=chemin.begin();it!=chemin.end();it++)
            if ((*it)>nbsom) (*it)=(*it)-nbsom;

        v_trouvecycle[sommetS]=1;
        vire_sommet_struct_dijkstra(sommetS);
        vire_sommet_struct_dijkstra(sommetS+nbsom);

	if (sortiedij) {

             L_vire.clear();

             maj_v_trouvecycle_and_cut(chemin,v_trouvecycle,0,L_vire);

             v_trouvecycle[chemin.front()]=1;

             extrait_hole(chemin);

             vall=0;
             for (it=chemin.begin();it!=chemin.end();it++)
                 vall+=valxpur(*it);

             if (vall>chemin.size()-1+10E-3) {

               v_trouvecycle[chemin.front()]=1;
               vire_sommet_struct_dijkstra(chemin.front());
               vire_sommet_struct_dijkstra(chemin.front()+nbsom);
               
               Lcte.push_back(chemin);

             }
	     else{
	       for (it=L_vire.begin();it!=L_vire.end();it++){
                  vire_sommet_struct_dijkstra(*it);
                  vire_sommet_struct_dijkstra(*it+nbsom);
	       }

	     }
        }
	else{
	   for (it=L_vire.begin();it!=L_vire.end();it++){
               v_trouvecycle[*it]=1;
               vire_sommet_struct_dijkstra(*it);
               vire_sommet_struct_dijkstra(*it+nbsom);
	   }
	}

     
      }  // fin traitement sommet

      if (Lcte.size()==nbctesouhaite)
         arretgdeboucle=true;

      if ((!pastout)&&(Lcte.size()==0)&&(itS==nbitermax)){
  	 nbitermax=nbitermax+200;
         sucre=true;
      }

      itS++;

  } // Fin Boucle de recherche

  return(Lcte.size());

}



void C_bipsub::maj_v_trouvecycle_and_cut(list<int> &L, vector<int> &vtrouv, int opt,list<int> &L_vire){
                 // Met à 2 les sommets de chaque cote de L jusqu'au dernier
                 // sommet en double
  list<int>::iterator it,it2,ita,itb,itt;
  
  ita=L.begin();
  itb=L.end();
  it2=itb;it2--;
  it=ita;it++;
  itt=it;itt++;
  while ((it2!=itt)&&(it2!=it)){
    if ((*it2)==(*it)){
      itb=it2;
      ita=it;
    }
    it++;
    it2--;
    itt++;
  }

  for (it=L.begin();it!=ita;it++) 
    if (vtrouv[*it]==0){
         vtrouv[*it]=2;
	 L_vire.push_back(*it);
    }

  for (it=itb;it!=L.end();it++) 
    if (vtrouv[*it]==0) {
         vtrouv[*it]=2;
	 L_vire.push_back(*it);
    }

  if (opt==1){    
    L.erase(L.begin(),ita);
    L.erase(itb,L.end());
  }

//for (ita=L.begin();ita!=L.end();ita++) cout<<*ita<<" ";cout<<endl;
  
}


void C_bipsub::extrait_hole(list<int> &L){

  list<int>::iterator it,it2,its,itsom,ita;
  list<int> *listv;
  C_bipmaster *maitre=(C_bipmaster*) master_;  
  bool are;

//for (ita=L.begin();ita!=L.end();ita++) cout<<*ita<<" ";cout<<endl;

  are=false;
  it=L.begin();it++;it++;
  while ((!are)&&(it!=L.end())){
    it2=L.begin();
    its=it;its--;its--;
    listv=&(maitre->G.v_sommets[*it]->l_vois);
    while ((!are)&&(it2!=its)) {
      if (find(listv->begin(),listv->end(),*it2)!=listv->end())
	are=true;      
      it2++;
    }
    it++;
  }

  are=false;
  it--;
  listv=&(maitre->G.v_sommets[*it]->l_vois);
  it2=it;it2--;it2--;
  while ((!are)&&(it2!=L.begin())){
    if (find(listv->begin(),listv->end(),*it2)!=listv->end())
       are=true;      
    it2--;
  }

  if (are) it2++;

  L.erase(L.begin(),it2);
  it++;
  L.erase(it,L.end());

//for (ita=L.begin();ita!=L.end();ita++) cout<<*ita<<" ";cout<<endl;


}


bool C_bipsub::verif(list<int>L){
  bool pbm;
  list<int>::const_iterator it,it2;

  pbm=0;

  if ((L.size()%2)==0){
    cout<<endl<<endl<<"Ou la la pas impair"<<endl;
    it=L.begin();
    while(it!=L.end()){
      cout<<*it<<" ";
      it++;
    }
    cout<<endl<<endl;
    pbm=1;
  }


  it=L.begin();
  it2=it;
  it2++;
  while ((!pbm)&&(it2!=L.end())){
     if (find(maitre()->G.v_sommets[*it]->l_vois.begin(),
              maitre()->G.v_sommets[*it]->l_vois.end(),
              *it2)
                   ==maitre()->G.v_sommets[*it]->l_vois.end())
     pbm=2;

    it++;
    it2++;
  }
  it2=L.begin();
  if (find(maitre()->G.v_sommets[*it]->l_vois.begin(),
           maitre()->G.v_sommets[*it]->l_vois.end(),
           *it2)
      ==maitre()->G.v_sommets[*it]->l_vois.end()){
     pbm=2;
  }

  if (pbm==2) cout<<"C'est pire"<<endl<<endl;

 
  return(pbm);
}


