#include "R3BNeuLandDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TH2F.h"
#include <string>
#include <iostream>


//#include "R3BLandPoint.h"
#include "R3BNeuLandPoint.h"
#include "R3BMCTrack.h"

		
using std::cout;
using std::endl;

		

R3BNeuLandDigitizer::R3BNeuLandDigitizer() :
  FairTask("R3BNeu Land Digitization scheme ") { 
}


R3BNeuLandDigitizer::~R3BNeuLandDigitizer() {
}


InitStatus R3BNeuLandDigitizer::Init() {
  
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("NeuLandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
  // Register output array LandDigi
  //fDigis = new TClonesArray("R3BNeuLandDigi",1000);
  //ioman->Register("LandDigi", "Digital response in Land", fDigis, kTRUE);
  
  // Initialise control histograms
  TString elossNamesP[6] = {"Eloss-p 1", "Eloss-p 2", "Eloss-p 3",
			    "Eloss-p 4", "Eloss-p 5", "Eloss-p 6"};
  TString elossNamesE[6] = {"Eloss-e 1", "Eloss-e 2", "Eloss-e 3",
			    "Eloss-e 4", "Eloss-e 5", "Eloss-e 6"};
  TString PxOutNamesP[6] = {"PxOut-p 1", "PxOut-p 2", "PxOut-p 3",
			    "PxOut-p 4", "PxOut-p 5", "PxOut-p 6"};
  TString PyOutNamesP[6] = {"PyOut-p 1", "PyOut-p 2", "PyOut-p 3",
			    "PyOut-p 4", "PyOut-p 5", "PyOut-p 6"};
  TString PzOutNamesP[6] = {"PzOut-p 1", "PzOut-p 2", "PzOut-p 3",
			    "PzOut-p 4", "PzOut-p 5", "PzOut-p 6"};
  TString xInNamesP[6] = {"xIn-p 1", "xIn-p 2", "xIn-p 3",
			  "xIn-p 4", "xIn-p 5", "xIn-p 6"};
  TString yInNamesP[6] = {"yIn-p 1", "yIn-p 2", "yIn-p 3",
			  "yIn-p 4", "yIn-p 5", "yIn-p 6"};
  TString zInNamesP[6] = {"zIn-p 1", "zIn-p 2", "zIn-p 3",
			  "zIn-p 4", "zIn-p 5", "zIn-p 6"};
  TString momNamesP[6] = {"Momentum-p 1", "Momentum-p 2", "Momentum-p 3",
			  "Momentum-p 4", "Momentum-p 5", "Momentum-p 6"};
  TString etotNamesP[6] = {"Total energy-p 1", "Total energy-p 2", "Total energy-p 3",
			   "Total energy-p 4", "Total energy-p 5", "Total energy-p 6"};
  TString etotNamesE[6] = {"Total energy-e 1", "Total energy-e 2", "Total energy-e 3",
			   "Total energy-e 4", "Total energy-e 5", "Total energy-e 6"};
  TString etot_elossNamesP[6] = {"Total energy-p vs Eloss-p 1", "Total energy-p vs Eloss-p 2",
				 "Total energy-p vs Eloss-p 3", "Total energy-p vs Eloss-p 4",
				 "Total energy-p vs Eloss-p 5", "Total energy-p vs Eloss-p 6"};
  TString etot_elossNamesE[6] = {"Total energy-e vs Eloss-e 1", "Total energy-e vs Eloss-e 2",
				 "Total energy-e vs Eloss-e 3", "Total energy-e vs Eloss-e 4",
				 "Total energy-e vs Eloss-e 5", "Total energy-e vs Eloss-e 6"};

  TString elossTitlesP[6] = {"Eloss-p in gas [keV] 1",
			     "Eloss-p in gas [keV] 2",
			     "Eloss-p in gas [keV] 3",
			     "Eloss-p in gas [keV] 4",
			     "Eloss-p in gas [keV] 5",
			     "Eloss-p in gas [keV] 6"};
  TString elossTitlesE[6] = {"Eloss-e in gas [keV] 1",
			     "Eloss-e in gas [keV] 2",
			     "Eloss-e in gas [keV] 3",
			     "Eloss-e in gas [keV] 4",
			     "Eloss-e in gas [keV] 5",
			     "Eloss-e in gas [keV] 6"};
  TString PxOutTitlesP[6] = {"PxOut-p in gas [MeV/c] 1",
			     "PxOut-p in gas [MeV/c] 2",
			     "PxOut-p in gas [MeV/c] 3",
			     "PxOut-p in gas [MeV/c] 4",
			     "PxOut-p in gas [MeV/c] 5",
			     "PxOut-p in gas [MeV/c] 6"};
  TString PyOutTitlesP[6] = {"PyOut-p in gas [MeV/c] 1",
			     "PyOut-p in gas [MeV/c] 2",
			     "PyOut-p in gas [MeV/c] 3",
			     "PyOut-p in gas [MeV/c] 4",
			     "PyOut-p in gas [MeV/c] 5",
			     "PyOut-p in gas [MeV/c] 6"};
  TString PzOutTitlesP[6] = {"PzOut-p in gas [MeV/c] 1",
			     "PzOut-p in gas [MeV/c] 2",
			     "PzOut-p in gas [MeV/c] 3",
			     "PzOut-p in gas [MeV/c] 4",
			     "PzOut-p in gas [MeV/c] 5",
			     "PzOut-p in gas [MeV/c] 6"};
  TString xInTitlesP[6] = {"xIn-p in gas [mm] 1",
			   "xIn-p in gas [mm] 2",
			   "xIn-p in gas [mm] 3",
			   "xIn-p in gas [mm] 4",
			   "xIn-p in gas [mm] 5",
			   "xIn-p in gas [mm] 6"};
  TString yInTitlesP[6] = {"yIn-p in gas [mm] 1",
			   "yIn-p in gas [mm] 2",
			   "yIn-p in gas [mm] 3",
			   "yIn-p in gas [mm] 4",
			   "yIn-p in gas [mm] 5",
			   "yIn-p in gas [mm] 6"};
  TString zInTitlesP[6] = {"zIn-p in gas [mm] 1",
			   "zIn-p in gas [mm] 2",
			   "zIn-p in gas [mm] 3",
			   "zIn-p in gas [mm] 4",
			   "zIn-p in gas [mm] 5",
			   "zIn-p in gas [mm] 6"};
  TString momTitlesP[6] = {"Momentum-p in gas [MeV/c] 1",
			   "Momentum-p in gas [MeV/c] 2",
			   "Momentum-p in gas [MeV/c] 3",
			   "Momentum-p in gas [MeV/c] 4",
			   "Momentum-p in gas [MeV/c] 5",
			   "Momentum-p in gas [MeV/c] 6"};
  TString etotTitlesP[6] = {"Total energy-p in gas [MeV] 1",
			    "Total energy-p in gas [MeV] 2",
			    "Total energy-p in gas [MeV] 3",
			    "Total energy-p in gas [MeV] 4",
			    "Total energy-p in gas [MeV] 5",
			    "Total energy-p in gas [MeV] 6"};
  TString etotTitlesE[6] = {"Total energy-e in gas [keV] 1",
			    "Total energy-e in gas [keV] 2",
			    "Total energy-e in gas [keV] 3",
			    "Total energy-e in gas [keV] 4",
			    "Total energy-e in gas [keV] 5",
			    "Total energy-e in gas [keV] 6"};
  TString etot_elossTitlesP[6] = {"Total energy-p [MeV] vs Eloss [keV] in gas 1",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 2",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 3",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 4",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 5",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 6"};
  TString etot_elossTitlesE[6] = {"Total energy-e [keV] vs Eloss [keV] in gas 1",
				  "Total energy-e [keV] vs Eloss [keV] in gas 2",
				  "Total energy-e [keV] vs Eloss [keV] in gas 3",
				  "Total energy-e [keV] vs Eloss [keV] in gas 4",
				  "Total energy-e [keV] vs Eloss [keV] in gas 5",
				  "Total energy-e [keV] vs Eloss [keV] in gas 6"};

  for(Int_t i=0;i<6;i++){
    h_eloss_p[i] = new TH1F(elossNamesP[i], elossTitlesP[i] , 3110 , -10.5 , 300.5);
    h_eloss_e[i] = new TH1F(elossNamesE[i], elossTitlesE[i] , 3110 , -10.5 , 300.5);
    h_xin_p[i] = new TH1F(xInNamesP[i], xInTitlesP[i] , 210 , -10.5 , 10.5);
    h_yin_p[i] = new TH1F(yInNamesP[i], yInTitlesP[i] , 210 , -10.5 , 10.5);
    h_zin_p[i] = new TH1F(zInNamesP[i], zInTitlesP[i] , 160 , -0.5 , 15.5);
    h_pxout_p[i] = new TH1F(PxOutNamesP[i], PxOutTitlesP[i] , 1001 , -500.5 , 500.5);
    h_pyout_p[i] = new TH1F(PyOutNamesP[i], PyOutTitlesP[i] , 1001 , -500.5 , 500.5);
    h_pzout_p[i] = new TH1F(PzOutNamesP[i], PzOutTitlesP[i] , 1101 , -100.5 , 1000.5);
    h_mom_p[i] = new TH1F(momNamesP[i], momTitlesP[i] , 1051 , -50.5 , 1000.5);
    h_etot_p[i] = new TH1F(etotNamesP[i], etotTitlesP[i] , 1101 , 899.5 , 2000.5);
    h_etot_e[i] = new TH1F(etotNamesE[i], etotTitlesE[i] , 1501 , 499.5 , 2000.5);
    h_etot_eloss_p[i] = new TH2F(etot_elossNamesP[i], etot_elossTitlesP[i] , 1101 , 899.5 , 2000.5, 3110 , -10.5 , 300.5);
    h_etot_eloss_e[i] = new TH2F(etot_elossNamesE[i], etot_elossTitlesE[i] , 1501 , 499.5 , 2000.5, 3110 , -10.5 , 300.5);
  }


  h_pdg = new TH1F("PDG","Code of particle deposited energy in gas",3000,-220.,3000.);
  h_detid = new TH1F("DetId","DetId", 10 , 95.5, 105.5);
  h_segid = new TH1F("SegId","SegId", 10 , 7.5, 17.5);
  h_cellid = new TH1F("CellId","CellId", 10 , -5.5, 4.5);
  h_ne = new TH1F("Ne","primary_el",100,0.,100.);
  h_ch = new TH1F("Charge","",1000,0.,1.);
  h_time = new TH1F("Time [ns]","",100,0.,1000.);
  h_energy = new TH1F("Total energy [MeV]","",100000,0.,4000.);
  h_zpos_e = new TH1F("Zpos electron [mm]","",5000,0.,15.);
  h_zpos_p = new TH1F("Zpos proton [mm]","",5000,0.,15.);
  h_xpos = new TH1F("Xpos [mm]","",100,-50.,50.);
  h_ypos = new TH1F("Ypos [mm]","",100,-50.,50.);
  h_xpos1_vs_ypos1 = new TH2F("Xpos1 [mm] vs Ypos1 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos2_vs_ypos2 = new TH2F("Xpos2 [mm] vs Ypos2 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos3_vs_ypos3 = new TH2F("Xpos3 [mm] vs Ypos3 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos4_vs_ypos4 = new TH2F("Xpos4 [mm] vs Ypos4 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos5_vs_ypos5 = new TH2F("Xpos5 [mm] vs Ypos5 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos6_vs_ypos6 = new TH2F("Xpos6 [mm] vs Ypos6 [mm]","",100,-50.,50.,100,-50.,50.);
  h_pdg_vs_eloss = new TH2F("PDG vs Eloss [keV]","",100,-220.,3000.,1000,0.,50.);
  h_zpos_vs_pdg = new TH2F("Zpos [mm] vs PDG","",100,-1.,30.,100,-220.,3000.);
  h_zpos_vs_eloss = new TH2F("Zpos [mm] vs Eloss [keV]","",100,-1.,30.,100,0.,50.);
  h_zpos_vs_time = new TH2F("Zpos [mm] vs Time [ns]","",100,-1.,30.,100,0.,1000.);
  h_pdg_vs_m0pdg = new TH2F("PDG vs m0PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_pdg_vs_m1pdg = new TH2F("PDG vs m1PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_pdg_vs_m2pdg = new TH2F("PDG vs m2PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_pdg_vs_m3pdg = new TH2F("PDG vs m3PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_energy_vs_eloss = new TH2F("Total energy [MeV] vs Eloss [keV]","",100,0.,4000.,100,0.,50.);
  h_energy_vs_pdg = new TH2F("Total energy [MeV] vs PDG [keV]","",10000,0.,4000.,100,-220.,3000.);
  h_energy_vs_zpos = new TH2F("Total energy [MeV] vs Zpos [mm]","",10000,0.,4000.,1000,-1.,30.);
  h_trackid_vs_m0trackid = new TH2F("TrackId vs m0TrackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);
  h_trackid_vs_m1trackid = new TH2F("TrackId vs m1TrackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);
  h_trackid_vs_m2trackid = new TH2F("TrackId vs m2TrackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);
  h_trackid_vs_m3trackid = new TH2F("TrackId vs m3trackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);

  
  return kSUCCESS;

}

void R3BNeuLandDigitizer::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
  
  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");
  
  
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BNeuLandDigitizer::Exec(Option_t* opt) {
  
  //-Reset entries in output arrays
  //-Reset local arrays 
  Reset();
  
  // Create Stochastic Avalanche
  
  //-Now do the job event/event
  //Int_t no_interaction = 0;
  Int_t nentries = fLandPoints->GetEntries();
  if(nentries==0){
    no_interaction = no_interaction + 1;
  }

  for (Int_t l=0;l<nentries;l++){
    // Get the Land Object in array
    R3BNeuLandPoint *land_obj = (R3BNeuLandPoint*) fLandPoints->At(l);
    Int_t detId = land_obj->GetDetID();
    Int_t segId = land_obj->GetSegID();
    Int_t cellId = land_obj->GetCellID();
    //cout << " detID: " << detId << " segID: " << segId << " cellID: " << cellId << endl;

    Int_t TrackId = land_obj->GetTrackID();
    Int_t Mot0TrackId = land_obj->GetMot0TrackID();
    Int_t Mot1TrackId = land_obj->GetMot1TrackID();
    Int_t Mot2TrackId = land_obj->GetMot2TrackID();
    Int_t Mot3TrackId = land_obj->GetMot3TrackID();
    //cout << " TrackID: " << TrackId << endl;
    //cout << " Mot0TrackID: " << Mot0TrackId << endl;
    //cout << " Mot1TrackID: " << Mot1TrackId << endl;
    //cout << " Mot2TrackID: " << Mot2TrackId << endl;
    //cout << " Mot3TrackID: " << Mot3TrackId << endl;

    R3BMCTrack *aTrack = (R3BMCTrack*) fLandMCTrack->At(TrackId);
    Int_t PID = aTrack->GetPdgCode();
    Double_t eloss = land_obj->GetEnergyLoss();
    Double_t energy = aTrack->GetEnergy();
    if(PID == 2212) energy = energy * 1E3; //MeV
    if(PID == 11) energy = energy * 1E6; //keV
    Double_t startx = aTrack->GetStartX() * 10;
    Double_t starty = aTrack->GetStartY() * 10;
    Double_t startz = aTrack->GetStartZ() * 10;
    Double_t xIn = land_obj->GetXIn() * 10;
    Double_t yIn = land_obj->GetYIn() * 10;
    Double_t zIn = land_obj->GetZIn() * 10;
    Double_t zOut = land_obj->GetZOut() * 10;
    Double_t xPos = land_obj->GetX(zIn * 0.1) * 10;
    Double_t yPos = land_obj->GetY(zIn * 0.1) * 10;
    Double_t time = land_obj->GetTime();
    Double_t PxOut = land_obj->GetPxOut() * 1E3; // MeV/c
    Double_t PyOut = land_obj->GetPyOut() * 1E3; // MeV/c
    Double_t PzOut = land_obj->GetPzOut() * 1E3; // MeV/c

    Double_t momentum = sqrt( PxOut * PxOut + PyOut * PyOut + PzOut * PzOut);


    if(Mot0TrackId>-1){
      R3BMCTrack *aMot0Track = (R3BMCTrack*) fLandMCTrack->At(Mot0TrackId);
      mot0PID = aMot0Track->GetPdgCode();
      //cout << " mot0PID: " << mot0PID << endl;
    }
    else{
      Mot0TrackId=Mot1TrackId=Mot2TrackId=Mot3TrackId=mot0PID=mot1PID=mot2PID=mot3PID=-1;
      //cout << " mot1PID: " << mot1PID << endl;
      //cout << " mot2PID: " << mot2PID << endl;
      //cout << " mot3PID: " << mot3PID << endl;
    }
    if(Mot1TrackId>-1){
      R3BMCTrack *aMot1Track = (R3BMCTrack*) fLandMCTrack->At(Mot1TrackId);
      mot1PID = aMot1Track->GetPdgCode();
      //cout << " mot1PID: " << mot1PID << endl;
    }
    else{
      Mot1TrackId=Mot2TrackId=Mot3TrackId=mot1PID=mot2PID=mot3PID=-1;
    }
    if(Mot2TrackId>-1){
      R3BMCTrack *aMot2Track = (R3BMCTrack*) fLandMCTrack->At(Mot2TrackId);
      mot2PID = aMot2Track->GetPdgCode();
      //cout << " mot2PID: " << mot2PID << endl;
    }
    else{
      Mot2TrackId=Mot3TrackId=mot2PID=mot3PID=-1;
    }
    if(Mot3TrackId>-1){
      R3BMCTrack *aMot3Track = (R3BMCTrack*) fLandMCTrack->At(Mot3TrackId);
      mot3PID = aMot3Track->GetPdgCode();
      //cout << " mot3PID: " << mot3PID << endl;
      cout << "error: 3rd mother is not a primary. " << endl;
    }
    else{
      Mot3TrackId=mot3PID=-1;
    }

    
    //    if (eloss > 0.0 ) {
	switch(segId){
	case 11:
	  switch(cellId){
	  case 1:
	    if (PID == 2212 ){
	      h_eloss_p[0]->Fill(eloss);
	      h_pxout_p[0]->Fill(PxOut);
	      h_pyout_p[0]->Fill(PyOut);
	      h_pzout_p[0]->Fill(PzOut);
	      h_xin_p[0]->Fill(xIn);
	      h_yin_p[0]->Fill(yIn);
	      h_zin_p[0]->Fill(zIn);
	      h_mom_p[0]->Fill(momentum);
	      h_etot_p[0]->Fill(energy);
	      h_etot_eloss_p[0]->Fill(energy,eloss);
	      //cout << " PxOut: " << PxOut << " PyOut: " << PyOut << " PzOut: " << PzOut << endl;
	      //cout << " startx: " << startx << " starty: " << starty
	      //	   << " xIn: " << xIn << " yIn: " << yIn
	      //	   << " eloss: " << eloss << " startz: " << startz
	      //     	   << " zIn: " << zIn << " zOut: " << zOut << endl;
	    }
	    if (PID == 11 ){
	      h_eloss_e[0]->Fill(eloss);
	      h_etot_e[0]->Fill(energy);
	      h_etot_eloss_e[0]->Fill(energy,eloss);
	    }
	    break; // case 1
	  case 2:
	    if (PID == 2212 ){
	      h_eloss_p[1]->Fill(eloss);
	      h_pxout_p[1]->Fill(PxOut);
	      h_pyout_p[1]->Fill(PyOut);
	      h_pzout_p[1]->Fill(PzOut);
	      h_xin_p[1]->Fill(xIn);
	      h_yin_p[1]->Fill(yIn);
	      h_zin_p[1]->Fill(zIn);
	      h_mom_p[1]->Fill(momentum);
	      h_etot_p[1]->Fill(energy);
	      h_etot_eloss_p[1]->Fill(energy,eloss);
	      //cout << " startx: " << startx << " starty: " << starty
	      //	   << " xIn: " << xIn << " yIn: " << yIn
	      //   	   << " eloss: " << eloss << " startz: " << startz
	      //    	   << " zIn: " << zIn << " zOut: " << zOut << endl;
	    }
	    if (PID == 11 ){
	      h_eloss_e[1]->Fill(eloss);
	      h_etot_e[1]->Fill(energy);
	      h_etot_eloss_e[1]->Fill(energy,eloss);
	    }
	    break; // case 2
	  case 3:
	    if (PID == 2212 ){
	      h_eloss_p[2]->Fill(eloss);
	      h_pxout_p[2]->Fill(PxOut);
	      h_pyout_p[2]->Fill(PyOut);
	      h_pzout_p[2]->Fill(PzOut);
	      h_xin_p[2]->Fill(xIn);
	      h_yin_p[2]->Fill(yIn);
	      h_zin_p[2]->Fill(zIn);
	      h_mom_p[2]->Fill(momentum);
	      h_etot_p[2]->Fill(energy);
	      h_etot_eloss_p[2]->Fill(energy,eloss);
	    //cout << " startx: " << startx << " starty: " << starty
	    //    	   << " xIn: " << xIn << " yIn: " << yIn
	    //	      	   << " eloss: " << eloss << " startz: " << startz
	    //     	   << " zIn: " << zIn << " zOut: " << zOut << endl;
	    }
	    if (PID == 11 ){
	      h_eloss_e[2]->Fill(eloss);
	      h_etot_e[2]->Fill(energy);
	      h_etot_eloss_e[2]->Fill(energy,eloss);
	    }
	    break; // case 3
	  } // cellId
	  break; // case 11
	case 12:
	  switch(cellId){
	  case 3:
	    if (PID == 2212 ){
	      h_eloss_p[3]->Fill(eloss);
	      h_pxout_p[3]->Fill(PxOut);
	      h_pyout_p[3]->Fill(PyOut);
	      h_pzout_p[3]->Fill(PzOut);
	      h_xin_p[3]->Fill(xIn);
	      h_yin_p[3]->Fill(yIn);
	      h_zin_p[3]->Fill(zIn);
	      h_mom_p[3]->Fill(momentum);
	      h_etot_p[3]->Fill(energy);
	      h_etot_eloss_p[3]->Fill(energy,eloss);
	      //cout << " PxOut: " << PxOut << " PyOut: " << PyOut << " PzOut: " << PzOut << endl;
	      //cout << " startx: " << startx << " starty: " << starty
	      //	   << " xIn: " << xIn << " yIn: " << yIn
	      //	   << " eloss: " << eloss << " startz: " << startz
	      //	   << " zIn: " << zIn << " zOut: " << zOut << endl;
	    }
	    if (PID == 11 ){
	      h_eloss_e[3]->Fill(eloss);
	      h_etot_e[3]->Fill(energy);
	      h_etot_eloss_e[3]->Fill(energy,eloss);
	    }
	    break; // case 3
	  case 2:
	    if (PID == 2212 ){
	      h_eloss_p[4]->Fill(eloss);
	      h_pxout_p[4]->Fill(PxOut);
	      h_pyout_p[4]->Fill(PyOut);
	      h_pzout_p[4]->Fill(PzOut);
	      h_xin_p[4]->Fill(xIn);
	      h_yin_p[4]->Fill(yIn);
	      h_zin_p[4]->Fill(zIn);
	      h_mom_p[4]->Fill(momentum);
	      h_etot_p[4]->Fill(energy);
	      h_etot_eloss_p[4]->Fill(energy,eloss);
	      //cout << " eloss: " << eloss << " startz: " << startz
	      //    	   << " zIn: " << zIn << " zOut: " << zOut << endl;
	    }
	    if (PID == 11 ){
	      h_eloss_e[4]->Fill(eloss);
	      h_etot_e[4]->Fill(energy);
	      h_etot_eloss_e[4]->Fill(energy,eloss);
	    }
	    break; // case 2
	  case 1:
	    if (PID == 2212 ){
	      h_eloss_p[5]->Fill(eloss);
	      h_pxout_p[5]->Fill(PxOut);
	      h_pyout_p[5]->Fill(PyOut);
	      h_pzout_p[5]->Fill(PzOut);
	      h_xin_p[5]->Fill(xIn);
	      h_yin_p[5]->Fill(yIn);
	      h_zin_p[5]->Fill(zIn);
	      h_mom_p[5]->Fill(momentum);
	      h_etot_p[5]->Fill(energy);
	      h_etot_eloss_p[5]->Fill(energy,eloss);
	      //cout << " eloss: " << eloss << " startz: " << startz
	      //   	   << " zIn: " << zIn << " zOut: " << zOut << endl;
	    }
	    if (PID == 11 ){
	      h_eloss_e[5]->Fill(eloss);
	      h_etot_e[5]->Fill(energy);
	      h_etot_eloss_e[5]->Fill(energy,eloss);
	    }
	    break; // case 1
	  } // cellId
	  break; // case 12
	} // segId
	
      h_pdg->Fill(PID);
      h_detid->Fill(detId);
      h_segid->Fill(segId);
      h_cellid->Fill(cellId);
      h_time->Fill(time);
      h_xpos->Fill( xPos );
      h_ypos->Fill( yPos );
      h_zpos_e->Fill( zIn );
      h_zpos_p->Fill( zIn );
      //      if(startz == zIn){
      //      if(PID == 11) h_zpos_e->Fill( zIn );
      //      if(PID == 2212) h_zpos_p->Fill( zIn );
      //      }
      h_energy_vs_zpos->Fill(energy, zIn );
      h_xpos1_vs_ypos1->Fill(xPos , yPos );
      h_xpos2_vs_ypos2->Fill(xPos , yPos );
      h_xpos3_vs_ypos3->Fill(xPos , yPos );
      h_xpos4_vs_ypos4->Fill(xPos , yPos );
      h_xpos5_vs_ypos5->Fill(xPos , yPos );
      h_xpos6_vs_ypos6->Fill(xPos , yPos );
      h_pdg_vs_eloss->Fill(PID , eloss ); // keV
      h_zpos_vs_pdg->Fill(zIn, PID);
      h_zpos_vs_eloss->Fill(zIn, eloss);
      h_zpos_vs_time->Fill(zIn, time);
      h_pdg_vs_m0pdg->Fill(PID, mot0PID);
      h_pdg_vs_m1pdg->Fill(PID, mot1PID);
      h_pdg_vs_m2pdg->Fill(PID, mot2PID);
      h_pdg_vs_m3pdg->Fill(PID, mot3PID);
      h_trackid_vs_m0trackid->Fill(TrackId, Mot0TrackId);
      h_trackid_vs_m1trackid->Fill(TrackId, Mot1TrackId);
      h_trackid_vs_m2trackid->Fill(TrackId, Mot2TrackId);
      h_trackid_vs_m3trackid->Fill(TrackId, Mot3TrackId);
      h_energy->Fill( energy ); // MeV
      h_energy_vs_eloss->Fill(energy , eloss );
      h_energy_vs_pdg->Fill(energy , PID);
      

      
      /*
      Double_t ne = 0.0;
      if ( tof[paddle] < 1.e-15 ){
	tof[paddle] = land_obj->GetTime(); // time since part. start [ns] 
	//cout << "-I- LANDOBJ tof " << tof[paddle] << endl;  
      }//! tof > 1e-15
      
       // Check the Units of Tof in [ns] here    
      if ( ( land_obj->GetTime()-tof[paddle] )*1e+9 < 100. ){
	//ne = eloss * 1e+9 / 25.; 
	ne = eloss * 1e+3 / 25.; 
	// cout << "-I- LANDOBJ Nb of el: " << ne << endl;
	
	h_ne ->Fill( ne );
	// Check the Ne ??  
	Int_t ine = (Int_t) (ne+0.5);
	
	// Loop over primary electrons
	for (Int_t i=0;i<ine;i++ ){
	  Double_t zz = gRandom->Rndm();     
	  Double_t xx = zz * 0.3;
	  Double_t dd =0.3;       // [mm]
	  
	  Double_t vv = 0.21e-3;  // [mm/ps]
	  Double_t nsteps = xx/0.0025; // mm
	  Double_t dt = 0.0025 / vv ;   // ps
	  Double_t tt = dt*nsteps; 
	  //  cout << "-I LandOBJ tt: " <<  tt << endl;
	  Double_t alpha = 123.0;
	  Double_t eta = 10.5;
	  Double_t wfield = 0.5;
	  
	  
	  Double_t qq2=0.0;
	  Double_t f1=0.0;
	  Double_t f2=0.0;
	  
	  Int_t ix = (int) (nsteps);
      */
	  // Add on Avan
	  /*   for (Int_t j=1;j<ix+1;j++){
	       Double_t nbar = TMath::Exp((alpha-eta)*j*0.0025);
	       Double_t kk = eta/alpha;
	       zz = gRandom->Rndm();
	       Double_t avan =0.0;
	       if ( zz < (kk*(nbar-1.)/(nbar-kk))) avan =0.0;
	       else {
	       f1 = TMath::Log(1.-(1.-kk)/(nbar-kk));
	       f2=  TMath::Log((nbar-kk)*(1.-zz)/nbar/(1.-kk));
	       avan =1.+ f2/f1;
	       }
	       //cout << "-I- LandOBJ f1:" << f1 << " f2: " << f2 << endl;
	       // QDC Saturation   
	       if (avan > 1.6e+7 ) avan = 1.6e+7;
	       //cout << "-I- LandOBJ Avalanche:" << j << " avan: " << avan << endl;
	       Double_t current = wfield*vv*1.6022e-19*avan;
	       //cout << "-I- LandOBJ current:" << current << endl;
	       qq2=qq2+current*dt;
	       }// !j
	  */
	  /*
	  qq2= 0.5 / (alpha-eta)*1.6022e-19* TMath::Exp((alpha-eta)*(0.3-xx)-1)*1.e+12;
	  Double_t qcharge = qq2 ;
	  // Fill Charge Control hist.
	  h_ch->Fill(qcharge);
	  
	  //??? The processHit has to be changed and Hit Info also !!!
	  x_pos[paddle]  = x_pos[paddle] + land_obj->GetXOut() *qcharge;
	  y_pos[paddle]  = y_pos[paddle] + land_obj->GetYOut() *qcharge;
	  z_pos[paddle]  = z_pos[paddle] + land_obj->GetZOut() *qcharge;
	  nuhits[paddle] = nuhits[paddle] +1;
	  
	  paddle_E[paddle][gap] = paddle_E[paddle][gap] + qcharge;
	  paddle_E[paddle][9] = paddle_E[paddle][9] + qcharge;
	  
	  Int_t first =  (Int_t) ((paddle/20.)/2.);
	  Int_t second = (Int_t) (paddle/20./2.);
	  
	  // ?? here what is the diff ??
	  if(first==second){
	    pm[paddle][1] = pm[paddle][1]+qcharge;
	    pm[paddle][2] = pm[paddle][2]+qcharge;
	    
	  }else{
	    
	    pm[paddle][1] = pm[paddle][1]+qcharge;
	    pm[paddle][2] = pm[paddle][2]+qcharge;
	    
	  }
	  
	  if (paddle_E[paddle][gap]>2.5) continue;
	  
	}//! ine
	}//!tof */
      //}//! eloss	 
    
  }//! MC hits   
  
}
// -------------------------------------------------------------------------

void R3BNeuLandDigitizer::Reset()
{
  // Clear the structure
  //   cout << " -I- Digit Reset() called " << endl;
  
  for(Int_t i=0;i<200;i++) {
    nuhits[i]=0;
    x_pos[i]=y_pos[i]=z_pos[i]=0.0;
    tof[i]=0.0;
    
    for (Int_t j=0;j<2;j++){
      pm[i][j]=0.0;
    }
    for (Int_t k=0;k<9;k++){
      paddle_E[i][k]=0.0;
    }
    
  } 
  
  for(Int_t i=0;i<50;i++) {
    part_E[i] = part_n[i] = 0.0;      
  }
  
  for(Int_t i=0;i<4;i++) {
    first_hit[i] = 0.0;      
  }   
  
  // if (fDigis ) fDigis->Clear();
}   

void R3BNeuLandDigitizer::Finish()
{
  // here event. write histos
  //   cout << " -I- Digit Finish() called " << endl;
  // Write control histograms
  for(Int_t i=0;i<6;i++){
    h_eloss_p[i]->Write();
    h_pxout_p[i]->Write();
    h_pyout_p[i]->Write();
    h_pzout_p[i]->Write();
    h_xin_p[i]->Write();
    h_yin_p[i]->Write();
    h_zin_p[i]->Write();
    h_mom_p[i]->Write();
    h_etot_p[i]->Write();
    h_etot_eloss_p[i]->Write();

    h_eloss_e[i]->Write();
    h_etot_e[i]->Write();
    h_etot_eloss_e[i]->Write();
  }
  h_ne->Write();
  h_ch->Write();
  h_pdg->Write();
  h_detid->Write();
  h_segid->Write();
  h_cellid->Write();
  h_time->Write();
  h_energy->Write();
  h_xpos->Write();
  h_ypos->Write();
  h_xpos1_vs_ypos1->Write();
  h_xpos2_vs_ypos2->Write();
  h_xpos3_vs_ypos3->Write();
  h_xpos4_vs_ypos4->Write();
  h_xpos5_vs_ypos5->Write();
  h_xpos6_vs_ypos6->Write();
  h_zpos_e->Write();
  h_zpos_p->Write();
  h_pdg_vs_eloss->Write();
  h_zpos_vs_pdg->Write();
  h_zpos_vs_eloss->Write();
  h_zpos_vs_time->Write();
  h_pdg_vs_m0pdg->Write();
  h_pdg_vs_m1pdg->Write();
  h_pdg_vs_m2pdg->Write();
  h_pdg_vs_m3pdg->Write();
  h_energy_vs_eloss->Write();
  h_energy_vs_pdg->Write();
  h_energy_vs_zpos->Write();
  h_trackid_vs_m0trackid->Write();
  h_trackid_vs_m1trackid->Write();
  h_trackid_vs_m2trackid->Write();
  h_trackid_vs_m3trackid->Write();

  cout << " Number of non-interacting primaries: " << no_interaction << endl;
}

ClassImp(R3BNeuLandDigitizer)
