#include "PrimaryGenerator.hh"
#include <iostream>
#include <cmath>
#include "G4RandomDirection.hh"

using namespace std;

G4ParticleGun* PrimaryGenerator::particleGun(0);

PrimaryGenerator::PrimaryGenerator(struct DataPass *LightInfoPass)
{
	mReaction = &(LightInfoPass->Reaction);
	mLightEnergy = &(LightInfoPass->LightEnergy);
	mLightTheta = &(LightInfoPass->LightTheta);
	mLightPhi = &(LightInfoPass->LightPhi);	

   G4String nameOfParticles = "proton";

   G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
   particleGun = new G4ParticleGun(1);
   particleGun -> SetParticleDefinition(particleTable -> FindParticle(nameOfParticles));
   
   particleGun -> SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0));
}


PrimaryGenerator::~PrimaryGenerator()
{
   delete particleGun;
}


void PrimaryGenerator::GeneratePrimaries(G4Event* event)
{
	//G4cout << "Generating Primys: " << G4endl;
	//--- set the beam spot size to a 2mm radius
   G4double randX = ( (G4UniformRand()-0.5)*( 4 ) )*mm;
   G4double randY = ( (G4UniformRand()-0.5)*( 4 ) )*mm;
	//--- Set the z interation in the target to random from front to back
   G4double randZ = (G4UniformRand()-0.5)*( (217/(2.267/1e-6))/2 )*cm;
   particleGun -> SetParticlePosition(G4ThreeVector(randX, randY, randZ));   
   G4ThreeVector PVecDirection;
   //double r1 = sqrt(r());
//G4UniformRand() calls from the flat() distribution in the CLHEP library.
//The definition can be found in RandFlat.icc   
   double r1; 
   double radius;
   double x,y,z;
   double PI = 4*atan(1);
   //double theta = acos(1-2*r());
   //double theta = 2*PI*G4UniformRand();
   double E, theta, phi; 
	G4double *kinematics;
	G4int Reaction = -1;
	G4double RandNum = G4UniformRand(); 
	if( RandNum <= 0.5 )
	{
		kinematics= mCalcLightProdKinematics();
		Reaction = 0;
	}
	
	else if( RandNum > 0.5 )
	{
		kinematics= mCalcLightProdKinematicsp6He4Het();		
		Reaction = 1;
	}
	*mReaction = Reaction;
	
	E = kinematics[0]*MeV;
	theta = kinematics[1];
	phi = kinematics[2];   
   
   particleGun -> SetParticleEnergy(E);    

   x = sin(theta)*cos(phi);
   y = sin(theta)*sin(phi);
   z = cos(theta);

   PVecDirection.set( x, y, z );
   particleGun -> SetParticleMomentumDirection(PVecDirection);
   //particleGun -> SetParticleMomentumDirection(G4ThreeVector(x, y, 65.0));
		
	//Create the 6He 2+ beam ion
  	//G4String nameOfParticles = "GenericIon";
	G4int Z = 2, A = 4;
	G4double ioncharge = 0.0*eplus;
	G4double excitEnergy = 0.*keV;
	if( Reaction == 0 )
	{
		//--- 4He
		Z = 2;
		A = 4;
		ioncharge = 0.0;
		excitEnergy = 0;
	}
	else if( Reaction == 1 )
	{
		//--- 4He
		Z = 2;
		A = 4;
		ioncharge = 0.0;
		excitEnergy = 0;	
	}
  	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
   G4ParticleDefinition *ion = particleTable->GetIon(Z,A,excitEnergy);      
  	particleGun->SetParticleDefinition( ion );
  	particleGun->SetParticleCharge( ioncharge );      
   particleGun -> GeneratePrimaryVertex(event);

	E = kinematics[3]*MeV;
	theta = kinematics[4];
	phi = kinematics[5];   

   particleGun -> SetParticleEnergy(E*MeV);    
   x = sin(theta)*cos(phi);
   y = sin(theta)*sin(phi);
   z = cos(theta);

   PVecDirection.set( x, y, z );
   particleGun -> SetParticleMomentumDirection(PVecDirection);
	
	if( Reaction == 0 )
	{
		//--- 14C
		Z = 6;
		A = 14;
		ioncharge = 0.0;
		excitEnergy = 0;
	}
	else if( Reaction == 1 )
	{
		//--- t
		Z = 1;
		A = 3;
		ioncharge = 0.0;
		excitEnergy = 0;	
	}	
	
   particleTable = G4ParticleTable::GetParticleTable();
	ion = particleTable->GetIon(Z,A,excitEnergy);      
   particleGun->SetParticleDefinition( ion );
   particleGun->SetParticleCharge( ioncharge );      
      
   particleGun -> GeneratePrimaryVertex(event);      
   //G4cout << "done." << G4endl;
}
 
G4double *PrimaryGenerator::mCalcLightProdKinematics()
{
	G4double *kinematics = new G4double[6];
	
	G4double temp, ExciteE=0;
	G4ThreeVector productDirection;
	
	G4double RandNum = G4UniformRand();
	
	G4double cut = 1./1.;
	
	/*
	if( RandNum > 0  && RandNum < cut )
		ExciteE = 0.0;
	else if( RandNum >= cut  && RandNum < 2*cut )
		ExciteE = 6.093;	
	*/
	if( RandNum >= 0  && RandNum < cut )
		ExciteE = 8.317;	
	/*
	if( RandNum >= 0  && RandNum < cut )
		ExciteE = 6.589;
	else if( RandNum >= cut  && RandNum < 2*cut )
		ExciteE = 6.728;
	else if( RandNum >= 2*cut  && RandNum < 3*cut )
		ExciteE = 7.012;
	else if( RandNum >= 3*cut  && RandNum < 4*cut )
		ExciteE = 7.341;				
	else if( RandNum >= 4*cut  && RandNum < 5*cut )
		ExciteE = 8.317;
	else if( RandNum >= 7*cut  && RandNum < 8*cut )
		ExciteE = 9.746;
	else if( RandNum >= 8*cut  && RandNum < 9*cut )
		ExciteE = 9.801;
	else if( RandNum >= 9*cut  && RandNum < 10*cut )
		ExciteE = 10.425;																								
	*/	
	m1 = 6*931.494 + 17.5928;
	m2 = 12*931.494;
	m3 = 4*931.494 + 2.4249156;
	m4 = 14*931.494 + 3.019892 + ExciteE;
	
	G4double pi = 4*atan(1.0);
	G4double qValue = m1 + m2 - m3 - m4;
	//G4cout << "qValue: " << qValue << " ExciteE " << ExciteE << " RandNum " << RandNum << G4endl;
	double BeamE = 29.950;
	
	G4double Energy1 = BeamE + m1;
	G4double EnergyT = Energy1 + m2; 
	
	G4double P1 = sqrt(BeamE * BeamE + 2 * m1 * BeamE);
	
	G4double A = 2 * m2 * BeamE + 2 * m1 * m3 + 2 * m2 * m3 + 2 * qValue * (m1 + m2 - m3) - qValue * qValue;
	
	G4double maxTheta = pi;
	G4double term = (A * A - 4 * EnergyT * EnergyT * m3 * m3) / (m3 * m3 * P1 * P1);
	term = 0.5*sqrt( -term );
	
	temp = acos(term);
	if ( temp > 0 && temp < pi)
   	maxTheta = temp;
   temp = -acos(term);
   if ( temp > 0 && temp < maxTheta)
      maxTheta = temp;
   temp = acos(-term);
   if ( temp > 0 && temp < maxTheta)
      maxTheta = temp;
   temp = -::acos(-term);
   if ( temp > 0 && temp < maxTheta)
      maxTheta = temp;	
      
   G4double zmin = cos(maxTheta);
   
   //G4double tempTheta = acos( zmin + (rand() % 10000)/10000.0*(1.0-zmin));
   G4double tempTheta = acos( 2*G4UniformRand() - 1 );   
   //G4double tempPhi = (rand() % 10000)/10000.0 * 2.0 * pi;
   G4double tempPhi = G4UniformRand() * 2.0 * pi;
   G4double HeavyPhi;
   
	if( tempPhi < pi )
		HeavyPhi = pi + tempPhi;
	else if( tempPhi > pi )
		HeavyPhi = tempPhi - pi;	   
   
   productDirection[0] = sin(tempTheta)*cos(tempPhi);
   productDirection[1] = sin(tempTheta)*sin(tempPhi);
   productDirection[2] = cos(tempTheta);
   
   G4double Theta = productDirection.theta();
   G4double B = EnergyT*EnergyT - pow(P1*cos(Theta),2);
   term = sqrt( A*A - 4*m3*m3*B );
   
   G4double productEnergy, HeavyProdEnergy;
	if( maxTheta == pi || ((long)((rand() % 10000)/10000.0 * 10000) % 2) == 0 )
   {
		productEnergy = 1/(2*B) * (EnergyT * A + P1 * ::cos(Theta) * term) - m3;
		double energy4 = EnergyT - ( productEnergy + m3 ) - (m1 + m2 - m3 - qValue ) + m4;	
    	HeavyProdEnergy = energy4 - m4;
   }
   else
   {
		productEnergy = 1/(2*B) * (EnergyT * A - P1 * ::cos(Theta) * term) - m3;
		double energy4 = EnergyT - ( productEnergy + m3 ) - (m1 + m2 - m3 - qValue ) + m4;	
	  	HeavyProdEnergy = energy4 - m4;
	}   

	G4double energy3 = productEnergy + m3;
	G4double energy4 = EnergyT - energy3 - (m1 + m2 - m3 - qValue) + m4;
	G4double p3 = sqrt(energy3 * energy3 - m3 * m3);
	G4double p4 = sqrt(energy4 * energy4 - m4 * m4);
	G4double HeavyProdTheta = asin(p3 * sin(Theta) / p4);		
	
	//G4cout << "energy3: " << energy3 << " energy4 " << energy4 << " Theta: " << Theta*180/pi << " HTheta: " << HeavyProdTheta*180/pi << G4endl; 	
	
	*mLightEnergy = productEnergy;
	*mLightTheta = Theta;	
	*mLightPhi = tempPhi;
	
	kinematics[0] = productEnergy;
	kinematics[1] = Theta;
	kinematics[2] = tempPhi;
	
	kinematics[3] = HeavyProdEnergy;
	kinematics[4] = HeavyProdTheta;
	kinematics[5] = HeavyPhi;
	//Return in the form of E, theta, phi
	return kinematics;
}

G4double *PrimaryGenerator::mCalcLightProdKinematicsp6He4Het()
{
	G4double *kinematics = new G4double[6];
	
	G4double temp, ExciteE=0;
	G4ThreeVector productDirection;
	
	G4double RandNum = G4UniformRand();
	
	G4double cut = 1./1.;
	
	if( RandNum > 0  && RandNum < cut )
		ExciteE = 0.0;																					
	
	//--- p(6He,4He)t
	m1 = 6*931.494 + 17.5928;
	m2 = 1*931.494 + 7.2889;
	m3 = 4*931.494 + 2.4249156;
	m4 = 3*931.494 + 14.9498 + ExciteE;
	
	G4double pi = 4*atan(1.0);
	G4double qValue = m1 + m2 - m3 - m4;
	//G4cout << "qValue: " << qValue << " ExciteE " << ExciteE << " RandNum " << RandNum << G4endl;
	double BeamE = 29.950;
	
	G4double Energy1 = BeamE + m1;
	G4double EnergyT = Energy1 + m2; 
	
	G4double P1 = sqrt(BeamE * BeamE + 2 * m1 * BeamE);
	
	G4double A = 2 * m2 * BeamE + 2 * m1 * m3 + 2 * m2 * m3 + 2 * qValue * (m1 + m2 - m3) - qValue * qValue;
	
	G4double maxTheta = pi;
	G4double term = (A * A - 4 * EnergyT * EnergyT * m3 * m3) / (m3 * m3 * P1 * P1);
	term = 0.5*sqrt( -term );
	
	temp = acos(term);
	if ( temp > 0 && temp < pi)
   	maxTheta = temp;
   temp = -acos(term);
   if ( temp > 0 && temp < maxTheta)
      maxTheta = temp;
   temp = acos(-term);
   if ( temp > 0 && temp < maxTheta)
      maxTheta = temp;
   temp = -::acos(-term);
   if ( temp > 0 && temp < maxTheta)
      maxTheta = temp;	
      
   G4double zmin = cos(maxTheta);
   
   G4double tempTheta = acos( zmin + (rand() % 10000)/10000.0*(1.0-zmin));
   //G4double tempTheta = acos( 2*G4UniformRand() - 1 );   
   //G4double tempPhi = (rand() % 10000)/10000.0 * 2.0 * pi;
   G4double tempPhi = G4UniformRand() * 2.0 * pi;
   G4double HeavyPhi;
   
	if( tempPhi < pi )
		HeavyPhi = pi + tempPhi;
	else if( tempPhi > pi )
		HeavyPhi = tempPhi - pi;	   
   
   productDirection[0] = sin(tempTheta)*cos(tempPhi);
   productDirection[1] = sin(tempTheta)*sin(tempPhi);
   productDirection[2] = cos(tempTheta);
   
   G4double Theta = productDirection.theta();
   G4double B = EnergyT*EnergyT - pow(P1*cos(Theta),2);
   term = sqrt( A*A - 4*m3*m3*B );
   
   G4double productEnergy, HeavyProdEnergy;
	if( maxTheta == pi || ((long)((rand() % 10000)/10000.0 * 10000) % 2) == 0 )
   {
		productEnergy = 1/(2*B) * (EnergyT * A + P1 * ::cos(Theta) * term) - m3;
		double energy4 = EnergyT - ( productEnergy + m3 ) - (m1 + m2 - m3 - qValue ) + m4;	
    	HeavyProdEnergy = energy4 - m4;
   }
   else
   {
		productEnergy = 1/(2*B) * (EnergyT * A - P1 * ::cos(Theta) * term) - m3;
		double energy4 = EnergyT - ( productEnergy + m3 ) - (m1 + m2 - m3 - qValue ) + m4;	
	  	HeavyProdEnergy = energy4 - m4;
	}   

	G4double energy3 = productEnergy + m3;
	G4double energy4 = EnergyT - energy3 - (m1 + m2 - m3 - qValue) + m4;
	G4double p3 = sqrt(energy3 * energy3 - m3 * m3);
	G4double p4 = sqrt(energy4 * energy4 - m4 * m4);
	G4double HeavyProdTheta = asin(p3 * sin(Theta) / p4);		
	
	//G4cout << "energy3: " << energy3 << " energy4 " << energy4 << " Theta: " << Theta*180/pi << " HTheta: " << HeavyProdTheta*180/pi << G4endl; 	
	
	*mLightEnergy = productEnergy;
	*mLightTheta = Theta;
	
	kinematics[0] = productEnergy;
	kinematics[1] = Theta;
	kinematics[2] = tempPhi;
	
	kinematics[3] = HeavyProdEnergy;
	kinematics[4] = HeavyProdTheta;
	kinematics[5] = HeavyPhi;
	//Return in the form of E, theta, phi
	return kinematics;
}
