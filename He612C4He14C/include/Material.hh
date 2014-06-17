#ifndef Material_H
#define Material_H 1

#include "globals.hh"
#include "G4Material.hh"

class Material
 {

   public:
      Material();
      ~Material();
      void DefineMaterials();

		G4Material* Vacuum;
      G4Material* Air;
      G4Material* Aluminum;
      G4Material* BC501A;
      G4Material* BC537;
      G4Material* Deuterium;
      G4Material* Hydrogen;
      G4Material* Silicon;
      G4Material* Board;      
      G4Material* Target;

   private:
      void defineElements();
      void makeVacuum();
      void makeAir();
      void makeAluminum();
      void makeBC501A();
      void makeBC537();
      void makeDeuterium();
      void makeHydrogen();
      void makeSilicon();
      void makeTarget();      
      void makeFiberGlass();

      G4Element* elAl;
      G4Element* elB;
      G4Element* elCa;
      G4Element* elK;      
      G4Element* elC;
      G4Element* elD;
      G4Element* elH;
      G4Element* elN;
      G4Element* elNa;      
      G4Element* elO;
      G4Element* elSi;      

 };

#endif



