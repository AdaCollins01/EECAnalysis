#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"

/*
 * Macro for plotting distribution of E3C, projected into xy coordinates. 
 */
void plotEEECxyPlane(){

  // File from which the E3C are read
  TString inputFileName = "projected_xyPlaneTest_06122025.root";
  
  // Open the input file
  TFile* inputFile = TFile::Open(inputFileName);
  
  if(inputFile == NULL){
    cout << "Error! The file " << inputFileName.Data() << " does not exist!" << endl;
    cout << "Maybe you forgot the data/ folder path?" << endl;
    cout << "Will not execute the code" << endl;
    return;
  }
  
  // Load the card from the file
  EECCard* card = new EECCard(inputFile);
  
  // ====================================================
  //  Binning configuration for the plotting
  // ====================================================
  
  // Number of bins from the card
  const int nCentralityBinsEEC = card->GetNCentralityBins();
  const int nJetPtBinsEEC = card->GetNJetPtBinsEEC();
  const int nTrackPtBinsEEC = card->GetNTrackPtBinsEEC();

  cout << "Jet pT Bins: " << nJetPtBinsEEC << endl;
  cout << "Track pT Bins: " << nTrackPtBinsEEC << endl;
  cout << "Centrality Bins: " << nCentralityBinsEEC << endl;

  cout << "Available Centrality Bin: " << EECCard::kCentralityBinEdges << endl;

  // Bin vectors - start with one 
  std::vector<std::pair<double,double>> comparedCentralityBin; // Maybe don't want centrality? Because it's pp?
  comparedCentralityBin.push_back(std::make_pair(-1,100));

  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));

  std::vector<double> comparedTrackPtBin;
  comparedTrackPtBin.push_back(1.0);

  // Create and setup a new histogram manager to project and handle the histograms
  EECHistogramManager* histograms = new EECHistogramManager(inputFile,card); // Should already know what to do
  
  int iJetPt, iTrackPt, iCentrality;
  int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorFull;
  int iPairingType = EECHistograms::kSameJetPair;
  int iSubevent = EECHistograms::kPythiaPythia;
 
  // Initialize the jet response matrices to NULL
  //TH2D* hEnergyEnergyEnergyCorrelatorFull[nJetPtBinsEEC][nTrackPtBinsEEC];
  TH2D* hEnergyEnergyEnergyCorrelatorFull[comparedJetPtBin.size()][comparedTrackPtBin.size()];

//      for(int iJetPt = 0; iJetPt < nJetPtBinsEEC; iJetPt++){
//         for(int iTrackPt = 0; iTrackPt < nTrackPtBinsEEC; iTrackPt++){
//        hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = NULL;
//         } // Track pT
//      } // Jet pT

  
  // Read the E3C from the file
    for(auto centralityBin : comparedCentralityBin){
     for(auto jetPtBin : comparedJetPtBin){
        for(auto trackPtBin : comparedTrackPtBin){

	iCentrality = card->FindBinIndexCentrality(centralityBin);
	iJetPt = card->FindBinIndexJetPtEEC(jetPtBin);
        iTrackPt = card->GetBinIndexTrackPtEEC(trackPtBin);
	  
	cout << "Centrality: " << iCentrality << endl;
  	cout << "Jet pT: " << iJetPt << endl;
  	cout << "Track pT: " << iTrackPt << endl;
   	
	hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, iTrackPt, iPairingType, iSubevent);
       
        // Debugging  
	if(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]){
		cout << "Not null after filling" << endl;
	}else{
	cout << "Null after filling" << endl;}

       } // Track pT
     } // Jet pT
   } // Centrality

  // Prepare a JDrawer for drawing purposes
  JDrawer *drawer = new JDrawer();
  drawer->SetLogZ(true);
  drawer->SetLeftMargin(0.13);
  drawer->SetRightMargin(0.11);
  drawer->SetTopMargin(0.08);
  drawer->SetTitleOffsetX(1.17);
  drawer->SetTitleOffsetY(1);

  // Draw the response matrices to canvas
     for(auto jetPtBin : comparedJetPtBin){
        for(auto trackPtBin : comparedTrackPtBin){
	
	iJetPt = card->FindBinIndexJetPtEEC(jetPtBin);
        iTrackPt = card->GetBinIndexTrackPtEEC(trackPtBin);
        
	// Debugging
	if(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]){
		cout << "Not null at start of drawing" << endl;
		hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetZaxis()->SetRangeUser(0.01,3000); 
		drawer->DrawHistogram(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt], "X", "Y", "120 < Jet pT < 140 [GeV], Track pT = 1.0 GeV", "colz");
	} else{
	cout << "Null at the start of drawing" << endl;}

	} // Track pT
     } // Jet pT
  
  // Save figure
   gPad->GetCanvas()->SaveAs("figures/xyPlaneTest_06132025.pdf");
}
