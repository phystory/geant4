//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "AlTubeParametrisation.hh"
//#include "HeTube01DetectorParameterDef.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"




AlTubeParametrisation::AlTubeParametrisation()
{

//#include "NeutronDetectorParameterDef.icc"


}

AlTubeParametrisation::~AlTubeParametrisation()
{;}

void AlTubeParametrisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4double phi;
  phi = 360.*deg;
  G4double x=Radius*sin(copyNo*phi/numberTubes);
  G4double y=Radius*cos(copyNo*phi/numberTubes);
  G4double z=ParaffinBlock_z/4;

  physVol->SetTranslation(G4ThreeVector(x,y,z));


}

void AlTubeParametrisation::ComputeDimensions
(G4Tubs& AlTubes_Param, const G4int copyNo, const G4VPhysicalVolume*) const
{
  AlTubes_Param.SetInnerRadius(innerRadiusOfAlTube);
  AlTubes_Param.SetOuterRadius(outerRadiusOfAlTube);
  AlTubes_Param.SetZHalfLength(heightOfAlTube);
  AlTubes_Param.SetStartPhiAngle(startAngleOfAlTube);
  AlTubes_Param.SetDeltaPhiAngle(spanningAngleOfAlTube);
}
