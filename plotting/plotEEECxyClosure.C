#include "EECHistogramManager.h" R__LOAD_LIBRARY(plotting/DrawingClasses.so)
#include "EECCard.h"
#include "JDrawer.h"
#include "../src/EECHistograms.h"
#include "EECDrawer.h"

/*
 * Macro for plotting ratios of E3C projected into XY plane. Compares different data sources, not bins. 
 * User *Comparison*.C for track pt bin comparisons. 
 */
void plotEEECxyClosure(){

  // ====================================================
  //  Frequently changed characteristics
  // ====================================================

  bool allRL = true;
  bool logAxis = false;
  bool normalizeArea = false; // Normalizes by area. Defaults to normalizing by NJets
  bool partonShower = true; // Compares Simple, DIRE, VINCIA 
  bool dataSource = false; // Compares justPythia, pythiaGeant, CMS
  bool validation = true; // Compares parton showers to CMS 
  bool invert = false; // Plots the reverse comparisons
  int rebin = 1; // Rebins histograms. Default is 1, IE no rebinning

  if(partonShower + dataSource != 1 || dataSource + validation != 1) { 
  	cout << "Please select either partonShower, dataSource, or validation." << endl;
	return;
  }

  TString dateFileString = "07222025";
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
 std::vector<std::pair<const char*, const char*>> closureDataLabels;
 std::vector<TH1D*> closurePartonShower;
 std::vector<std::pair<const char*, const char*>> closurePartonShowerLabels;
 std::vector<TH1D*> closureValidation;
 std::vector<std::pair<const char*, const char*>> closureValidationLabels;

   i = 0;
   for(auto RLBin : comparedRL){
     j = 0;
     for(auto jetPtBin : comparedJetPtBin){
        k = 0;
	for(auto trackPtBin : comparedTrackPtBin){

  	
	if(dataSource){
  	  TString inputFileJustPythia = "ppMC2017/justPythia/jetRadius_0.8/projected_xyPlane_RL-All_06302025.root"; 
	  TString inputFilePythiaGeant = "ppMC2017/pythiaGeant/projected_xyPlane_Geant_RL-All_07182025.root"; 
	  TString inputFileCMS = "pp2017/projected_xyPlane_NoTrigger_RL-All_07182025.root";
	  
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
	   	  int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorFull;
	   	  int iPairingType = EECHistograms::kSameJetPair;
	   	  int iSubevent = EECHistograms::knSubeventCombinations;
	   	  
	   	  // Create two NULL histogram objects
	   	  TH2D* hEnergyEnergyEnergyCorrelatorJustPythia[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with justPythia value
	   	  TH2D* hEnergyEnergyEnergyCorrelatorPythiaGeant[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with pythiaGeant value
	   	  TH2D* hEnergyEnergyEnergyCorrelatorCMS[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with CMS value
	          TH1D* justPythiaPythiaGeant;
	          TH1D* justPythiaCMS;
	          TH1D* PythiaGeantCMS;
	   
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
	   	
	   		hEnergyEnergyEnergyCorrelatorJustPythia[iJetPtJustPythia][iTrackPtJustPythia] = histogramsJustPythia->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPtJustPythia, iTrackPtJustPythia, iPairingType, iSubevent);
	   	
	   		int yLowBinJustPythia = hEnergyEnergyEnergyCorrelatorJustPythia[iJetPtJustPythia][iTrackPtJustPythia]->GetYaxis()->FindBin(0.0);
	   		int yHighBinJustPythia = hEnergyEnergyEnergyCorrelatorJustPythia[iJetPtJustPythia][iTrackPtJustPythia]->GetYaxis()->GetNbins();
	   
	   		// Project histograms
	   		TH1D* projEEECJustPythia = hEnergyEnergyEnergyCorrelatorJustPythia[iJetPtJustPythia][iTrackPtJustPythia]->ProjectionX("projEEECJustPythia", yLowBinJustPythia, yHighBinJustPythia);
	   
	   		// Normalize them
			if(normalizeArea){
	   			double integralJustPythia = projEEECJustPythia->Integral("width");
	   			projEEECJustPythia->Scale(1.0 / integralJustPythia);
			} else {
				std::pair<double,double> jetPtBinBordersJustPythia = cardJustPythia->GetBinBordersJetPtEEC(iJetPtJustPythia);
        			double nJetsJustPythia = histogramsJustPythia->GetJetPtIntegral(iCentrality, jetPtBinBordersJustPythia.first, jetPtBinBordersJustPythia.second);
	   			projEEECJustPythia->Scale(1.0 / nJetsJustPythia);
			}
			projEEECJustPythia->Rebin(rebin);
	   	
	   	// pythiaGeant
	   		int iJetPtPythiaGeant = cardPythiaGeant->FindBinIndexJetPtEEC(jetPtBin);
	   	        int iTrackPtPythiaGeant = cardPythiaGeant->GetBinIndexTrackPtEEC(trackPtBin);
	   	
	   		hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPtPythiaGeant][iTrackPtPythiaGeant] = histogramsPythiaGeant->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPtPythiaGeant, iTrackPtPythiaGeant, iPairingType, iSubevent);
	   	
	   		int yLowBinPythiaGeant = hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPtPythiaGeant][iTrackPtPythiaGeant]->GetYaxis()->FindBin(0.0);
	   		int yHighBinPythiaGeant = hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPtPythiaGeant][iTrackPtPythiaGeant]->GetYaxis()->GetNbins();
	   	
	   		// Project histograms
	   		TH1D* projEEECPythiaGeant = hEnergyEnergyEnergyCorrelatorPythiaGeant[iJetPtPythiaGeant][iTrackPtPythiaGeant]->ProjectionX("projEEECPythiaGeant", yLowBinPythiaGeant, yHighBinPythiaGeant);
	   	
	   		// Normalize them
			if(normalizeArea){
	   			double integralPythiaGeant = projEEECPythiaGeant->Integral("width");
	   			projEEECPythiaGeant->Scale(1.0 / integralPythiaGeant);
			} else {	
				std::pair<double,double> jetPtBinBordersPythiaGeant = cardPythiaGeant->GetBinBordersJetPtEEC(iJetPtPythiaGeant);
        			double nJetsPythiaGeant = histogramsPythiaGeant->GetJetPtIntegral(iCentrality, jetPtBinBordersPythiaGeant.first, jetPtBinBordersPythiaGeant.second);
	   			projEEECPythiaGeant->Scale(1.0 / nJetsPythiaGeant);
			}
			projEEECPythiaGeant->Rebin(rebin);
	   	
	   	// CMS
	   		int iJetPtCMS = cardCMS->FindBinIndexJetPtEEC(jetPtBin);
	   	        int iTrackPtCMS = cardCMS->GetBinIndexTrackPtEEC(trackPtBin);
	   	
	   		hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS] = histogramsCMS->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPtCMS, iTrackPtCMS, iPairingType, iSubevent);
	   	
	   		int yLowBinCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS]->GetYaxis()->FindBin(0.0);
	   		int yHighBinCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS]->GetYaxis()->GetNbins();

	   	
	   		// Project histograms
	   		TH1D* projEEECCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS]->ProjectionX("projEEECCMS", yLowBinCMS, yHighBinCMS);
	   	
	   		// Normalize them
	   		if(normalizeArea){
				double integralCMS = projEEECCMS->Integral("width");
	   			projEEECCMS->Scale(1.0 / integralCMS);
			} else {
				std::pair<double,double> jetPtBinBordersCMS = cardCMS->GetBinBordersJetPtEEC(iJetPtCMS);
        			double nJetsCMS = histogramsCMS->GetJetPtIntegral(iCentrality, jetPtBinBordersCMS.first, jetPtBinBordersCMS.second);
	   			projEEECCMS->Scale(1.0 / nJetsCMS);
			}
			projEEECCMS->Rebin(rebin);
	   	
	   	// Calculate comparisons
		if(invert){
		   	justPythiaPythiaGeant = (TH1D*) projEEECPythiaGeant->Clone("justPythiaPythiaGeant");
		   	justPythiaPythiaGeant->Divide(projEEECJustPythia);
		   	
		   	justPythiaCMS = (TH1D*) projEEECCMS->Clone("justPythiaCMS");
		   	justPythiaCMS->Divide(projEEECJustPythia);
		   	
		   	PythiaGeantCMS = (TH1D*) projEEECCMS->Clone("PythiaGeantCMS");
		   	PythiaGeantCMS->Divide(projEEECPythiaGeant);
		} else{
		   	justPythiaPythiaGeant = (TH1D*) projEEECJustPythia->Clone("justPythiaPythiaGeant");
		   	justPythiaPythiaGeant->Divide(projEEECPythiaGeant);
		   	
		   	justPythiaCMS = (TH1D*) projEEECJustPythia->Clone("justPythiaCMS");
		   	justPythiaCMS->Divide(projEEECCMS);
		   	
		   	PythiaGeantCMS = (TH1D*) projEEECPythiaGeant->Clone("PythiaGeantCMS");
		   	PythiaGeantCMS->Divide(projEEECCMS);
		}
	
        closureDataSource.clear();
	closureDataSource.push_back(justPythiaPythiaGeant);
	closureDataSource.push_back(justPythiaCMS);
	closureDataSource.push_back(PythiaGeantCMS);

        closureDataLabels.clear();
	if(invert){
		closureDataLabels.push_back(std::make_pair("Pythia-Geant","Pythia"));
		closureDataLabels.push_back(std::make_pair("CMS","Pythia"));
		closureDataLabels.push_back(std::make_pair("CMS","Pythia-Geant"));
	} else {
		closureDataLabels.push_back(std::make_pair("Pythia","Pythia-Geant"));
		closureDataLabels.push_back(std::make_pair("Pythia","CMS"));
		closureDataLabels.push_back(std::make_pair("Pythia-Geant","CMS"));
		
        }
      }	
	
      if(partonShower){

	  
	  TString inputFileDire = "pythia/rootFiles/pythia_pp_eec_direWeighted_6000events_07212025.root";
  	  TString inputFileSimple = "pythia/rootFiles/pythia_pp_eec_simple_6000events_07162025.root";
  	  TString inputFileVincia = "pythia/rootFiles/pythia_pp_eec_vincia_6000events_07162025.root";
	 
	  // Open the input files
	  TFile* inputDire = TFile::Open(inputFileDire);
	  TFile* inputSimple = TFile::Open(inputFileSimple);
	  TFile* inputVincia = TFile::Open(inputFileVincia);
	  
	  if(inputDire == NULL || inputSimple == NULL || inputVincia == NULL){
	    cout << "Error! One of the parton shower files does not exist!" << endl;
	    cout << "Will not execute the code" << endl;
	    return;
	  }
	
	TString histname = Form("trackPt_%.1f_jetPt_%.0f-%.0f_Charged_E1", comparedTrackPtBin.at(k), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
      	
	// Simple
		TH2F* histSimple =  (TH2F*) inputSimple->Get(histname);
		int yLowBinSimple = histSimple->GetYaxis()->FindBin(0.0);
		int yHighBinSimple = histSimple->GetYaxis()->GetNbins();
	
		// Project histograms
		TH1D* projEEECSimple = histSimple->ProjectionX("projEEECSimple", yLowBinSimple, yHighBinSimple);
	
		// Normalize them
		if(normalizeArea){
			double integralSimple = projEEECSimple->Integral("width");
			projEEECSimple->Scale(1.0 / integralSimple);
		} else{
			TParameter<int>* paramSimple = (TParameter<int>*) inputSimple->Get("numJets");
			double nJetsSimple = (double) paramSimple->GetVal();
			projEEECSimple->Scale(1.0 / nJetsSimple);
		}
		projEEECSimple->Rebin(rebin);
	// Dire
		TH2F* histDire =  (TH2F*) inputDire->Get(histname);
		int yLowBinDire = histDire->GetYaxis()->FindBin(0.0);
		int yHighBinDire = histDire->GetYaxis()->GetNbins();
	
		// Project histograms
		TH1D* projEEECDire = histDire->ProjectionX("projEEECDire", yLowBinDire, yHighBinDire);
	
		// Normalize them
		if(normalizeArea){
			double integralDire = projEEECDire->Integral("width");
			projEEECDire->Scale(1.0 / integralDire);
		} else {
			TParameter<int>* paramDire = (TParameter<int>*) inputDire->Get("numJets");
			double nJetsDire = (double) paramDire->GetVal();
			projEEECDire->Scale(1.0 / nJetsDire);
		}
		projEEECDire->Rebin(rebin);

	// Vincia
		TH2F* histVincia =  (TH2F*) inputVincia->Get(histname);
		int yLowBinVincia = histVincia->GetYaxis()->FindBin(0.0);
		int yHighBinVincia = histVincia->GetYaxis()->GetNbins();
			
		// Project histograms
		TH1D* projEEECVincia = histVincia->ProjectionX("projEEECVincia", yLowBinVincia, yHighBinVincia);
	
		// Normalize them
		if(normalizeArea){
			double integralVincia = projEEECVincia->Integral("width");
			projEEECVincia->Scale(1.0 / integralVincia);
		} else {
			TParameter<int>* paramVincia = (TParameter<int>*) inputVincia->Get("numJets");
			double nJetsVincia = (double) paramVincia->GetVal();
			projEEECVincia->Scale(1.0 / nJetsVincia);
		}
		projEEECVincia->Rebin(rebin);

	// Comparisons
	TH1D* simpleDire;
	TH1D* simpleVincia;
	TH1D* direVincia;

	if(invert){
		simpleDire = (TH1D*) projEEECDire->Clone("simpleDire");
		simpleDire->Divide(projEEECSimple);
		
		simpleVincia = (TH1D*) projEEECVincia->Clone("simpleVincia");
		simpleVincia->Divide(projEEECSimple);
		
		direVincia = (TH1D*) projEEECVincia->Clone("direVincia");
		direVincia->Divide(projEEECDire);
	} else {
		simpleDire = (TH1D*) projEEECSimple->Clone("simpleDire");
		simpleDire->Divide(projEEECDire);
		
		simpleVincia = (TH1D*) projEEECSimple->Clone("simpleVincia");
		simpleVincia->Divide(projEEECVincia);
		
		direVincia = (TH1D*) projEEECDire->Clone("direVincia");
		direVincia->Divide(projEEECVincia);
	}

	closurePartonShower.clear();
	closurePartonShower.push_back(simpleDire);
	closurePartonShower.push_back(simpleVincia);
	closurePartonShower.push_back(direVincia);
	
	closurePartonShowerLabels.clear();
	if(invert){
		closurePartonShowerLabels.push_back(std::make_pair("DIRE","Simple"));
		closurePartonShowerLabels.push_back(std::make_pair("VINCIA","Simple"));
		closurePartonShowerLabels.push_back(std::make_pair("VINCIA","DIRE"));
	} else {
		closurePartonShowerLabels.push_back(std::make_pair("Simple","DIRE"));
		closurePartonShowerLabels.push_back(std::make_pair("Simple","VINCIA"));
		closurePartonShowerLabels.push_back(std::make_pair("DIRE","VINCIA"));
	}

	if(validation){
		TString inputFileCMS = "pp2017/projected_xyPlane_NoTrigger_RL-All_07182025.root";
		TFile* inputCMS = TFile::Open(inputFileCMS);
		EECCard* cardCMS = new EECCard(inputCMS);
		const int nCentralityBinsEEC = cardCMS->GetNCentralityBins();
	   	const int nJetPtBinsEEC = cardCMS->GetNJetPtBinsEEC();
	   	const int nTrackPtBinsEEC = cardCMS->GetNTrackPtBinsEEC();
		EECHistogramManager* histogramsCMS = new EECHistogramManager(inputCMS,cardCMS);
		int iCentrality = 0;
	   	int iEnergyEnergyCorrelatorType = EECHistogramManager::kEnergyEnergyEnergyCorrelatorFull;
	   	int iPairingType = EECHistograms::kSameJetPair;
	   	int iSubevent = EECHistograms::knSubeventCombinations;
		TH2D* hEnergyEnergyEnergyCorrelatorCMS[nJetPtBinsEEC][nTrackPtBinsEEC]; // Fill with CMS value
		int iJetPtCMS = cardCMS->FindBinIndexJetPtEEC(jetPtBin);
	   	int iTrackPtCMS = cardCMS->GetBinIndexTrackPtEEC(trackPtBin);

   		hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS] = histogramsCMS->GetHistogramEnergyEnergyEnergyCorrelatorFull(iEnergyEnergyCorrelatorType, iCentrality, iJetPtCMS, iTrackPtCMS, iPairingType, iSubevent);

   		int yLowBinCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS]->GetYaxis()->FindBin(0.0);
   		int yHighBinCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS]->GetYaxis()->GetNbins();

   		// Project histograms
   		TH1D* projEEECCMS = hEnergyEnergyEnergyCorrelatorCMS[iJetPtCMS][iTrackPtCMS]->ProjectionX("projEEECCMS", yLowBinCMS, yHighBinCMS);

   		// Normalize them
   		if(normalizeArea){
			double integralCMS = projEEECCMS->Integral("width");
   			projEEECCMS->Scale(1.0 / integralCMS);
		} else {
			std::pair<double,double> jetPtBinBordersCMS = cardCMS->GetBinBordersJetPtEEC(iJetPtCMS);
       			double nJetsCMS = histogramsCMS->GetJetPtIntegral(iCentrality, jetPtBinBordersCMS.first, jetPtBinBordersCMS.second);
   			projEEECCMS->Scale(1.0 / nJetsCMS);
		}
		projEEECCMS->Rebin(rebin);

	   // Calculate comparisons 
	   TH1D* simpleCMS;
	   TH1D* direCMS;
	   TH1D* vinciaCMS;

	   if(invert){
           	simpleCMS = (TH1D*) projEEECCMS->Clone("simpleCMS");
		simpleCMS->Divide(projEEECSimple);
           	
		direCMS = (TH1D*) projEEECCMS->Clone("direCMS");
		direCMS->Divide(projEEECDire);
		
		vinciaCMS = (TH1D*) projEEECCMS->Clone("vinciaCMS");
		vinciaCMS->Divide(projEEECVincia);

		closureValidation.clear();
		closureValidation.push_back(simpleCMS);
		closureValidation.push_back(direCMS);
		closureValidation.push_back(vinciaCMS);
		
		closureValidationLabels.clear();
		closureValidationLabels.push_back(std::make_pair("CMS","Simple"));
		closureValidationLabels.push_back(std::make_pair("CMS","DIRE"));
		closureValidationLabels.push_back(std::make_pair("CMS","VINCIA"));
	   } else {
           	simpleCMS = (TH1D*) projEEECSimple->Clone("simpleCMS");
		simpleCMS->Divide(projEEECCMS);
           	
		direCMS = (TH1D*) projEEECDire->Clone("direCMS");
		direCMS->Divide(projEEECCMS);
		
		vinciaCMS = (TH1D*) projEEECVincia->Clone("vinciaCMS");
		vinciaCMS->Divide(projEEECCMS);

		closureValidation.clear();
		closureValidation.push_back(simpleCMS);
		closureValidation.push_back(direCMS);
		closureValidation.push_back(vinciaCMS);
		
		closureValidationLabels.clear();
		closureValidationLabels.push_back(std::make_pair("Simple","CMS"));
		closureValidationLabels.push_back(std::make_pair("DIRE","CMS"));
		closureValidationLabels.push_back(std::make_pair("VINCIA","CMS"));

	   }
	}
      }

      // ====================================================
      // Plot each E3C
      // ====================================================
	

	// Prepare a JDrawer for drawing purposes
	  JDrawer *drawer = new JDrawer();
	  if(logAxis) {drawer->SetLogY(true);}
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

	  TString jetPtFileString = Form("J=%.0f-%.0f", comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second);
	  TString RLFileString = Form("RL=%.1f-%.1f", comparedRL.at(i).first, comparedRL.at(i).second);
	 
	  if(allRL) {RLFileString = Form("RL=All");}
	  
	  TString trackPtFileString = Form("T=%.1f", comparedTrackPtBin.at(0));
	  TString ptTitleString = "";
	  TString normalMethodString = "N_{Jets}";
	  if(normalizeArea) {normalMethodString = "Area";}
	  TString normalFileString = "normalNJets";
	  if(normalizeArea) {normalFileString = "normalArea";}
	  TString compareTitleString = "";
	  TString commentFileString = "";
	  if(partonShower) {commentFileString = "partonShowers";}
	  if(dataSource) {commentFileString = "dataSource";}
	  if(validation) {commentFileString = "validation";}
	  if(invert) {commentFileString += "_inverted";}
	  if(rebin != 1) {commentFileString += "_rebinned";}
	  TString logFileString = "";
	  if(logAxis) {logFileString = "_logAxis";}
	  TString axisFileString = "closure"; 
	
	  ptTitleString = Form("0 < Y < 1.0, R_{Jet} = %s, %.0f GeV < Jet p_{T} < %.0f GeV, Normalized by %s", jetRadiusString.Data(), comparedJetPtBin.at(j).first, comparedJetPtBin.at(j).second, normalMethodString.Data());
	
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
		closurePartonShower[0]->SetMinimum(0.5); 
		closurePartonShower[0]->SetMaximum(1.5); 

		for(int iHist = 0; iHist < closurePartonShower.size(); iHist ++){
	   	   closurePartonShower[iHist]->SetLineColor(color[iHist]);
                   closurePartonShower[iHist]->SetMarkerStyle(markerStyle[iHist]);
                   closurePartonShower[iHist]->SetMarkerColor(color[iHist]);
                   closurePartonShower[iHist]->SetLineWidth(1);
		   if(iHist == 0){
		   	drawer->DrawHistogram(closurePartonShower[iHist], "X", "Ratio", ptTitleString);
		   } else {
		        closurePartonShower[iHist]->Draw("SAME");
		   }

		  TString legendEntry = Form("#frac{%s}{%s}", closurePartonShowerLabels.at(iHist).first, closurePartonShowerLabels.at(iHist).second);
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
		closureDataSource[0]->SetMinimum(0.7); 
		closureDataSource[0]->SetMaximum(1.3); 

		legend->SetHeader("Data Sources", "c");

		for(int iHist = 0; iHist < closureDataSource.size(); iHist ++){
	   	   closureDataSource[iHist]->SetLineColor(color[iHist]);
                   closureDataSource[iHist]->SetMarkerStyle(markerStyle[iHist]);
                   closureDataSource[iHist]->SetMarkerColor(color[iHist]);
                   closureDataSource[iHist]->SetLineWidth(1);
		   if(iHist == 0){
		   	drawer->DrawHistogram(closureDataSource[iHist], "X", "Ratio", ptTitleString);
		   } else {
		        closureDataSource[iHist]->Draw("SAME");
		   }

		  TString legendEntry = Form("#frac{%s}{%s}", closureDataLabels.at(iHist).first, closureDataLabels.at(iHist).second);
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

		closureValidation[0]->SetMinimum(0); 
		closureValidation[0]->SetMaximum(5); 

		for(int iHist = 0; iHist < closureValidation.size(); iHist ++){
	   	   closureValidation[iHist]->SetLineColor(color[iHist]);
                   closureValidation[iHist]->SetMarkerStyle(markerStyle[iHist]);
                   closureValidation[iHist]->SetMarkerColor(color[iHist]);
                   closureValidation[iHist]->SetLineWidth(1);
		   if(iHist == 0){
		   	drawer->DrawHistogram(closureValidation[iHist], "X", "Ratio", ptTitleString);
		   } else {
		        closureValidation[iHist]->Draw("SAME");
		   }

		  TString legendEntry = Form("#frac{%s}{%s}", closureValidationLabels.at(iHist).first, closureValidationLabels.at(iHist).second);
		  legend->AddEntry(closureValidation[iHist], legendEntry, "p");
		
		}
	}

	legend->Draw();
	
	// Add guide lines
	TLine *line = new TLine(0, 1.0, 1.0, 1.0);
	line->SetLineWidth(2);
   	line->SetLineColor(kGray);
   	line->SetLineStyle(2); // Dashed
	line->Draw();
	
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
