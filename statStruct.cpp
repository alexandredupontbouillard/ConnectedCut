#include <ctime>
#include "statStruct.h"



void statStruct::computeTime(){

	for(int i = 0 ; i < (int) sizeof (temps)/ sizeof(*temps); i++){
		temps[i] = temps[i]/CLOCKS_PER_SEC;

	}
	totalTime = (end - start)/CLOCKS_PER_SEC; 
	
}

void statStruct::printInfo(){
	computeTime();
	cout<<"-------NB CONSTRAINTS-------"<<endl;
	cout<<"CYCLE : "<< nbCuts[0]<<endl;
	cout<<"W1W2 : "<< nbCuts[2]<<endl;
	cout<<"W1W2I : "<< nbCuts[3]<<endl;
	cout<<"GLOUTON : "<< nbCuts[4]<<endl;
	
	cout<<"-------Separation Time-------"<<endl;
	cout<<"CYCLE : "<<temps[0] <<endl;
	cout<<"W1W2 : "<< temps[2]<<endl;
	cout<<"W1W2I : "<< temps[3]<<endl;
	cout<<"GLOUTON : "<< temps[4]<<endl;
	
	cout<<"-------Information-------"<<endl;
	cout<<"total time : "<< totalTime << endl;
	cout<<"nb nodes : "<< nbNodes << endl;
	cout<<"optimality gap  : "<< optimalityGap << endl;
	cout<<"nbCst At begining : "<< nbCst <<endl;
	cout<<"integrality gap : "<<   ( bestInteger  - bestObj ) / bestInteger<<endl;

}

void statStruct::writeFile(string s){

	fstream fichier(s);
	fichier.seekp(0,ios::end);//On se rend à la fin du fichier afin de ne pas effacer le contenu déjà présent
	fichier <<formulation << ";"<<totalTime<<";"<<nbNodes<<";"<<nbCuts[0]<<";" << temps[0]<< ";" << nbCuts[2] <<";" <<  temps[2]<< ";" << nbCuts[3]<< ";"  << temps[3] << ";" << nbCuts[4]<< ";"  << temps[4] << ";"<<nbCst<<";;"<< endl;
	fichier.close();


}


