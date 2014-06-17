//Program to run multiple Geant4 run for different geometry setups in a row
//
//
//
//
//Sascha Falahat, sfalahat@nd.edu, 08-08-2007
//
//
//
//
//
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;


int main(int argc, char** argv)
{

        
        int  numberOfRingsMax = 1;
        
	
		//open DetectorParameter.dat			
		ofstream *fout = new ofstream("DetectorParameter.dat");
		*fout << numberOfRingsMax << endl;   // and write it to the file
		*fout << argv[1] << "   " << argv[2] <<endl <<endl;   // and write it to the file
		fout->close();
	return 0;
}
