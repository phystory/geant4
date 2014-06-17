#include <iostream>
#include <fstream>
#include <cstdlib>

#include "MyConfig.hh"

using namespace std;

MyConfig::MyConfig()
{
 if (ReadData()){;}
 else
 {cerr<<"An error occured during reading of the DetectorParameter.dat file reading -- sorry."<< endl; exit(-1); }
}


int MyConfig::ReadData()
{
 ifstream infile;
 char buffer[200];
 infile.open ("DetectorParameter.dat");
 if (infile>>nb_radius) infile.getline(buffer,200); else goto stop;
 nb_tube = new int[nb_radius];
 radius = new float[nb_radius];
 for (int i=0;i<nb_radius;i++)
   if(infile >> nb_tube[i] >> radius[i]) infile.getline(buffer,200); else goto stop;
 return 1;
 stop:return 0;
}


int MyConfig::TotalNumberofTube()
{
 int ret=0;
 for (int i=0;i<nb_radius;i++) ret+=nb_tube[i];
 return ret;
}


