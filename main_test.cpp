using namespace std;

#include"Graph.h"

 int main(int argc,char**argv){

 

   if(argc!=2){
     cerr<<"usage: "<<argv[0]<<" <file.dat>"<<endl;
     return 1;
   }

   Graph G(argv[1]);

   G.Print();

   G.Write_ps_pdf();

   return 0;

 }
