#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting distribution of E3C, projected into xy coordinates. 
 */
void plotEEECxyPlane(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================
  
  bool is2d; // Helper to loop through 2d and 3d plots
  bool allRL = false; // To set this as true, comparedRL = (0.0, 0.0)
  bool normalizeDistributions = true; // If true, distributions are normalized. If false, raw counts are used
  bool logAxis = true;

  TString dateFileString = "07072025";
  TString jetRadiusString = "0.8"; // Or 0.4

  // Bin vectors 
  std::vector<std::pair<double,double>> comparedCentralityBin;
  comparedCentralityBin.push_back(std::make_pair(-1,100));

  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));
//  comparedJetPtBin.push_back(std::make_pair(140,160));
//  comparedJetPtBin.push_back(std::make_pair(160,180));
//  comparedJetPtBin.push_back(std::make_pair(180,200));
  
  std::vector<std::pair<double,double>> comparedRL; // Set (0.0, 0.0) if RL=All
//  comparedRL.push_back(std::make_pair(0.0,0.0)); 
  comparedRL.push_back(std::make_pair(0.3,0.4)); 
  comparedRL.push_back(std::make_pair(0.4,0.5)); 
  comparedRL.push_back(std::make_pair(0.5,0.6)); 
  comparedRL.push_back(std::make_pair(0.6,0.7)); 
  comparedRL.push_back(std::make_pair(0.7,0.8)); 

  std::vector<double> comparedTrackPtBin;
//  comparedTrackPtBin.push_back(0.7);
  comparedTrackPtBin.push_back(1.0);
//  comparedTrackPtBin.push_back(2.0);
//  comparedTrackPtBin.push_back(3.0);
//  comparedTrackPtBin.push_back(4.0);
 
  std::vector<bool> compared2d;
  compared2d.push_back(true);
  compared2d.push_back(false); // Makes 2d plot then 3d plot

  // ====================================================
  // Loop through characteristics
  // ====================================================
 
 // Manual enumeration
 int i = 0;
 int j = 0;
 int k = 0;
 
 for (auto bool2d : compared2d){
   i = 0;
   for(auto RLBin : comparedRL){
     j = 0;
     for(auto jetPtBin : comparedJetPtBin){
        k = 0;
	for(auto trackPtBin : comparedTrackPtBin){

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
	  
	  int iJetPt, iTrackPt;
	  int iCentrality = 0;
	  int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorFull;
	  int iPairingType = EECHistograms::kSameJetPair;
	  int iSubevent = EECHistograms::knSubeventCombinations;
	  
	  // Initialize the jet response matrices to NULL
	  TH2D* hEnergyEnergyEnergyCorrelatorFull[nJetPtBinsEEC][nTrackPtBinsEEC];
	
	      for(int iJetPt = 0; iJetPt < nJetPtBinsEEC; iJetPt++){
	         for(int iTrackPt = 0; iTrackPt < nTrackPtBinsEEC; iTrackPt++){
	        hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = NULL;
	         } // Track pT
	      } // Jet pT

	// ====================================================
	//  Read in the E3C
	// ====================================================
        
	iJetPt = card->FindBinIndexJetPtEEC(jetPtBin);
        iTrackPt = card->GetBinIndexTrackPtEEC(trackPtBin);
	is2d = bool2d;

	if(normalizeDistributions){
            std::pair<double, double> drawingRange = std::make_pair(0.001, 0.999);
	    double epsilon = 0.0001;

	hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, iTrackPt, iPairingType, iSubevent);
	
	int lowNormalizationBin = hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetXaxis()->FindBin(drawingRange.first + epsilon);
           int highNormalizationBin = hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetXaxis()->FindBin(drawingRange.second - epsilon);

            hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->Scale(1 / hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->Integral(lowNormalizationBin, highNormalizationBin, lowNormalizationBin, highNormalizationBin, "width"));
          } else {
	hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, iTrackPt, iPairingType, iSubevent);
          } // Normalization  

      // ====================================================
      // Plot each E3C
      // ====================================================

	// Prepare a JDrawer for drawing purposes
	  JDrawer *drawer = new JDrawer();
	  if(logAxis) {drawer->SetLogZ(true);}
	  drawer->SetLeftMargin(0.13);
	  drawer->SetRightMargin(0.16);
	  drawer->SetTopMargin(0.08);
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
	  
	  TString trackPtFileString = Form("T=%.1f", comparedTrackPtBin.at(k));
	  TString ptTitleString = "";
	  TString normalizeFileString = "";
	  if(normalizeDistributions) {normalizeFileString = "Normalized_";}
	  
	  TString axisFileString = "ogAxis";
	  if(logAxis) {axisFileString = "logAxis";}
	  if(normalizeDistributions) {axisFileString = "normalAxis";}
	  if(logAxis && normalizeDistributions) {axisFileString = "normalLogAxis";}
	
	
	  if(normalizeDistributions) { 
	  	if(allRL){
		    ptTitleString = Form("R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV, Normalized", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k));}
		else{    
		    ptTitleString = Form("R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV, %.1f #leq R_{L} < %.1f, Normalized", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k), comparedRL.at(i).first, comparedRL.at(i).second);}}
	  else if(allRL) {ptTitleString = Form("R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k));} 
	  else { ptTitleString = Form("R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV, %.1f #leq R_{L} < %.1f", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k), comparedRL.at(i).first, comparedRL.at(i).second);} 

	  // Draw the histogram to canvas
	if(normalizeDistributions && logAxis){
//		hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetZaxis()->SetRangeUser(0.01,10);
	} 

        if(is2d){
		drawer->DrawHistogram(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt], "X", "Y", 0, 0, ptTitleString, "colz");
	} else {
	drawer->DrawHistogram(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt], "X", "Y", 45, 135, ptTitleString, "lego2z");}
	
	gPad->Modified();
	gPad->Update();
	  
	  // Save figure
	  if(is2d){
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/jetRadius_%s/%s/%s/2d_%s%s_%s_%s_%s.pdf", jetRadiusString.Data(),axisFileString.Data(), RLFileString.Data(), normalizeFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));
	  } else {
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/jetRadius_%s/%s/%s/3d_%s%s_%s_%s_%s.pdf", jetRadiusString.Data(), axisFileString.Data(), RLFileString.Data(), normalizeFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));
	  } 

       if(k < comparedTrackPtBin.size() - 1) {k++;}
       } // Track pT
     if(j < comparedJetPtBin.size() - 1) {j++;}
     } // Jet pT
   if(i < comparedRL.size() - 1) {i++;}
   } // RL

 } // is2d

} // End script
