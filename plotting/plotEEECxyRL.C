#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting E3C vs. RL using PYTHIA files from Kurt's setup. 
 */
void plotEEECxyRL(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================

  bool normalizeArea = false; // Normalizes area to be 1. Otherwise, normalizes by NJets
  bool allRL = false;
  bool stackJetPt = true; // Puts all jet pt curves on same plot 
  bool stackFiles = false; // Allows input from several different files on the same plot 
  bool partonShower = false;
  bool dataSource = false;
  bool validation = false;


  TString dateFileString = "07242025";
  TString jetRadiusString = "0.8"; // Or 0.4
  TString partonShowerString = "Simple";
  TString inputFileName = "pythia/rootFiles/pythia_pp_eec_simple_100000events_07232025.root";

  // Bin vectors 
  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));
 // comparedJetPtBin.push_back(std::make_pair(140,160));
 // comparedJetPtBin.push_back(std::make_pair(160,180));
 // comparedJetPtBin.push_back(std::make_pair(180,200));
  //comparedJetPtBin.push_back(std::make_pair(200,300));
  //comparedJetPtBin.push_back(std::make_pair(300,500));
  //comparedJetPtBin.push_back(std::make_pair(500,5000));
  
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
//  comparedTrackPtBin.push_back(4.0);

  std::vector<char*> comparedFiles;
  if(partonShower && !validation){
  }
  if(dataSource){
  	if(unique){
	comparedFiles.push_back("ppMC2017/justPythia/projected_xyPlane_justPythia_RL-M-S-Full_AllTriangles_07232025.root");
	comparedFiles.push_back("ppMC2017/pythiaGeant/projected_xyPlane_pythiaGeant_RL-M-S-Full_AllTriangles_07232025.root");
	comparedFiles.push_back("pp2017/projected_xyPlane_CMS_RL-M-S-Full_AllTriangles_07232025.root");
	} else {
	comparedFiles.push_back("ppMC2017/justPythia/projected_xyPlane_justPythia_RL-M-S-Full_AllTriangles_07232025.root");
	comparedFiles.push_back("ppMC2017/pythiaGeant/projected_xyPlane_pythiaGeant_RL-M-S-Full_AllTriangles_07232025.root");
	comparedFiles.push_back("pp2017/projected_xyPlane_CMS_RL-M-S-Full_AllTriangles_07232025.root");
	}
  }
  if(validation){
  }
  

 // Checkpoint
 if(stackFiles && stackJetPt || stackFiles && comparedJetPtBin.size() > 1){
 	cout << "Are you trying to stack files? Multiple jet pt options are selected." << endl;
	return;
 }

  // ====================================================
  // Loop through characteristics
  // ====================================================
 
 // Manual enumeration
 int i = 0;
 int j = 0;
 int k = 0;

 std::vector<TH1F*> compareHists;
 std::vector<TString> jetPtLabels;
 
   i = 0;
   for(auto RLBin : comparedRL){
     j = 0;
     for(auto jetPtBin : comparedJetPtBin){
        k = 0;
//	for(auto trackPtBin : comparedTrackPtBin){

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
       TH1F* hist;
       
       TString histname = Form("RLtrackPt_%.1f_jetPt_%.0f-%.0f_Charged_E1", comparedTrackPtBin.at(0), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
       
      hist = (TH1F*) inputFile->Get(histname); 

	// Normalize them
	if(normalizeArea){
		double integral = hist->Integral("width");
		hist->Scale(1.0 / integral);
	}
	else{ // Normalize by NJets
		TParameter<int>* param = (TParameter<int>*) inputFile->Get("numJets");
		double nJets = (double) param->GetVal();
		
		hist->Scale(1.0 / nJets);
	}

	// For stackJetPt
	compareHists.push_back(hist);
	jetPtLabels.push_back(Form("Jet p_{T}: %.0f - %.0f GeV", comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second));

      // ====================================================
      // Plot each E3C
      // ====================================================
	
	if(!stackJetPt){ // Keep previous logic
	
	// Prepare a JDrawer for drawing purposes
	  JDrawer *drawer = new JDrawer();
	  drawer->SetLogY(true);
	  drawer->SetLogX(true);
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
	  
	  TString trackPtFileString = Form("T=%.1f", comparedTrackPtBin.at(0));
	  TString ptTitleString = "";
	  TString normalMethodString = "N_{Jets}";
	  TString normalFileString = "normalNJets";
	  if(normalizeArea) {normalMethodString = "Area"; normalFileString = "normalArea";}
	  TString compareTitleString = "";
	  TString commentFileString = "RL";
	  TString logFileString = "";
	  TString axisFileString = "RL"; 
	  TString partonShowerFileString = partonShowerString;
	
	  ptTitleString = Form("PYTHIA 8 - %s Showers, R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Normalized by %s", partonShowerString.Data(), jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, normalMethodString.Data());

	  // Draw the histogram to canvas
	hist->GetXaxis()->SetRangeUser(0.008, 0.39);
	drawer->DrawHistogram(hist, "R_{L}", "EEEC", ptTitleString);
	
	gPad->Modified();
	gPad->Update();
	  
	  // Save figure
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/pythia/%s/%s_%s%s_%s_%s_%s_%s_%s.pdf",  axisFileString.Data(), partonShowerFileString.Data(), commentFileString.Data(), logFileString.Data(), normalFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));
	  } // If jet pts are individually plotted, save after each

       if(k < comparedTrackPtBin.size() - 1) {k++;}
    //   } // Track pT
     if(j < comparedJetPtBin.size() - 1) {j++;}
     } // Jet pT
   if(i < comparedRL.size() - 1) {i++;}
   } // RL

   if(stackJetPt){  // Draw all histograms on one canvas

        JDrawer *drawer = new JDrawer();
	drawer->SetLogY(true);
        drawer->SetLogX(true);
	drawer->SetLeftMargin(0.16);
        drawer->SetRightMargin(0.20);
        drawer->SetTopMargin(0.13);
        drawer->SetTitleOffsetX(1.2);
        drawer->SetTitleOffsetY(1.2);
        drawer->SetLabelSizeX(0.04);
        drawer->SetLabelSizeY(0.04);
        drawer->SetLabelSizeZ(0.03);
        drawer->SetTitleSizeX(0.05);
        drawer->SetTitleSizeY(0.05);
	int markerStyle[5] = {kFullCircle, kOpenSquare, kOpenCross, kFullStar, kFullDiamond}; 
	int color[5] = {kBlack, kRed, kGreen, kBlue, kOrange-1}; 
	
	TString normalMethodString = "N_{Jets}";
	if(normalizeArea) {normalMethodString = "Area";}

	TLegend *legend = new TLegend(0.80,0.40,0.98,0.57);
        legend->SetHeader(Form("PYTHIA 8 - %s", partonShowerString.Data()), "c");
	legend->SetBorderSize(0);
        legend->SetFillStyle(0); 


	for(int iHist = 0; iHist < compareHists.size(); iHist++){
            compareHists[iHist]->SetLineColor(color[iHist]);
            compareHists[iHist]->SetMarkerStyle(markerStyle[iHist]);
            compareHists[iHist]->SetMarkerColor(color[iHist]);
            compareHists[iHist]->SetLineWidth(1);
	    compareHists[iHist]->GetXaxis()->SetRangeUser(0.008, 0.39);
            
	    if(iHist == 0){
                	drawer->DrawHistogram(compareHists[iHist], "R_{L}", "EEEC", Form("R_{Jet} = %s, Normalized by %s", jetRadiusString.Data(), normalMethodString.Data()));
	    }
            
	    else{
                compareHists[iHist]->Draw("SAME");
            }
            legend->AddEntry(compareHists[iHist], jetPtLabels[iHist], "p");
        }


        legend->Draw();
        
	gPad->Modified();
	gPad->Update();

	 TString trackPtFileString = Form("T=%.1f", comparedTrackPtBin.at(0));
	 TString normalFileString = "normalNJets";
	 if(normalizeArea) {normalFileString = "normalArea";}
	 TString commentFileString = "RL";
	 TString logFileString = "";
	 TString axisFileString = "RL"; 
	 TString partonShowerFileString = partonShowerString;

	 gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/pythia/%s/%s_stackedJetPt_%s%s_%s_%s_%s.pdf", axisFileString.Data(), partonShowerFileString.Data(), commentFileString.Data(), logFileString.Data(), normalFileString.Data(), trackPtFileString.Data(), dateFileString.Data()));

    } // If jet pts are stacked, save at the very end

} // End script
