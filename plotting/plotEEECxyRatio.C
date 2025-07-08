#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting ratios of E3C projected into XY plane. Currently only suited to plot ratios of track pT,
 * should be generalized in the future. 
 */
void plotEEECxyRatio(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================

  bool subtraction = false; // Does (track pt > 0.7) - (track pt > 4.0)

  TString dateFileString = "07082025";
  TString jetRadiusString = "0.8"; // Or 0.4

  // Bin vectors 
  std::vector<std::pair<double,double>> comparedCentralityBin;
  comparedCentralityBin.push_back(std::make_pair(-1,100));

  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));
  comparedJetPtBin.push_back(std::make_pair(140,160));
  comparedJetPtBin.push_back(std::make_pair(160,180));
  comparedJetPtBin.push_back(std::make_pair(180,200));
  
  std::vector<std::pair<double,double>> comparedRL; // Set (0.0, 0.0) if RL=All
  comparedRL.push_back(std::make_pair(0.0,0.0)); 
//  comparedRL.push_back(std::make_pair(0.3,0.4)); 
//  comparedRL.push_back(std::make_pair(0.4,0.5)); 
//  comparedRL.push_back(std::make_pair(0.5,0.6)); 
//  comparedRL.push_back(std::make_pair(0.6,0.7)); 
//  comparedRL.push_back(std::make_pair(0.7,0.8)); 

  std::vector<double> comparedTrackPtBin;
  comparedTrackPtBin.push_back(0.7);
//  comparedTrackPtBin.push_back(1.0);
//  comparedTrackPtBin.push_back(2.0);
//  comparedTrackPtBin.push_back(3.0);
  comparedTrackPtBin.push_back(4.0);

  // ====================================================
  // Loop through characteristics
  // ====================================================
 
 // Manual enumeration
 int i = 0;
 int j = 0;
 int k = 0;
 
   i = 0;
   for(auto RLBin : comparedRL){
     j = 0;
     for(auto jetPtBin : comparedJetPtBin){
        k = 0;
//	for(auto trackPtBin : comparedTrackPtBin){

  	  // Set allRL boolean
	  allRL = false; // Reset it first
  	  if(comparedRL.at(i).second == 0.0) {allRL = true;}
	 
	  // File from which the E3C are read
	  TString inputFileName = Form("jetRadius_%s/projected_xyPlane_RL-%.1f-%.1f_06302025.root", jetRadiusString.Data(), comparedRL.at(i).first, comparedRL.at(i).second);
	  
	  if(allRL) {inputFileName = Form("jetRadius_%s/projected_xyPlane_RL-All_06302025.root", jetRadiusString.Data());}
	  
	  // Open the input file
	  TFile* inputFile = TFile::Open(inputFileName);
	  
	  if(inputFile == NULL){
	    cout << "Error! The file " << inputFileName.Data() << " does not exist!" << endl;
	    cout << "Maybe you forgot the jetRadius_x/ folder path? Or added the incorrect date?" << endl;
	    cout << "Will not execute the code" << endl;
	    return;
	  }
	  
	  // Load the card from the file
	  EECCard* card = new EECCard(inputFile);
	  
	  // ====================================================
	  //  Initial configuration for the plotting
	  // ====================================================
	  
	  // Number of bins from the card
	  const int nCentralityBinsEEC = card->GetNCentralityBins();
	  const int nJetPtBinsEEC = card->GetNJetPtBinsEEC();
	  const int nTrackPtBinsEEC = card->GetNTrackPtBinsEEC();
	
	  // Create and setup a new histogram manager to project and handle the histograms
	  EECHistogramManager* histograms = new EECHistogramManager(inputFile,card); // Should already know what to do
	  
	  int iJetPt;
	  int iTrackPt;
	  int trackPtFirst;
	  int trackPtSecond;
	  int iCentrality = 0;
	  int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorFull;
	  int iPairingType = EECHistograms::kSameJetPair;
	  int iSubevent = EECHistograms::knSubeventCombinations;
	  
	  // Create two NULL histogram objects
	  TH2D* hEnergyEnergyEnergyCorrelatorFirst[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with one track pt bin
	  TH2D* hEnergyEnergyEnergyCorrelatorSecond[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with the other track pt bin
          TH1D* ratioHist;

	      for(int iJetPt = 0; iJetPt < nJetPtBinsEEC; iJetPt++){
	         for(int iTrackPt = 0; iTrackPt < nTrackPtBinsEEC; iTrackPt++){
	        hEnergyEnergyEnergyCorrelatorFirst[iJetPt][iTrackPt] = NULL;
	        hEnergyEnergyEnergyCorrelatorSecond[iJetPt][iTrackPt] = NULL;
	         } // Track pT
	      } // Jet pT

	// ====================================================
	//  Read in the E3C
	// ====================================================
        
	iJetPt = card->FindBinIndexJetPtEEC(jetPtBin);
        trackPtFirst = card->GetBinIndexTrackPtEEC(comparedTrackPtBin.at(0));
	iTrackPt = trackPtFirst;
        trackPtSecond = card->GetBinIndexTrackPtEEC(comparedTrackPtBin.at(1));

	hEnergyEnergyEnergyCorrelatorFirst[iJetPt][trackPtFirst] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, trackPtFirst, iPairingType, iSubevent);
	hEnergyEnergyEnergyCorrelatorSecond[iJetPt][trackPtSecond] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, trackPtSecond, iPairingType, iSubevent);


	int yLowBin = hEnergyEnergyEnergyCorrelatorFirst[iJetPt][trackPtFirst]->GetYaxis()->FindBin(0.0);
	int yHighBin = hEnergyEnergyEnergyCorrelatorFirst[iJetPt][trackPtFirst]->GetYaxis()->FindBin(0.2);

	// Project histograms
	TH1D* projEEECFirst = hEnergyEnergyEnergyCorrelatorFirst[iJetPt][trackPtFirst]->ProjectionX("projEEECFirst", yLowBin, yHighBin);
	TH1D* projEEECSecond = hEnergyEnergyEnergyCorrelatorSecond[iJetPt][trackPtSecond]->ProjectionX("projEEECSecond", yLowBin, yHighBin);

	// Normalize them
	double integralFirst = projEEECFirst->Integral("width");
	double integralSecond = projEEECSecond->Integral("width");
	projEEECFirst->Scale(1.0 / integralFirst);
	projEEECSecond->Scale(1.0 / integralSecond);

	// Calculate ratio
	ratioHist = (TH1D*) projEEECSecond->Clone("ratioHist");
	ratioHist->Divide(projEEECFirst);

      // ====================================================
      // Plot each E3C
      // ====================================================

	// Prepare a JDrawer for drawing purposes
	  JDrawer *drawer = new JDrawer();
	  if(logAxis) {drawer->SetLogZ(true);}
	  drawer->SetLeftMargin(0.16);
	  drawer->SetRightMargin(0.16);
	  drawer->SetTopMargin(0.13);
	  drawer->SetTitleOffsetX(1.2);
	  drawer->SetTitleOffsetY(1.2);
	  drawer->SetLabelSizeX(0.04);
	  drawer->SetLabelSizeY(0.04);
	  drawer->SetLabelSizeZ(0.03);
	  drawer->SetTitleSizeX(0.05);
	  drawer->SetTitleSizeY(0.05);
	  
	  TString jetPtFileString = Form("J=%.0f-%.0f", comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
	  TString RLFileString = Form("RL=%.1f-%.1f", comparedRL.at(i).first, comparedRL.at(i).second);
	 
	  if(allRL) {RLFileString = Form("RL=All");}
	  
	  TString trackPtFileString = Form("T=%.1f_%.1f", comparedTrackPtBin.at(1), comparedTrackPtBin.at(0));
	  TString ptTitleString = "";
	  TString commentFileString = "ratio";
	  if(subtract) {commentFileString = "subtraction"};
	  
	  TString axisFileString = "ratio";
	
	   ptTitleString = Form("R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);}
	  
	  // Draw the histogram to canvas
	drawer->DrawHistogram(ratioHist, "X", Form("#frac{Track p_{T} #geq %.1f GeV}{Track p_{T} #geq %.1f GeV}", comparedTrackPtBin.at(1), comparedTrackPtBin.at(0)), ptTitleString);
	
	gPad->Modified();
	gPad->Update();
	  
	  // Save figure
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/jetRadius_%s/%s/%s/%s_%s_%s_%s_%s.pdf", jetRadiusString.Data(), axisFileString.Data(), RLFileString.Data(), commentFileString.Data(),jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));

       if(k < comparedTrackPtBin.size() - 1) {k++;}
    //   } // Track pT
     if(j < comparedJetPtBin.size() - 1) {j++;}
     } // Jet pT
   if(i < comparedRL.size() - 1) {i++;}
   } // RL

 } // is2d

} // End script
