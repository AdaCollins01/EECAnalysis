#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"

/*
 * Macro for finding background normalization scale in MC. This is done by integrating the energy-energy correlator distributions for background within the signal cone, and in the reflected cone, and taking a ratio of these numbers.
 */
void plotEEECxyPlane(){

  // Enumeration for projection direction
  enum enumProjectionType{kConstantGen, kConstantReco, knProjectionTypes};

  // File from which the integrals are calculated
  TString inputFileName = "projected_xyPlaneTest_06122025.root";
  
  // Open the input file
  TFile* inputFile = TFile::Open(inputFileName);
  
  if(inputFile == NULL){
    cout << "Error! The file " << inputFileName.Data() << " does not exist!" << endl;
    cout << "Maybe you forgot the data/ folder path?" << endl;
    cout << "Will not execute the code" << endl;
    return;
  }
  
  // Load the card from the file and read the collision system
  EECCard* card = new EECCard(inputFile);
  TString collisionSystem = card->GetDataType();
  bool isPbPbData = collisionSystem.Contains("PbPb");
  
  // ====================================================
  //  Binning configuration for the integral calculation
  // ====================================================
  
  // Find the number of bins from the card
  const int nCentralityBins = (isPbPbData) ? card->GetNCentralityBins() : 1;
  const int nAnalysisJetPtBins = 4;
  
  // Default binning ranges for reference
  // centrality = {0,10,30,50,90}

  const double analysisJetPtBinBorders[nAnalysisJetPtBins+1] = {120,140,160,180,200};
    
  // Create and setup a new histogram manager to project and handle the histograms
  EECHistogramManager* histograms = new EECHistogramManager(inputFile,card);
  
  // Choose the energy-energy correlator types to load
  histograms->SetLoadEnergyEnergyEnergyCorrelators(true);
  
  // Choose the bin ranges
  histograms->SetCentralityBinRange(0,nCentralityBins);
  
  // Load the histograms from the file
  histograms->LoadHistograms(); // Processed or unprocessed?

  // Initialize E3C to NULL
  TH2D* hEnergyEnergyEnergyCorrelatorFull;

for(int iCentrality = 0; iCentrality < nCentralityBins; iCentrality++){
    for(int iJetPt = 0; iJetPt < nAnalysisJetPtBins; iJetPt++){
      for(int iTrackPt = 0; iTrackPt < nTrackPtBins; iTrackPt++){
        hEnergyEnergyEnergyCorrelatorFull[iCentrality][iJetPt][iTrackPt] = NULL;
      } // Track pT loop
    }  // Jet pT loop
  }  // Centrality loop

  // Read the jet response matrices from the file
for(int iCentrality = 0; iCentrality < nCentralityBins; iCentrality++){
    for(int iJetPt = 0; iJetPt < nAnalysisJetPtBins; iJetPt++){
      for(int iTrackPt = 0; iTrackPt < nTrackPtBins; iTrackPt++){
        hEnergyEnergyEnergyCorrelatorFull[iCentrality][iJetPt][iTrackPt] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFullProcessed(8, iCentrality, iJetPt, iTrackPt);
      } // Track pT loop
    }  // Jet pT loop
  }  // Centrality loop


  // **********************************
  //         Draw the figures
  // **********************************

  // Prepare a JDrawer for drawing purposes
  JDrawer* drawer = new JDrawer();
  //drawer->SetLogZ(true);
  drawer->SetLeftMargin(0.13);
  drawer->SetRightMargin(0.11);
  drawer->SetTopMargin(0.08);
  drawer->SetTitleOffsetX(1.17);
  drawer->SetTitleOffsetY(1);

  // Helper variables
  TLegend* legend;
  TString centralityString;
  TString compactCentralityString;
  TString compactJetPtString;

  // Draw the response matrices to canvas
  for(int iCentrality = 0; iCentrality < nCentralityBins; iCentrality++){

    if(isPbPbData){
      centralityString = Form("Pythia+Hydjet: %.0f-%.0f", card->GetLowBinBorderCentrality(iCentrality), card->GetHighBinBorderCentrality(iCentrality));
      compactCentralityString = Form("_C%.0f-%.0f", card->GetLowBinBorderCentrality(iCentrality), card->GetHighBinBorderCentrality(iCentrality));
    } else {
      centralityString = "Pythia8";
      compactCentralityString = "_pythia8";
    }

    hEnergyEnergyEnergyCorrelatorFull[iCentrality]->GetZaxis()->SetRangeUser(0.01,3000);
    hEnergyEnergyEnergyCorrelatorFull[iCentrality]->GetXaxis()->SetRangeUser(0,1);
    hEnergyEnergyEnergyCorrelatorFull[iCentrality]->GetYaxis()->SetRangeUser(0,1);
    drawer->DrawHistogram(hEnergyEnergyEnergyCorrelatorFull[iCentrality],"x", "y", "E3C in XY Plane", "colz");

  } // Centrality loop

  drawer->SaveFigure("xyE3C_06122025.pdf");

//        legend = new TLegend(0.42, 0.7, 0.8, 0.88);
//        legend->SetFillStyle(0); legend->SetBorderSize(0); legend->SetTextSize(0.05); legend->SetTextFont(62);
//
//        legend->AddEntry((TObject*)0, centralityString.Data(), "");
//        legend->AddEntry((TObject*)0, jetPtString.Data(), "");
//
//        legend->Draw();
}
