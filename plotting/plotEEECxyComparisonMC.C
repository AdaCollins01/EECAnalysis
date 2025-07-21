#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting ratios of E3C projected into XY plane using files from Kurt's PYTHIA setup. Currently only
 * suited to plot ratios of track pT, should be generalized in the future. 
 */
void plotEEECxyComparisonMC(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================

  bool subtract = true; // Does (track pt > 0.7) - (track pt > 4.0)
  bool ratio = false; // Does (track pt > 0.7) / (track pt > 4.0)
  bool normalizeArea = false; // Normalizes area to be 1. Otherwise, normalizes by NJets
  bool allRL = false;
  bool logAxis = false;
  bool stackJetPt = true; // Puts all jet pt curves on same plot 

  if(ratio + subtract != 1) { 
  	cout << "Check your ratio/subtraction configuration. Please select one." << endl;
	return;
  }

  TString dateFileString = "07172025";
  TString jetRadiusString = "0.8"; // Or 0.4
  TString partonShowerString = "Dire";
  TString inputFileName = "pythia/rootFiles/pythia_pp_eec_dire_6000events_07162025.root";

  // Bin vectors 
  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));
  //comparedJetPtBin.push_back(std::make_pair(140,160));
  //comparedJetPtBin.push_back(std::make_pair(160,180));
  comparedJetPtBin.push_back(std::make_pair(180,200));
  comparedJetPtBin.push_back(std::make_pair(200,300));
  comparedJetPtBin.push_back(std::make_pair(300,500));
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
  comparedTrackPtBin.push_back(4.0);

  // ====================================================
  // Loop through characteristics
  // ====================================================
 
 // Manual enumeration
 int i = 0;
 int j = 0;
 int k = 0;

 std::vector<TH1D*> compareHists;
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
       TH2F* histFirst;
       TH2F* histSecond;
       TH1D* compareHist;
       
       TString histnameFirst = Form("trackPt_%.1f_jetPt_%.0f-%.0f_Charged_E1", comparedTrackPtBin.at(0), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
       TString histnameSecond= Form("trackPt_%.1f_jetPt_%.0f-%.0f_Charged_E1", comparedTrackPtBin.at(1), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
       
      histFirst = (TH2F*) inputFile->Get(histnameFirst); 
      histSecond = (TH2F*) inputFile->Get(histnameSecond);

	int yLowBin = histFirst->GetYaxis()->FindBin(0.0);
	int yHighBin = histFirst->GetYaxis()->FindBin(0.2);

	// Project histograms
	TH1D* projEEECFirst = histFirst->ProjectionX("projEEECFirst", yLowBin, yHighBin);
	TH1D* projEEECSecond = histSecond->ProjectionX("projEEECSecond", yLowBin, yHighBin);

	// Normalize them
	if(normalizeArea){
		double integralFirst = projEEECFirst->Integral("width");
		double integralSecond = projEEECSecond->Integral("width");
		projEEECFirst->Scale(1.0 / integralFirst);
		projEEECSecond->Scale(1.0 / integralSecond);
	}
	else{ // Normalize by NJets
		TParameter<int>* param = (TParameter<int>*) inputFile->Get("numJets");
		double nJets = (double) param->GetVal();
		
		projEEECFirst->Scale(1.0 / nJets);
		projEEECSecond->Scale(1.0 / nJets);
	}

	// Calculate comparison
	if(ratio){
		compareHist = (TH1D*) projEEECSecond->Clone(Form("compareHist_ratio_%d", j));
		compareHist->Divide(projEEECFirst);
		compareHist->Rebin(2);
	}
	
	if(subtract){
		compareHist = (TH1D*) projEEECFirst->Clone(Form("compareHist_subtract_%d", j));
		compareHist->Add(projEEECSecond, -1); // (track pt > 0.7) - (track pt > 4.0)
		compareHist->Rebin(2);
	}

	// For stackJetPt
	compareHists.push_back(compareHist);
	jetPtLabels.push_back(Form("Jet p_{T}: %.0f - %.0f GeV", comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second));

      // ====================================================
      // Plot each E3C
      // ====================================================
	
	if(!stackJetPt){ // Keep previous logic
	
	// Prepare a JDrawer for drawing purposes
	  JDrawer *drawer = new JDrawer();
	  if(logAxis) {drawer->SetLogY(true);}
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
	  TString normalMethodString = "N_{Jets}";
	  TString normalFileString = "normalNJets";
	  if(normalizeArea) {normalMethodString = "Area"; normalFileString = "normalArea";}
	  TString compareTitleString = "";
	  TString commentFileString = "";
	  if(subtract) {commentFileString = "subtraction";}
	  if(ratio) {commentFileString = "ratio";}
	  TString logFileString = "";
	  if(logAxis) {logFileString = "_logAxis";}
	  TString axisFileString = "comparison"; 
	  TString partonShowerFileString = partonShowerString;
	
	  ptTitleString = Form("PYTHIA 8 - %s Showers, 0 < Y < 0.2, R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Normalized by %s", partonShowerString.Data(), jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, normalMethodString.Data());
	  if(ratio) {compareTitleString = Form("#frac{Track p_{T} #geq %.1f GeV}{Track p_{T} #geq %.1f GeV}", comparedTrackPtBin.at(1), comparedTrackPtBin.at(0));}
	  if(subtract) {compareTitleString = Form("(Track p_{T} #geq %.1f GeV) - (Track p_{T} #geq %.1f GeV)", comparedTrackPtBin.at(0), comparedTrackPtBin.at(1));}

	  // Draw the histogram to canvas
	drawer->DrawHistogram(compareHist, "X", compareTitleString, ptTitleString);

	// Add guide lines
	if(subtract){
		TLine *line = new TLine(0, 0, 1.0, 0);
		line->SetLineWidth(2);
   		line->SetLineColor(kGray);
   		line->SetLineStyle(2); // Dashed
		line->Draw();
	}
	
	if(ratio){
		TLine *line = new TLine(0, 1.0, 1.0, 1.0);
		line->SetLineWidth(2);
   		line->SetLineColor(kGray);
   		line->SetLineStyle(2); // Dashed
		line->Draw();
	}
	
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
        if(logAxis) drawer->SetLogY(true);
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

        // Find global max and set y-axis max on first histogram
	if(ratio){
		double globalMax = 0.0;
		for(auto hist : compareHists) {
		    double maxInHist = hist->GetMaximum();
		    if(maxInHist > globalMax) globalMax = maxInHist;
		}
		compareHists[0]->SetMaximum(globalMax * 1.1); // 1.1 for buffer
	}

	if(subtract){
		double globalMin = 1000000.0;
		for(auto hist : compareHists) {
		    double minInHist = hist->GetMinimum();
		    cout << minInHist << endl;
		    if(minInHist < globalMin) globalMin = minInHist;
		}
		//compareHists[0]->SetMinimum(globalMin * 0.1); // 0.1 for buffer
	cout << globalMin << endl;
	}

	for(int iHist = 0; iHist < compareHists.size(); iHist++){
            compareHists[iHist]->SetLineColor(color[iHist]);
            compareHists[iHist]->SetMarkerStyle(markerStyle[iHist]);
            compareHists[iHist]->SetMarkerColor(color[iHist]);
            compareHists[iHist]->SetLineWidth(1);
            
	    if(iHist == 0){
	    	if(ratio){
                	drawer->DrawHistogram(compareHists[iHist], "X", Form("#frac{Track p_{T} #geq %.1f GeV}{Track p_{T} #geq %.1f GeV}", comparedTrackPtBin.at(1), comparedTrackPtBin.at(0)), Form("0 < Y < 0.2, R_{Jet} = %s, Normalized by %s", jetRadiusString.Data(), normalMethodString.Data()));
		}
		if(subtract){
                	drawer->DrawHistogram(compareHists[iHist], "X", Form("(Track p_{T} #geq %.1f GeV) - (Track p_{T} #geq %.1f GeV)", comparedTrackPtBin.at(0), comparedTrackPtBin.at(1)), Form("0 < Y < 0.2, R_{Jet} = %s, Normalized by %s", jetRadiusString.Data(), normalMethodString.Data()));
            	}
	    }
            
	    else{
                compareHists[iHist]->Draw("SAME");
            }
            legend->AddEntry(compareHists[iHist], jetPtLabels[iHist], "p");
        }

       
	// Add guide lines
	if(subtract){
		TLine *line = new TLine(0, 0, 1.0, 0);
		line->SetLineWidth(2);
   		line->SetLineColor(kGray);
   		line->SetLineStyle(kDashed); 
		line->Draw();
	}
	
	if(ratio){
		TLine *line = new TLine(0, 1.0, 1.0, 1.0);
		line->SetLineWidth(2);
   		line->SetLineColor(kGray);
   		line->SetLineStyle(kDashed); 
		line->Draw();
	}

        legend->Draw();
        
	gPad->Modified();
	gPad->Update();

	 TString trackPtFileString = Form("T=%.1f_%.1f", comparedTrackPtBin.at(1), comparedTrackPtBin.at(0));
	 TString normalFileString = "normalNJets";
	 if(normalizeArea) {normalFileString = "normalArea";}
	 TString commentFileString = "";
	 if(subtract) {commentFileString = "subtraction";}
	 if(ratio) {commentFileString = "ratio";}
	 TString logFileString = "";
	 if(logAxis) {logFileString = "_logAxis";}
	 TString axisFileString = "comparison"; 
	 TString partonShowerFileString = partonShowerString;

	 gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/pythia/%s/%s_stackedJetPt_%s%s_%s_%s_%s.pdf", axisFileString.Data(), partonShowerFileString.Data(), commentFileString.Data(), logFileString.Data(), normalFileString.Data(), trackPtFileString.Data(), dateFileString.Data()));

    } // If jet pts are stacked, save at the very end

} // End script
