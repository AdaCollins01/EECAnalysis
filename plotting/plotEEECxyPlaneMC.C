#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting distribution of E3C, projected into xy coordinates using files generated from Kurt's 
 * PYTHIA setup. 
 */
void plotEEECxyPlaneMC(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================
  
  bool is2d; // Helper to loop through 2d and 3d plots
  bool allRL = false; // To set this as true, comparedRL = (0.0, 0.0)
  bool normalizeDistributions = false; // If true, distributions are normalized. If false, raw counts are used
  bool normalizeArea = false; // If true, area integral = 1, otherwise, normalized by nJets
  bool logAxis = true;

  TString dateFileString = "07182025";
  TString jetRadiusString = "0.8"; // Or 0.4
  TString partonShowerString = "Simple";
  TString inputFileName = "pythia/rootFiles/pythia_ppCP5_eec_simple_07142025.root";

  // Bin vectors 
  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));
//  comparedJetPtBin.push_back(std::make_pair(140,160));
//  comparedJetPtBin.push_back(std::make_pair(160,180));
//  comparedJetPtBin.push_back(std::make_pair(180,200));
  
  std::vector<std::pair<double,double>> comparedRL; // Set (0.0, 0.0) if RL=All
  comparedRL.push_back(std::make_pair(0.0,0.0)); 
//  comparedRL.push_back(std::make_pair(0.3,0.4)); 
//  comparedRL.push_back(std::make_pair(0.4,0.5)); 
//  comparedRL.push_back(std::make_pair(0.5,0.6)); 
//  comparedRL.push_back(std::make_pair(0.6,0.7)); 
// comparedRL.push_back(std::make_pair(0.7,0.8)); 

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
	 
	  // Open the input file
	  TFile* inputFile = TFile::Open(inputFileName);
	  
	  if(inputFile == NULL){
	    cout << "Error! The file " << inputFileName.Data() << " does not exist!" << endl;
	    cout << "Maybe you forgot the jetRadius_x/ folder path? Or added the incorrect date?" << endl;
	    cout << "Will not execute the code" << endl;
	    return;
	  }
	  
	// ====================================================
	//  Read in the E3C
	// ====================================================
	
	is2d = bool2d;
	TH2F* hist;
	TString histname = Form("trackPt_%.1f_jetPt_%.0f-%.0f_Charged_E1", comparedTrackPtBin.at(k), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
	cout << histname << endl;

	hist = (TH2F*) inputFile->Get(histname);

	if(normalizeDistributions){
		if(normalizeArea){
	            std::pair<double, double> drawingRange = std::make_pair(0.001, 0.999);
		    double epsilon = 0.0001;
		    int lowNormalizationBin = hist->GetXaxis()->FindBin(drawingRange.first + epsilon);
	            int highNormalizationBin = hist->GetXaxis()->FindBin(drawingRange.second - epsilon);
	
	            hist->Scale(1 / hist->Integral(lowNormalizationBin, highNormalizationBin, lowNormalizationBin, highNormalizationBin, "width"));
	          }
		else{ // Normalize by number of jets
		    TParameter<int>* param = (TParameter<int>*) inputFile->Get("numJets");
		    double nJets = (double) param->GetVal();
		    cout << nJets << endl;
	            hist->Scale(1 / nJets);
		}
	  }

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
	  
	  TString partonShowerFileString = partonShowerString;
	  TString trackPtFileString = Form("T=%.1f", comparedTrackPtBin.at(k));
	  TString ptTitleString = "";
	  TString normalMethodString = "N_{Jets}";
	  if(normalizeArea) {normalMethodString = "Area";}
	  TString normalizeFileString = "";
	  if(normalizeDistributions) {normalizeFileString = "normalizedNJets_";}
	  if(normalizeArea) {normalizeFileString = "normalizedArea_";}
	  
	  TString axisFileString = "ogAxis";
	  if(logAxis) {axisFileString = "logAxis";}
	  if(normalizeDistributions) {axisFileString = "normalAxis";}
	  if(logAxis && normalizeDistributions) {axisFileString = "normalLogAxis";}
	
	
	  if(normalizeDistributions) { 
	  	if(allRL){
		    ptTitleString = Form("R_{Jet} = %s, Parton Shower = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV, Normalized by  %s", jetRadiusString.Data(), partonShowerString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k), normalMethodString.Data());}
		else{    
		    ptTitleString = Form("R_{Jet} = %s, Parton Shower = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV, %.1f #leq R_{L} < %.1f, Normalized by %s", jetRadiusString.Data(), partonShowerString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k), comparedRL.at(i).first, comparedRL.at(i).second, normalMethodString.Data());}}
	  else if(allRL) {ptTitleString = Form("R_{Jet} = %s, Parton Shower = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV", jetRadiusString.Data(), partonShowerString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k));} 
	  else { ptTitleString = Form("R_{Jet} = %s, Parton Shower = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Track p_{T} #geq %.1f GeV, %.1f #leq R_{L} < %.1f", jetRadiusString.Data(), partonShowerString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, comparedTrackPtBin.at(k), comparedRL.at(i).first, comparedRL.at(i).second);} 

	  // Draw the histogram to canvas
	if(normalizeDistributions && logAxis){
//		hEnergyEnergyEnergyCorrelatorFull[iJetPt][iTrackPt]->GetZaxis()->SetRangeUser(0.01,10);
	} 

        if(is2d){
		drawer->DrawHistogram(hist, "X", "Y", 0, 0, ptTitleString, "colz");
	} else {
	drawer->DrawHistogram(hist, "X", "Y", 45, 135, ptTitleString, "lego2z");}
	
	gPad->Modified();
	gPad->Update();
	  
	  // Save figure
	  if(is2d){
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/pythia/%s/2d_CP5_%s_%s%s_%s_%s_%s.pdf", axisFileString.Data(), partonShowerFileString.Data(), normalizeFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));
	  } else {
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/pythia/%s/3d_CP5_%s_%s%s_%s_%s_%s.pdf", axisFileString.Data(), partonShowerFileString.Data(), normalizeFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));
	  } 

       if(k < comparedTrackPtBin.size() - 1) {k++;}
       } // Track pT
     if(j < comparedJetPtBin.size() - 1) {j++;}
     } // Jet pT
   if(i < comparedRL.size() - 1) {i++;}
   } // RL

 } // is2d

} // End script
