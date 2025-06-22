#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting distribution of E3C, projected into xy coordinates. 
 */
void plotEEECxyPlane(){

  // Characteristics that are often changed
  bool is2d = true; // If true, 2d plot is made. If false, 3d plot is made
  bool normalizeDistributions = true; // If true, distributions are normalized. If false, raw counts are used
  TString dateFileString = "06222025";

  // File from which the E3C are read
  TString inputFileName = "projected_noLog_xyPlaneFull_06172025.root";
  
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

  // Bin vectors - start with one 
  std::vector<std::pair<double,double>> comparedCentralityBin; // Maybe don't want centrality? Because it's pp?
  comparedCentralityBin.push_back(std::make_pair(-1,100));

  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(160,180));

  std::vector<double> comparedTrackPtBin;
  comparedTrackPtBin.push_back(1.0);

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

  
  // Read the E3C from the file
     for(auto jetPtBin : comparedJetPtBin){
        for(auto trackPtBin : comparedTrackPtBin){

	//iCentrality = card->FindBinIndexCentrality(centralityBin);
	iJetPt = card->FindBinIndexJetPtEEC(jetPtBin);
        iTrackPt = card->GetBinIndexTrackPtEEC(trackPtBin);
	
	if(normalizeDistributions){
            std::pair<double, double> drawingRange = std::make_pair(0.008, 0.39);
	    double epsilon = 0.0001;

	hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, iTrackPt, iPairingType, iSubevent);
	
	int lowNormalizationBin = hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetXaxis()->FindBin(drawingRange.first + epsilon);
           int highNormalizationBin = hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetXaxis()->FindBin(drawingRange.second - epsilon);

            hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->Scale(1 / hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->Integral(lowNormalizationBin, highNormalizationBin, lowNormalizationBin, highNormalizationBin, "width"));
          } else {
	hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt] = histograms->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPt, iTrackPt, iPairingType, iSubevent);
          } 

       } // Track pT
     } // Jet pT

  // Prepare a JDrawer for drawing purposes
  JDrawer *drawer = new JDrawer();
  //drawer->SetLogZ(true);
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

  TString jetPtFileString = Form("J=%.0f-%.0f", comparedJetPtBin.at(0).first, comparedJetPtBin.at(0).second);
  TString trackPtFileString = Form("T=%.0f", comparedTrackPtBin.at(0));
  TString normalizeFileString = "";
  if(normalizeDistributions) {normalizeFileString = "Normalized_";}

  TString ptTitleString = Form("%.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} = %.0f GeV", comparedJetPtBin.at(0).first, comparedJetPtBin.at(0).second, comparedTrackPtBin.at(0));
  if(normalizeDistributions) {ptTitleString = Form("%.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} = %.0f GeV, Normalized", comparedJetPtBin.at(0).first, comparedJetPtBin.at(0).second, comparedTrackPtBin.at(0));}

  // Draw the histogram to canvas
     for(auto jetPtBin : comparedJetPtBin){
        for(auto trackPtBin : comparedTrackPtBin){
	iJetPt = card->FindBinIndexJetPtEEC(jetPtBin);
        iTrackPt = card->GetBinIndexTrackPtEEC(trackPtBin);
        
	if(normalizeDistributions){
		hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetZaxis()->SetRangeUser(0.01,24);
	} else {
		hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetZaxis()->SetRangeUser(0.01,3000);
	}

        if(is2d){
		drawer->DrawHistogram(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt], "X", "Y", 0, 0, ptTitleString, "colz");
	} else {
	drawer->DrawHistogram(hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt], "X", "Y", 45, 135, ptTitleString, "lego2z");}

gPad->Modified();
gPad->Update();

	} // Track pT
     } // Jet pT
  
  // Save figure
  if(is2d){
	    gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/2d_%s%s_%s_%s.pdf", normalizeFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), dateFileString.Data()));
  } else {
  	gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/3d_%s%s_%s_%s.pdf", normalizeFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), dateFileString.Data()));
  }
}
