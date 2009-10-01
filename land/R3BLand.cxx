// -------------------------------------------------------------------------
// -----                        R3BLand source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BLand.h"

#include "R3BGeoLand.h"
#include "R3BLandPoint.h"
#include "R3BGeoLandPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BLand::R3BLand() : R3BDetector("R3BLand", kTRUE, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fVersion =1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BLand::R3BLand(const char* name, Bool_t active) 
  : R3BDetector(name, active, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fVersion = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BLand::~R3BLand() {

  if ( flGeoPar ) delete flGeoPar;
  if (fLandCollection) {
    fLandCollection->Delete();
    delete fLandCollection;
  }
}
// -------------------------------------------------------------------------

void R3BLand::Initialize()
{

  FairDetector::Initialize();

  cout << "-I- R3BLand: initialisation " << endl;
  cout << "-I- R3BLand: Paddle B3 (McId): " << gMC->VolId("padle_h_box3") << endl;
  cout << "-I- R3BLand: Paddle B4 (McId): " << gMC->VolId("padle_h_box4") << endl;
  cout << "-I- R3BLand: Paddle B5 (McId): " << gMC->VolId("padle_h_box5") << endl;

 Int_t id1 = gMC->VolId("padle_h_box3");
 Int_t id2 = gMC->VolId("padle_h_box4");
 Int_t id3 = gMC->VolId("padle_h_box5");

 // Sensitive Volumes :: Unique  Id
 //  paddle_h_box3          1
 //  paddle_h_box4          2
 //  paddle_h_box5          3

 fMapMcId[id1]=1;
 fMapMcId[id2]=2;
 fMapMcId[id3]=3;


}





// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BLand::ProcessHits(FairVolume* vol) {

  // --- get Geometry hiearchical Information
  Int_t cp1=-1;
  Int_t cp2=-1;
  Int_t volId1=-1;
  Int_t volId2=-1;


  if ( fVersion == 1) {
  // Crystals Ids
  volId1 =  gMC->CurrentVolID(cp1);
  // Lead - Crystal numbering scheme
  fPaddleTyp = fMapMcId[volId1];
  // Mother Assembly def
  volId2 =  gMC->CurrentVolOffID(1, cp2);
  }

  if ( (fVersion == 2) || ( fVersion == 3) ){

  volId1 =  gMC->CurrentVolID(cp1);
  volId2 =  gMC->CurrentVolOffID(1, cp2);
  }

   if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  
  // Set additional parameters at exit of active volume. Create R3BLandPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);

    if (fELoss == 0. ) return kFALSE;
    
    if (gMC->IsTrackExiting()) {
      const Double_t* oldpos;
      const Double_t* olddirection;
      Double_t newpos[3];
      Double_t newdirection[3];
      Double_t safety;
      
      gGeoManager->FindNode(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      oldpos = gGeoManager->GetCurrentPoint();
      olddirection = gGeoManager->GetCurrentDirection();
      
//       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

      for (Int_t i=0; i<3; i++){
	newdirection[i] = -1*olddirection[i];
      }
      
      gGeoManager->SetCurrentDirection(newdirection);
    //  TGeoNode *bla = gGeoManager->FindNextBoundary(2);
      safety = gGeoManager->GetSafeDistance();


      gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);
      
      for (Int_t i=0; i<3; i++){
	newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }

      if ( fPosIn.Z() < 30. && newpos[2] > 30.02 ) {
	cerr << "-I- R3BLand : 2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] 
	     << " with safety = " << safety << endl;
	cerr << "-I- R3BLand oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << endl;
	cerr << "-I- R3BLand newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << endl;
      }

      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }

    AddHit(fTrackID, fVolumeID,   fPaddleTyp,   cp2, cp1,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of LandPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kLAND);
    
    ResetParameters();
  }

  return kTRUE;
}




// -----   Public method EndOfEvent   -----------------------------------------
void R3BLand::BeginEvent() {
   // cout << "-I- begin event called ############################# " << endl;

    if (gGeoManager) {

    TGeoVolume * vol = gGeoManager->FindVolumeFast("padle_h_box3");

    // cout << "UID box3 : " << gGeoManager->GetUID("padle_h_box3") << endl;
    //cout << "UID box4 : " << gGeoManager->GetUID("padle_h_box4") << endl;
    //cout << "UID box5 : " << gGeoManager->GetUID("padle_h_box5") << endl;

	    if (vol) {

	       //	cout << "id box3 serial number: " << vol->GetNumber() << endl;

	    }

    }


//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I- LAND geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BLand::EndOfEvent() {

  if (fVerboseLevel) Print();
  fLandCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BLand::Register() {
  FairRootManager::Instance()->Register("LandPoint", GetName(), fLandCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BLand::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fLandCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BLand::Print() const {
  Int_t nHits = fLandCollection->GetEntriesFast();
  cout << "-I- R3BLand: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BLand::Reset() {
  fLandCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BLand::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BLand: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BLandPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BLandPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BLandPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BLand: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BLandPoint* R3BLand::AddHit(Int_t trackID, Int_t detID, Int_t box, Int_t id1, Int_t id2,
			     TVector3 posIn,
			     TVector3 posOut, TVector3 momIn,
			     TVector3 momOut, Double_t time,
			     Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fLandCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BLand: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BLandPoint(trackID, detID, box, id1, id2,  posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BLand::ConstructGeometry() {
   if (fVersion == 1 ) return ConstructGeometry1();
   if ((fVersion == 2 ) || (fVersion ==3 )) return ConstructGeometry2();
   cout << "-I- R3BLand  ConstruGeometry() : unknown Geometry  !!! " << endl; 
}

void R3BLand::ConstructGeometry1() {   

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t a;
   Double_t z, density, w;
   Double_t tx,ty,tz;
   Int_t nel, numed;
   Double_t radl,absl;

/****************************************************************************/
// Material definition

   // Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
    nel     = 2;
    density = 0.001290;
    TGeoMixture*
	pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
    pMat2->DefineElement(1,a,z,w);
    pMat2->SetIndex(1);
    // Medium: Air
    numed   = 1;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }


//------------------Creat media----------------------------------
 // --  Material: Iron
   TGeoMedium * pMedFe=NULL;
   if (gGeoManager->GetMedium("Iron") ){
       pMedFe=gGeoManager->GetMedium("Iron");
   }else{
    w       =        0.;
    a       = 55.850000;
    z       = 26.000000;
    density = 7.870000;
    radl    = 1.757717;
    absl    = 169.994418;
    TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
    pMatFe->SetIndex(701);
    numed   = 23;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
   }

   TGeoMedium *Iron = pMedFe;

 // BC408 plastic medium
 // Mixture: BC408
   TGeoMedium * pMed37=NULL;
   if (gGeoManager->GetMedium("BC408") ){
       pMed37=gGeoManager->GetMedium("BC408");
   }else{
     nel     = 2;
     density = 1.032000;
     TGeoMixture*
	 pMat37 = new TGeoMixture("BC408", nel,density);
     a = 1.007940;   z = 1.000000;   w = 0.520000;  // H
     pMat37->DefineElement(0,a,z,w);
     a = 12.010700;   z = 6.000000;   w = 0.480000;  // C
     pMat37->DefineElement(1,a,z,w);
     pMat37->SetIndex(36);
     // Medium: BC408
     numed   = 36;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed37 = new TGeoMedium("BC408", numed,pMat37,par);
   }

   //
// <DB> Attempt to use TGeo Assemblies
// to model neutron detectors 25.03.09
//

   TGeoVolume* vWorld = gGeoManager->GetTopVolume();
   vWorld->SetVisLeaves(kTRUE);

   // Create a global Mother Volume
   /*
   dx = 500.000000;
   dy = 500.000000;
   dz = 500.000000;
   TGeoShape *pBoxWorld = new TGeoBBox("LandBoxWorld", dx,dy,dz);
   TGeoVolume*
   vWorld  = new TGeoVolume("LandBoxLogWorld",pBoxWorld, pMed2);
   vWorld->SetVisLeaves(kTRUE);
   TGeoCombiTrans *pGlobalc = GetGlobalPosition();

   // add the sphere as Mother Volume
   pAWorld->AddNode(vWorld, 0, pGlobalc);
   */


    // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  Double_t padle_h_dim1x = 100.1;
  Double_t padle_h_dim1y = 5.1;
  Double_t padle_h_dim1z = 5.1;
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
					 padle_h_dim1x,
					 padle_h_dim1y,
					 padle_h_dim1z);
  Double_t padle_h_dim2x = 100.0;
  Double_t padle_h_dim2y = 5.0;
  Double_t padle_h_dim2z = 5.0;
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
					 padle_h_dim2x,
					 padle_h_dim2y,
					 padle_h_dim2z);

  // Create a composite shape
  TGeoCompositeShape *sheetbox
      = new TGeoCompositeShape("diffbox", "padle_h_box1 - padle_h_box2");
  // Corresponding boolean volume
  TGeoVolume *bvol = new TGeoVolume("sheetbox",sheetbox,Iron);

  //end-cap list
  dx1 = 5.100000;
  dx2 = 2.510000;
  dy1 = 5.100000;
  dy2 = 2.510000;
  dz  = 5.000000;

  TGeoVolume *trap = gGeoManager->MakeTrd2("atrap",pMed37,dx1,dx2,dy1,dy2,dz);

  // sheet Iron list
  //------------------ Iron sheets -----------------------------------------
  Double_t padle_h_dim3x = 100.0;
  Double_t padle_h_dim3y = 5.0;
  Double_t padle_h_dim3z = 0.125;

  TGeoVolume *padle_h_box3
      =gGeoManager->MakeBox("padle_h_box3",Iron,
                                    padle_h_dim3x,
				    padle_h_dim3y,
				    padle_h_dim3z);

  //cout << " box3: " << padle_h_box3->GetNumber() << endl;



  Double_t padle_h_dim4x = 100.0;
  Double_t padle_h_dim4y = 5.0;
  Double_t padle_h_dim4z = 0.25;

  TGeoVolume *padle_h_box4
      =gGeoManager->MakeBox("padle_h_box4",Iron,
                                    padle_h_dim4x,
				    padle_h_dim4y,
				    padle_h_dim4z);

  // cout << " box4: " << padle_h_box4->GetNumber() << endl;


  //------------------ BC408 sheets -----------------------------------------
  Double_t padle_h_dim5x = 100.0;
  Double_t padle_h_dim5y = 5.0;
  Double_t padle_h_dim5z = 0.25;


  TGeoVolume *padle_h_box5
      =gGeoManager->MakeBox("padle_h_box5",pMed37,
                                    padle_h_dim5x,
				    padle_h_dim5y,
				    padle_h_dim5z);


  // cout << " box5: " << padle_h_box5->GetNumber() << endl;


 // Make the elementary assembly of the whole structure
   TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");

   TGeoRotation *rot0 = new TGeoRotation();
   rot0->RotateX(0.);
   rot0->RotateY(0.);
   rot0->RotateZ(0.);
   Double_t xx = 0.;
   Double_t yy = -95.;
   Double_t zz = -45.;

   aLand->AddNode(bvol,1,new TGeoCombiTrans(xx,yy,zz,rot0));

   TGeoRotation *rot1 = new TGeoRotation();
   rot1->RotateX(0.);
   rot1->RotateY(90.);
   rot1->RotateZ(90.);
   xx = 105.1;
   yy = -95.;
   zz = -45.;

   aLand->AddNode(trap,2,new TGeoCombiTrans(xx,yy,zz,rot1));
 

   TGeoRotation *rot2 = new TGeoRotation();
   rot2->RotateX(0.);
   rot2->RotateY(90.);
   rot2->RotateZ(270.);
   xx = -105.1;
   yy = -95.;
   zz = -45.;
   aLand->AddNode(trap,3,new TGeoCombiTrans(xx,yy,zz,rot2));


   
   AddSensitiveVolume(padle_h_box3); //Fe
   AddSensitiveVolume(padle_h_box4); //Fe
   AddSensitiveVolume(padle_h_box5); //Scint.

   fNbOfSensitiveVol+=3; //? FIXME


   //paddles
   TGeoRotation *rot3 = new TGeoRotation();
   rot3->RotateX(0.);
   rot3->RotateY(0.);
   rot3->RotateZ(0.);
   xx = 0.;
   yy = -95.;
   
   zz = -49.875;
   // normaly seq from 4
   aLand->AddNode(padle_h_box3,1,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -49.;
   aLand->AddNode(padle_h_box4,1,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -48.;
   aLand->AddNode(padle_h_box4,2,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -47.;
   aLand->AddNode(padle_h_box4,3,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -46.;
   aLand->AddNode(padle_h_box4,4,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -45.;
   aLand->AddNode(padle_h_box4,5,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -44.;
   aLand->AddNode(padle_h_box4,6,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -43.;
   aLand->AddNode(padle_h_box4,7,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.;
   aLand->AddNode(padle_h_box4,8,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -41.;
   aLand->AddNode(padle_h_box4,9,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -40.125;
   aLand->AddNode(padle_h_box3,2,new TGeoCombiTrans(xx,yy,zz,rot3));


   //bc408 paddles

   xx = 0.;
   yy = -95.;

   zz = -49.5;
   aLand->AddNode(padle_h_box5,1,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -48.5;
   aLand->AddNode(padle_h_box5,2,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -47.5;
   aLand->AddNode(padle_h_box5,3,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -46.5;
   aLand->AddNode(padle_h_box5,4,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -45.5;
   aLand->AddNode(padle_h_box5,5,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -44.5;
   aLand->AddNode(padle_h_box5,6,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -43.5;
   aLand->AddNode(padle_h_box5,7,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.5;
   aLand->AddNode(padle_h_box5,8,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -41.5;
   aLand->AddNode(padle_h_box5,9,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -40.5;
   aLand->AddNode(padle_h_box5,10,new TGeoCombiTrans(xx,yy,zz,rot3));


 //------------------------- Horizontal Assembly Multiplication & Rotation -----------------------------------------------------------------------  


   // cout << " -I- Assembly: aLand serial nb: " << aLand->GetNumber() << endl;
   // cout << " -I- now couting subnodes ... " << aLand->GetNdaughters() << endl;
   //aLand->CountNodes(1000,1);


  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  // cout << " -I- Assembly: cell serial nb: " << cell->GetNumber() << endl;


   TGeoRotation *rot4 = new TGeoRotation();
   rot4->RotateX(0.);
   rot4->RotateY(0.);
   rot4->RotateZ(0.);

   tx=0.;
   ty=0.;
   tz=0.;

 

 for ( Int_t i = 0; i < 5; i++ )
 {
  for( Int_t j = 0; j < 20; j++ )
  {
     tx=0.;
     ty=j*10.2;
     tz=i*20.;
     Int_t nindex = (j+20*i)+1; //1-100
     cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,rot4));
  }
 }


 //  cout << " -I- R3BLand Assembly: cell count nodes (1) "  <<   cell->GetNdaughters() << endl;
 //  cout << " -I- R3BLand total : " << cell->CountNodes(5000,0) << endl;


//------------------------- Vertical Assembly Multiplication & Rotation -----------------------------------------------------------------------  

   TGeoRotation *rot5 = new TGeoRotation();
   rot5->RotateX(0.);
   rot5->RotateY(0.);
   rot5->RotateZ(90.);

  for ( Int_t i = 0; i < 5; i++ )
 {
  for( Int_t j = 0; j < 20; j++ )
  {
     tx=j*10.2-192.;
     ty=1.9;
     tz=(2*i+1)*10.;
     Int_t nindex2 = (j+20*i)+101; //101 200
     cell->AddNode(aLand,nindex2,new TGeoCombiTrans(tx,ty,tz,rot5));
  }
 }

 //  cout << " -I- R3BLand  Assembly: cell count nodes (2) "  <<  cell->GetNdaughters() << endl;
 //  cout << " -I- R3BLand total " << cell->CountNodes(5000,0) << endl;



//
   TGeoRotation *rotg = new TGeoRotation();
   rotg->RotateX(0.);
   rotg->RotateY(0.);
   rotg->RotateZ(0.);
   tx = 0.0;
   ty = 0.0;
   tz = 1000.0; // cm

   TGeoCombiTrans *t0 = new TGeoCombiTrans(tx,ty,tz,rotg);

  vWorld->AddNode(cell,1, GetGlobalPosition(t0) );

}

void R3BLand::ConstructGeometry2() {   
  // This is the  defintion for RPC based Land
  // Detector
  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t a;
   Double_t z, density, w;
   Int_t nel, numed;
   Double_t radl,absl;

/****************************************************************************/
// Material definition
// User tracking media parameters   

    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 10.00000; // tmaxfd
    par[4]  = 0.100000; // stemax
    par[5]  = 0.050000; // deemax
    par[6]  = 0.001000; // epsil
    par[7]  = 0.001000; // stmin
   
//-- Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
    nel     = 2;
    density = 0.001290;
    TGeoMixture*
	pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
    pMat2->DefineElement(1,a,z,w);
    pMat2->SetIndex(1);
    // Medium: Air
    numed   = 22;  // medium number
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }

//--  Material: Iron
   TGeoMedium * pMedFe=NULL;
   if (gGeoManager->GetMedium("Iron") ){
       pMedFe=gGeoManager->GetMedium("Iron");
   }else{
    w       =        0.;
    a       = 55.850000;
    z       = 26.000000;
    density = 7.870000;
    radl    = 1.757717;
    absl    = 169.994418;
    TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
    pMatFe->SetIndex(701);
    numed   = 23;  // medium number
    pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
   }

//-- Mixture: PolyStyrene
    nel     = 2;
    density = 1.032;
    Double_t aps[2] = {12.011,1.008};
    Double_t zps[2] = {6.,1.};
    Double_t wps[2] = {1.,1.104};
    
    TGeoMixture*
    pMat_ps = new TGeoMixture("PS", nel,density);

    pMat_ps->DefineElement(0,aps[0],zps[0],wps[0]);
    pMat_ps->DefineElement(1,aps[1],zps[1],wps[1]);
    pMat_ps->SetIndex(24);
    // Medium: Air
    numed   = 24;  // medium number
    TGeoMedium*
    pMed_ps = new TGeoMedium("PS", numed,pMat_ps, par);

    

//-- Mixture: Quartz Glas
    nel     = 2;
    density = 2.5;
    Double_t aglas[2] = {16.,28.};
    Double_t zglas[2] = {8.,14.};
    Double_t wglas[2] = {2.,1.};
    
    TGeoMixture*
    pMat_glas = new TGeoMixture("QGLASS", nel,density);

    pMat_glas->DefineElement(0,aglas[0],zglas[0],wglas[0]);
    pMat_glas->DefineElement(1,aglas[1],zglas[1],wglas[1]);
    pMat_glas->SetIndex(24);
    // Medium: Quartz glas
    numed   = 25;  // medium number
    TGeoMedium*
    pMed_glas = new TGeoMedium("QGLAS", numed,pMat_glas, par);
    

//-- Geometry defintion

//-- Mixture: RPC Gas
    nel     = 4;
    density = 0.0053; // [g/cm3]
    Double_t agas[4] = {1.,12.,19.,32.};
    Double_t zgas[4] = {1.,6.,9.,16.};
    Double_t wgas[4] = {2.2,1.9,4.,0.85};
    
    TGeoMixture*
    pMat_gas = new TGeoMixture("RPCGAS", nel,density);

    pMat_gas->DefineElement(0,agas[0],zgas[0],wgas[0]);
    pMat_gas->DefineElement(1,agas[1],zgas[1],wgas[1]);
    pMat_gas->DefineElement(2,agas[2],zgas[2],wgas[2]);
    pMat_gas->DefineElement(3,agas[3],zgas[3],wgas[3]);
    pMat_gas->SetIndex(25);
    // Medium: RPCgas
    numed   = 26;  // medium number
    // More fine steping def. in Gas
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 10.00000; // tmaxfd
    par[4]  = 0.010000; // stemax
    par[5]  = 0.005000; // deemax
    par[6]  = 0.001000; // epsil
    par[7]  = 0.001000; // stmin
    
    TGeoMedium*
    pMed_gas = new TGeoMedium("RPCGAS", numed,pMat_gas, par);





//-- Main Geometry definition
//-- Get the top volume (CAVE) from the TGeoManager class
    
    TGeoVolume* vWorld = gGeoManager->GetTopVolume();
    vWorld->SetVisLeaves(kTRUE);
    
// -- Define paddle
    // Volume : GAS
    Double_t padx = 99.8;
    Double_t pady = 24.8;
    Double_t padz = 0.015;
    TGeoShape* pad_gas = new TGeoBBox("paddle_gas1",
				       padx,
				       pady,
				       padz);

    TGeoVolume*
       pGas = new TGeoVolume("GAS",pad_gas, pMed_gas);
    pGas->SetVisLeaves(kTRUE);

    // Add Gas as a sensitive volume
    AddSensitiveVolume(pGas);


    // Volume GLASS
    padx = 99.8;
    pady = 24.8;
    padz = 0.05;
    TGeoShape* pad_glas = new TGeoBBox("paddle_glas",
				       padx,
				       pady,
				       padz);
    TGeoVolume*
       pGlas = new TGeoVolume("GLAS",pad_glas, pMed_glas);
    pGlas->SetVisLeaves(kTRUE);

    // Volume : IRON
    padx = 100.0;
    pady = 25.0;
    padz = 1.02;
    TGeoShape* pad_mod1 = new TGeoBBox("paddle_mod1",
				       padx,
				       pady,
				       padz);    
  // Volume: MOD1
    TGeoVolume*
       pMod1 = new TGeoVolume("MOD1",pad_mod1, pMedFe);
    pMod1->SetVisLeaves(kTRUE);
    
    
    
    TGeoRotation *rot = new TGeoRotation();
    rot->RotateX(0.);
    rot->RotateY(0.);
    rot->RotateZ(0.);
    
    Double_t tx = 0.0;
    Double_t ty = 0.0;
    Double_t tz = 0.0;
    
  //- Create a Module 

    //- GAS sensitive part
    tz = 0.705;
    pMod1->AddNode(pGas,0, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.575;
    pMod1->AddNode(pGas,1, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.445;
    pMod1->AddNode(pGas,2, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.315;
    pMod1->AddNode(pGas,3, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.315;
    pMod1->AddNode(pGas,4, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.445;
    pMod1->AddNode(pGas,5, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.575;
    pMod1->AddNode(pGas,6, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.705;
    pMod1->AddNode(pGas,7, new TGeoCombiTrans(tx,ty,tz,rot));

    //- GLASS passive part 
    tz = 0.77;
    pMod1->AddNode(pGlas,8, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.640;
    pMod1->AddNode(pGlas,9, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.510; 
    pMod1->AddNode(pGlas,10, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.38;  
    pMod1->AddNode(pGlas,11, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.25; 
    pMod1->AddNode(pGlas,12, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.250;
    pMod1->AddNode(pGlas,13, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.380;
    pMod1->AddNode(pGlas,14, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.510; 
    pMod1->AddNode(pGlas,15, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.640;  
    pMod1->AddNode(pGlas,16, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.770; 
    pMod1->AddNode(pGlas,17, new TGeoCombiTrans(tx,ty,tz,rot));      

    
   // Full Geometry Definition
     Double_t posZ =1000.;

    if ( fVersion == 3 ){
       Double_t thx,thy,thz;
       Double_t phx,phy,phz;
       thx = 90.0;    phx = 270.0;
       thy = 90.0;    phy = 0.0;
       thz = 0.0;     phz = 0.0;
       TGeoRotation *pRot2 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
       
       
       for ( Int_t i=0;i<50;i+=2){
	  
	  for (Int_t j=0;j<4;j++){
	     tx = 0.0;
	     ty = -75.3+(j)*50.2;
	     tz = posZ +i*2.14;
	     vWorld->AddNode(pMod1,(i)*4+j, new TGeoCombiTrans(tx,ty,tz,rot) );
             ty = 0.0;
	     tx = -75.3+(j)*50.2;
	     tz = posZ +(i+1)*2.14;
	     vWorld->AddNode(pMod1,(i+1)*4+j, new TGeoCombiTrans(tx,ty,tz,pRot2) );
	  }
	  
       } 
       
    }//! version 3
    
    // Module Only 
    if ( fVersion == 2 ){
       tz = posZ;
       vWorld->AddNode(pMod1,0, new TGeoCombiTrans(tx,ty,tz,rot));             
    }
    
    
//!end of construction
}

ClassImp(R3BLand)
