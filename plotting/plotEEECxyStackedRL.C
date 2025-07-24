#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting E3C against RL for different files, on the same plot. 
 * Use plotEEECxyRL.C or compareEECinDefinedBins for individual files, stacked bins. 
 */
void plotEEECxyStackedRL(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================

  bool allRL = true;
  bool logAxis = true;
  bool unique = true; // Helps with titles
  bool normalizeArea = false; // Normalizes by area. Defaults to normalizing by NJets
  bool partonShower = false; // Compares Simple, DIRE, VINCIA 
  bool dataSource = true; // Compares justPythia, pythiaGeant, CMS
  bool validation = false; // Compares parton showers to CMS

  if(partonShower + dataSource > 1 || dataSource + validation > 1) { 
  	cout << "Please select either partonShower, dataSource, or validation." << endl;
	return;
  }

  TString dateFileString = "07242025";
  TString jetRadiusString = "0.8"; // Or 0.4

  // Bin vectors 
  std::vector<std::pair<double,double>> comparedJetPtBin;
  comparedJetPtBin.push_back(std::make_pair(120,140));
  
  std::vector<std::pair<double,double>> comparedRL; // Set (0.0, 0.0) if RL=All
  comparedRL.push_back(std::make_pair(0.0,0.0)); 

  std::vector<double> comparedTrackPtBin;
  comparedTrackPtBin.push_back(0.7);

  // ====================================================
  // Loop through characteristics
  // ====================================================
 
 // Manual enumeration
 int i = 0;
 int j = 0;
 int k = 0;

 // Declarations necessary for plotting later
 std::vector<TH1D*> closureDataSource;
 std::vector<const char*> closureDataLabels;
 std::vector<TH1D*> closurePartonShower;
 std::vector<const char*> closurePartonShowerLabels;
 std::vector<TH1D*> closureValidation;
 std::vector<const char*> closureValidationLabels;

   i = 0;
   for(auto RLBin : comparedRL){
     j = 0;
     for(auto jetPtBin : comparedJetPtBin){
        k = 0;
	for(auto trackPtBin : comparedTrackPtBin){

	if(dataSource){
//  	  TString inputFileJustPythia = "ppMC2017/justPythia/jetRadius_0.8/projected_xyPlane_RL-All_06302025.root"; 
//	  TString inputFilePythiaGeant = "ppMC2017/pythiaGeant/projected_xyPlane_Geant_RL-All_07182025.root"; 
//	  TString inputFileCMS = "pp2017/projected_xyPlane_NoTrigger_RL-All_07182025.root";

	  TString inputFileJustPythia = "ppMC2017/justPythia/projected_xyPlane_justPythia_RL-M-S-Full_AllTriangles_07232025.root";
	  TString inputFilePythiaGeant = "ppMC2017/pythiaGeant/projected_xyPlane_pythiaGeant_RL-M-S-Full_AllTriangles_07232025.root";
	  TString inputFileCMS = "pp2017/projected_xyPlane_CMS_RL-M-S-Full_AllTriangles_07232025.root";
	  
	  if(unique){
	  	inputFileJustPythia = "ppMC2017/justPythia/projected_xyPlane_justPythia_RL-M-S-Full_UniqueTriangles_07232025.root";
	  	inputFilePythiaGeant = "ppMC2017/pythiaGeant/projected_xyPlane_pythiaGeant_RL-M-S-Full_UniqueTriangles_07232025.root";
	  	inputFileCMS = "pp2017/projected_xyPlane_CMS_RL-M-S-Full_UniqueTriangles_07232025.root";
	  }

	  // Open the input files
	  TFile* inputJustPythia = TFile::Open(inputFileJustPythia);
	  TFile* inputPythiaGeant = TFile::Open(inputFilePythiaGeant);
	  TFile* inputCMS = TFile::Open(inputFileCMS);
	  
	  if(inputJustPythia == NULL || inputPythiaGeant == NULL || inputCMS == NULL){
	    cout << "Error! One of the data source files does not exist!" << endl;
	    cout << "Will not execute the code" << endl;
	    return;
	  }
	   	  // Load the card from Jussi's files
	   	  EECCard* cardJustPythia = new EECCard(inputJustPythia);
	   	  EECCard* cardPythiaGeant = new EECCard(inputPythiaGeant);
	   	  EECCard* cardCMS = new EECCard(inputCMS);
	   	  
	   	  // ====================================================
	   	  //  Initial configuration for the plotting
	   	  // ====================================================
	   	  
	   	  // Number of bins from the card - should all have the same bins
	   	  const int nCentralityBinsEEC = cardCMS->GetNCentralityBins();
	   	  const int nJetPtBinsEEC = cardCMS->GetNJetPtBinsEEC();
	   	  const int nTrackPtBinsEEC = cardCMS->GetNTrackPtBinsEEC();
	   	
	   	  // Create and setup a new histogram manager to project and handle the histograms
	   	  EECHistogramManager* histogramsJustPythia = new EECHistogramManager(inputJustPythia,cardJustPythia); 
	   	  EECHistogramManager* histogramsPythiaGeant = new EECHistogramManager(inputPythiaGeant,cardPythiaGeant); 
	   	  EECHistogramManager* histogramsCMS = new EECHistogramManager(inputCMS,cardCMS); 
	   	  
		  int iCentrality = 0;
	   	  int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorRL;
	   	  int iPairingType = EECHistograms::kSameJetPair;
	   	  int iSubevent = EECHistograms::knSubeventCombinations;
	   	  
	   	  // Create two NULL histogram objects
	   	  TH1D* hEnergyEnergyEnergyCorrelatorJustPythia[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with justPythia value
	   	  TH1D* hEnergyEnergyEnergyCorrelatorPythiaGeant[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with pythiaGeant value
	   	  TH1D* hEnergyEnergyEnergyCorrelatorCMS[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with CMS value
	   
	   	      for(int iJetPt = 0; iJetPt < nJetPtBinsEEC; iJetPt++){
	   		for(int iTrackPt = 0; iTrackPt < nTrackPtBinsEEC; iTrackPt++){
	   	        	hEnergyEnergyEnergyCorrelatorJustPythia[iJetPt][iTrackPt] = NULL;
	   	        	hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPt][iTrackPt] = NULL;
	   	        	hEnergyEnergyEnergyCorrelatorCMS[iJetPt][iTrackPt] = NULL;
	   	         } // Track pT
	   	      } // Jet pT
	   
	   	// ====================================================
	   	//  Read in the E3C
	   	// ====================================================
	           
	   	// justPythia
	   		int iJetPtJustPythia = cardJustPythia->FindBinIndexJetPtEEC(jetPtBin);
	   	        int iTrackPtJustPythia = cardJustPythia->GetBinIndexTrackPtEEC(trackPtBin);
	   	
	   		hEnergyEnergyEnergyCorrelatorJustPythia[iJetPtJustPythia][iTrackPtJustPythia] = histogramsJustPythia->GetHistogramEnergyEnergyCorrelator(iEnergyEnergyCorrelatorType, iCentrality, iJetPtJustPythia, iTrackPtJustPythia, iPairingType, iSubevent);
	   	
	   		TH1D* projEEECJustPythia = hEnergyEnergyEnergyCorrelatorJustPythia[iJetPtJustPythia][iTrackPtJustPythia]; // Renaming it is just easier

			// Normalize them
			if(normalizeArea){
	   			double integralJustPythia = projEEECJustPythia->Integral("width");
	   			projEEECJustPythia->Scale(1.0 / integralJustPythia);
			} else {
				std::pair<double,double> jetPtBinBordersJustPythia = cardJustPythia->GetBinBordersJetPtEEC(iJetPtJustPythia);
        			double nJetsJustPythia = histogramsJustPythia->GetJetPtIntegral(iCentrality, jetPtBinBordersJustPythia.first, jetPtBinBordersJustPythia.second);
	   			projEEECJustPythia->Scale(1.0 / nJetsJustPythia);
			}
	   	
	   	// pythiaGeant
	   		int iJetPtPythiaGeant = cardPythiaGeant->FindBinIndexJetPtEEC(jetPtBin);
	   	        int iTrackPtPythiaGeant = cardPythiaGeant->GetBinIndexTrackPtEEC(trackPtBin);
	   	
	   		hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPtPythiaGeant][iTrackPtPythiaGeant] = histogramsPythiaGeant->GetHistogramEnergyEnergyCorrelator(iEnergyEnergyCorrelatorType, iCentrality, iJetPtPythiaGeant, iTrackPtPythiaGeant, iPairingType, iSubevent);
	   	
	   		TH1D* projEEECPythiaGeant = hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPtPythiaGeant][iTrackPtPythiaGeant];
	   	
	   		// Normalize them
			if(normalizeArea){
	   			double integralPythiaGeant = projEEECPythiaGeant->Integral("width");
	   			projEEECPythiaGeant->Scale(1.0 / integralPythiaGeant);
			} else {	
				std::pair<double,double> jetPtBinBordersPythiaGeant = cardPythiaGeant->GetBinBordersJetPtEEC(iJetPtPythiaGeant);
        			double nJetsPythiaGeant = histogramsPythiaGeant->GetJetPtIntegral(iCentrality, jetPtBinBordersPythiaGeant.first, jetPtBinBordersPythiaGeant.second);
	   			projEEECPythiaGeant->Scale(1.0 / nJetsPythiaGeant);
			}
	   	
	   	// CMS
	   		int iJetPtCMS = cardCMS->FindBinIndexJetPtEEC(jetPtBin);
	   	        int iTrackPtCMS = cardCMS->GetBinIndexTrackPtEEC(trackPtBin);
	   	
	   		hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS] = histogramsCMS->GetHistogramEnergyEnergyCorrelator(iEnergyEnergyCorrelatorType, iCentrality, iJetPtCMS, iTrackPtCMS, iPairingType, iSubevent);
	   	
	   		TH1D* projEEECCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS];
	   	
	   		// Normalize them
	   		if(normalizeArea){
				double integralCMS = projEEECCMS->Integral("width");
	   			projEEECCMS->Scale(1.0 / integralCMS);
			} else {
				std::pair<double,double> jetPtBinBordersCMS = cardCMS->GetBinBordersJetPtEEC(iJetPtCMS);
        			double nJetsCMS = histogramsCMS->GetJetPtIntegral(iCentrality, jetPtBinBordersCMS.first, jetPtBinBordersCMS.second);
	   			projEEECCMS->Scale(1.0 / nJetsCMS);
			}
	   	
		   	
        closureDataSource.clear();
	closureDataSource.push_back(projEEECJustPythia);
	closureDataSource.push_back(projEEECPythiaGeant);
	closureDataSource.push_back(projEEECCMS);

        closureDataLabels.clear();
	closureDataLabels.push_back("Pythia");
	closureDataLabels.push_back("Pythia-Geant");
	closureDataLabels.push_back("CMS");		
        
	}

      if(partonShower){

	  unique = true; // Currently only have files for unique triangles 

	  TString inputFileDire = "pythia/rootFiles/pythia_pp_eec_dire_uniqueTriangles_100000events_07232025.root";
  	  TString inputFileSimple = "pythia/rootFiles/pythia_pp_eec_simple_uniqueTriangles_100000events_07232025.root";
  	  TString inputFileVincia = "pythia/rootFiles/pythia_pp_eec_vincia_uniqueTriangles_100000events_07232025.root";
	 
	  // Open the input files
	  TFile* inputDire = TFile::Open(inputFileDire);
	  TFile* inputSimple = TFile::Open(inputFileSimple);
	  TFile* inputVincia = TFile::Open(inputFileVincia);
	  
	  if(inputDire == NULL || inputSimple == NULL || inputVincia == NULL){
	    cout << "Error! One of the parton shower files does not exist!" << endl;
	    cout << "Will not execute the code" << endl;
	    return;
	  }
	
	TString histname = Form("RLtrackPt_%.1f_jetPt_%.0f-%.0f_Charged_E1", comparedTrackPtBin.at(k), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
      	
	// Simple
		TH1D* projEEECSimple =  (TH1D*) inputSimple->Get(histname);
	
		// Normalize them
		if(normalizeArea){
			double integralSimple = projEEECSimple->Integral("width");
			projEEECSimple->Scale(1.0 / integralSimple);
		} else{
			TParameter<int>* paramSimple = (TParameter<int>*) inputSimple->Get("numJets");
			double nJetsSimple = (double) paramSimple->GetVal();
			projEEECSimple->Scale(1.0 / nJetsSimple);
		}
	// Dire
		TH1D* projEEECDire =  (TH1D*) inputDire->Get(histname);
	
		// Normalize them
		if(normalizeArea){
			double integralDire = projEEECDire->Integral("width");
			projEEECDire->Scale(1.0 / integralDire);
		} else {
			TParameter<int>* paramDire = (TParameter<int>*) inputDire->Get("numJets");
			double nJetsDire = (double) paramDire->GetVal();
			projEEECDire->Scale(1.0 / nJetsDire);
		}

	// Vincia
		TH1D* projEEECVincia =  (TH1D*) inputVincia->Get(histname);
	
		// Normalize them
		if(normalizeArea){
			double integralVincia = projEEECVincia->Integral("width");
			projEEECVincia->Scale(1.0 / integralVincia);
		} else {
			TParameter<int>* paramVincia = (TParameter<int>*) inputVincia->Get("numJets");
			double nJetsVincia = (double) paramVincia->GetVal();
			projEEECVincia->Scale(1.0 / nJetsVincia);
		}

	closurePartonShower.clear();
	closurePartonShower.push_back(projEEECSimple);
	closurePartonShower.push_back(projEEECDire);
	closurePartonShower.push_back(projEEECVincia);
	
	closurePartonShowerLabels.clear();
	closurePartonShowerLabels.push_back("Simple");
	closurePartonShowerLabels.push_back("DIRE");
	closurePartonShowerLabels.push_back("VINCIA");
       

	if(validation){
		TString inputFileCMS = "pp2017/projected_xyPlane_CMS_RL-M-S-Full_AllTriangles_07232025.root";
		TFile* inputCMS = TFile::Open(inputFileCMS);
		EECCard* cardCMS = new EECCard(inputCMS);
		const int nCentralityBinsEEC = cardCMS->GetNCentralityBins();
	   	const int nJetPtBinsEEC = cardCMS->GetNJetPtBinsEEC();
	   	const int nTrackPtBinsEEC = cardCMS->GetNTrackPtBinsEEC();
		EECHistogramManager* histogramsCMS = new EECHistogramManager(inputCMS,cardCMS);
		int iCentrality = 0;
	   	int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorRL;
	   	int iPairingType = EECHistograms::kSameJetPair;
	   	int iSubevent = EECHistograms::knSubeventCombinations;
		TH1D* hEnergyEnergyEnergyCorrelatorCMS[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with CMS value
		int iJetPtCMS = cardCMS->FindBinIndexJetPtEEC(jetPtBin);
	   	int iTrackPtCMS = cardCMS->GetBinIndexTrackPtEEC(trackPtBin);

   		hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS] = histogramsCMS->GetHistogramEnergyEnergyCorrelator(iEnergyEnergyCorrelatorType, iCentrality, iJetPtCMS, iTrackPtCMS, iPairingType, iSubevent);

   		// Project histograms
   		TH1D* projEEECCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS];

   		// Normalize them
   		if(normalizeArea){
			double integralCMS = projEEECCMS->Integral("width");
   			projEEECCMS->Scale(1.0 / integralCMS);
		} else {
			std::pair<double,double> jetPtBinBordersCMS = cardCMS->GetBinBordersJetPtEEC(iJetPtCMS);
       			double nJetsCMS = histogramsCMS->GetJetPtIntegral(iCentrality, jetPtBinBordersCMS.first, jetPtBinBordersCMS.second);
   			projEEECCMS->Scale(1.0 / nJetsCMS);
		}


		closureValidation.clear();
		closureValidation.push_back(projEEECSimple);
		closureValidation.push_back(projEEECDire);
		closureValidation.push_back(projEEECVincia);
		closureValidation.push_back(projEEECCMS);
		
		closureValidationLabels.clear();
		closureValidationLabels.push_back("Simple");
		closureValidationLabels.push_back("Dire");
		closureValidationLabels.push_back("Vincia");
		closureValidationLabels.push_back("CMS");

	   }
	}   
      

      // ====================================================
      // Plot each E3C
      // ====================================================
	

	// Prepare a JDrawer for drawing purposes
	  JDrawer *drawer = new JDrawer();
	  if(logAxis) {drawer->SetLogY(true);}
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
//	  int markerStyle[5] = {kFullCircle, kOpenSquare, kOpenCross, kFullStar, kFullDiamond}; 
//	  int color[5] = {kBlack, kRed, kGreen, kBlue, kOrange-1}; 
	  int markerStyle[9] = {kFullCircle, kFullCircle, kFullCircle, kOpenSquare, kOpenSquare, kOpenSquare, kFullStar, kFullStar, kFullStar}; 
	  int color[9] = {kBlack, kRed, kGreen, kBlack, kRed, kGreen, kBlack, kRed, kGreen}; 


	  TString jetPtFileString = Form("J=%.0f-%.0f", comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
	  TString RLFileString = Form("RL=%.1f-%.1f", comparedRL.at(i).first, comparedRL.at(i).second);
	 
	  if(allRL) {RLFileString = Form("RL=All");}
	  
	  TString trackPtFileString = Form("T=%.1f", comparedTrackPtBin.at(0));
	  TString ptTitleString = "";
	  TString normalMethodString = "N_{Jets}";
	  if(normalizeArea) {normalMethodString = "Area";}
	  TString normalFileString = "normalNJets";
	  if(normalizeArea) {normalFileString = "normalArea";}
	  TString triangleTitleString = "All";
	  if(unique) {triangleTitleString = "Unique";}
	  TString compareTitleString = "";
	  TString commentFileString = "RLDistributions";
	  if(unique) {commentFileString += "_uniqueTriangles";}
	  else { commentFileString += "_allTriangles";}
	  if(partonShower && !validation) {commentFileString += "_partonShowers";}
	  if(dataSource) {commentFileString += "_dataSource";}
	  if(validation) {commentFileString += "_validation";}
	  TString logFileString = "";
	  if(logAxis) {logFileString = "_logAxis";}
	  TString axisFileString = "closure"; 
	
	  //ptTitleString = Form("%s Triangles, R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Normalized by %s", triangleTitleString.Data(), jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, normalMethodString.Data());
	  ptTitleString = Form("R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Normalized by %s", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, normalMethodString.Data());
	
	TLegend *legend;
	if(partonShower && !validation) {legend = new TLegend(0.83,0.38,1.01,0.67);}
	if(dataSource) {legend = new TLegend(0.81,0.38,0.99,0.69);}
	
	if(validation) {legend = new TLegend(0.81,0.38,0.99,0.72);}
	
	legend->SetBorderSize(0);
        legend->SetFillStyle(0); 
	

	if(partonShower && !validation){

		legend->SetHeader("Parton Showers", "c");
		
		double globalMin = 1000000.0;
		for(auto hist : closurePartonShower) {
		    double minInHist = hist->GetMinimum();
		    if(minInHist < globalMin) globalMin = minInHist;
		}
		//closurePartonShower[0]->SetMinimum(globalMin * 0.1); // 0.1 for buffer
//		closurePartonShower[0]->SetMinimum(0.001); 
//		closurePartonShower[0]->SetMaximum(1.1); 

		for(int iHist = 0; iHist < closurePartonShower.size(); iHist ++){
	   	   closurePartonShower[iHist]->SetLineColor(color[iHist]);
                   closurePartonShower[iHist]->SetMarkerStyle(markerStyle[iHist]);
                   closurePartonShower[iHist]->SetMarkerColor(color[iHist]);
                   closurePartonShower[iHist]->SetLineWidth(1);
		   closurePartonShower[iHist]->GetXaxis()->SetRangeUser(0.008, 0.39);
		   if(iHist == 0){
		   	drawer->DrawHistogram(closurePartonShower[iHist], "R_{L}", "EEEC", ptTitleString);
		   } else {
		        closurePartonShower[iHist]->Draw("SAME");
		   }

		  TString legendEntry = Form("%s", closurePartonShowerLabels.at(iHist));
		  legend->AddEntry(closurePartonShower[iHist], legendEntry, "p");
		
		}
	}
	
	if(dataSource){

		double globalMin = 1000000.0;
		for(auto hist : closureDataSource) {
		    double minInHist = hist->GetMinimum();
		    if(minInHist < globalMin) globalMin = minInHist;
		}
		//closureDataSource[0]->SetMinimum(globalMin * 0.9); // 0.9 for buffer
//		if(normalizeArea && !unique){
//			closureDataSource[0]->SetMinimum(0.4); 
//			closureDataSource[0]->SetMaximum(1.8);
//		} else {
//			closureDataSource[0]->SetMinimum(0.1); 
//			closureDataSource[0]->SetMaximum(7.0);
//		}

		legend->SetHeader("Data Sources", "c");

		for(int iHist = 0; iHist < closureDataSource.size(); iHist ++){
	   	   closureDataSource[iHist]->SetLineColor(color[iHist]);
                   closureDataSource[iHist]->SetMarkerStyle(markerStyle[iHist]);
                   closureDataSource[iHist]->SetMarkerColor(color[iHist]);
                   closureDataSource[iHist]->SetLineWidth(1);
		   closureDataSource[iHist]->GetXaxis()->SetRangeUser(0.008, 0.39);
		   if(iHist == 0){
		   	drawer->DrawHistogram(closureDataSource[iHist], "R_{L}", "EEEC", ptTitleString);
		   } else {
		        closureDataSource[iHist]->Draw("SAME");
		   }

		  TString legendEntry = Form("%s", closureDataLabels.at(iHist));
		  legend->AddEntry(closureDataSource[iHist], legendEntry, "p");
		
		}
	}
	
	if(validation){

		legend->SetHeader("MC / Data", "c");
		
		double globalMin = 1000000.0;
		for(auto hist : closureValidation) {
		    double minInHist = hist->GetMinimum();
		    if(minInHist < globalMin) globalMin = minInHist;
		}
		//closureValidation[0]->SetMinimum(globalMin * 0.9); // 0.9 for buffer

		//closureValidation[0]->SetMinimum(0.6); 
		closureValidation[0]->SetMaximum(10000000); 

		for(int iHist = 0; iHist < closureValidation.size(); iHist ++){
	   	   closureValidation[iHist]->SetLineColor(color[iHist]);
                   closureValidation[iHist]->SetMarkerStyle(markerStyle[iHist]);
                   closureValidation[iHist]->SetMarkerColor(color[iHist]);
                   closureValidation[iHist]->SetLineWidth(1);
		   closureValidation[iHist]->GetXaxis()->SetRangeUser(0.008, 0.39);
		   if(iHist == 0){
		   	drawer->DrawHistogram(closureValidation[iHist], "R_{L}", "EEEC", ptTitleString);
		   } else {
		        closureValidation[iHist]->Draw("SAME");
		   }

		  TString legendEntry = Form("%s", closureValidationLabels.at(iHist));
		  legend->AddEntry(closureValidation[iHist], legendEntry, "p");
		
		}
	}

	legend->Draw();
	
	gPad->Modified();
	gPad->Update();
	  
	  // Save figure
	   gPad->GetCanvas()->SaveAs(Form("figures/xyPlane/closure/%s%s_%s_%s_%s_%s_%s.pdf", commentFileString.Data(), logFileString.Data(), normalFileString.Data(), jetPtFileString.Data(), trackPtFileString.Data(), RLFileString.Data(), dateFileString.Data()));

       if(k < comparedTrackPtBin.size() - 1) {k++;}
       } // Track pT
     if(j < comparedJetPtBin.size() - 1) {j++;}
     } // Jet pT
   if(i < comparedRL.size() - 1) {i++;}
   } // RL

} // End script
