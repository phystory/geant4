#include <iostream>

#include <TFile.h>
#include <TTree.h>

#include <cmath>

#include "MyStat.hh"

using namespace std;

MyStat::MyStat(int n)
{
 nb_triton=0;
 nb_proton=0;
 nb_event=0;
 have_reacted=0;
 in_tube=0;
 have_reacted_DoIt_proton=0;
 have_reacted_DoIt_triton=0;
 have_reacted_Done_proton=0;
 have_reacted_Done_triton=0;
 CleanEvent();
 nb_tube = n;
 nb_neutron_tube = new int[n];
 nb_reaction_tube = new int[n];
 xtube = new double[n];
 ytube = new double[n];

 for (int i=0;i<n;i++){nb_neutron_tube[i]=0;nb_reaction_tube[i]=0; xtube[i]=0.;ytube[i]=0.;}

 store_file = new TFile("G4data/storedata.root","RECREATE");
 tree = new TTree("tree","store result of G4 simulation");
 tree->Branch("e_proton",&e_proton,"e_proton/D");
 tree->Branch("e_triton",&e_triton,"e_triton/D");
 tree->Branch("initial_e_neutron",&initial_e_neutron,"initial_e_neutron/D");
 tree->Branch("tube_number_reaction",&tube_number_reaction,"tube_number_reaction/I");
 tree->Branch("tube_number_neutron",&tube_number_neutron,"tube_number_neutron/I");

}

void MyStat::CleanEvent()
{
 e_proton=-1;
 initial_e_neutron=-1;
 e_triton=-1;
 have_reacted=0;
 in_tube=0;
 have_reacted_DoIt_proton=0;
 have_reacted_DoIt_triton=0;
 have_reacted_Done_proton=0;
 have_reacted_Done_triton=0;
 tube_number_reaction=-1;
 tube_number_neutron=-1;
}

void MyStat::NewEvent(double e)
{
 CleanEvent();
 nb_event++;
 initial_e_neutron=e;
}

int MyStat::FindTube(double x,double y, double z)
{
 int close=-1;
 double min=1000000000.;
 for (int i=0;i<nb_tube;i++) 
 {
   if (sqrt((x-xtube[i])*(x-xtube[i])+(y-ytube[i])*(y-ytube[i]))<min)
   {
    min=sqrt((x-xtube[i])*(x-xtube[i])+(y-ytube[i])*(y-ytube[i]));
    close=i;
   }
  }
 return close;
}

void MyStat::PrintScreen()
{
 cout <<"===================================="<<endl;
 cout << nb_event << " neutron simulated" << endl;
 int count_n,count_r;
 count_n=0;count_r=0;
 for(int i=0;i<nb_tube;i++){count_n+=nb_neutron_tube[i]; count_r+=nb_reaction_tube[i];}
 cout << count_n << " neutron reach tubes" <<endl;
 cout << count_r << " reaction occurs" <<endl;
 cout <<"===================================="<<endl;
}

void MyStat::FillTree()
{
 tree->Fill();
}

void MyStat::SaveTree()
{
 store_file->Write();
}


