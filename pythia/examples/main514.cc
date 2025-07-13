#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include "fastjet/ClusterSequence.hh"
#include "TH2.h"
#include "TMath.h"
#include "TVirtualPad.h"
#include "TApplication.h"
#include "TFile.h"
#include "Pythia8/Pythia.h"

using namespace fastjet;
using namespace Pythia8;
using namespace std;

// !!!Don't forget to add to Makefile

std::string floatToString(float val, int precision = 1) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << val;
    return oss.str();
}

// Function to compute Delta R
float deltaR(const PseudoJet& p1, const PseudoJet& p2) {
  float dphi = std::abs(p1.phi() - p2.phi());
  if (dphi > M_PI) dphi = 2 * M_PI - dphi;
  float deta = p1.eta() - p2.eta();
  return std::sqrt(deta * deta + dphi * dphi);
}

 
int main(int argc, char* argv[]) {
    
    // Number of events
    int nEvents = 6000;

    //Check if a .cmnd file is provided + allows for the .cmnd file to be accepted as an argument
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <config.cmnd>" << endl;
        return 1;
    } 
    
    TFile *out = new TFile("../rootFiles/pythia_pp_eec_07132025.root", "RECREATE");
    if(!out){
    	cout << "This directory does not exist. Code will not be executed." << endl;
	return 1;
    }

    out->cd();
    
    cout << "Created rootFiles/pythia_pp_eec_07132025.root" << endl;
    
    Pythia pythia;
    
    if (!pythia.readFile(argv[1])) {
        cerr << "Error: Could not read file.cmnd!" << endl;
        return 1;
    }
    
    pythia.init();
    
    float jet_radius = 0.4;
    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, jet_radius);

        
    //declare power vectors
    vector<int> v1 = {1};
    
    //declare track pt cut vector
    vector<float> pcut = {0.7, 1.0, 2.0, 3.0, 4.0};
    
    //declare jet pt cut vector
    vector<int> jetPtCut = {120, 140, 160, 180, 200};
   
    //Declare-create histogram variables
    string histname;
    string histtitle;
    string pcutname;
    string pcuttitle;
    TH2F* hists[jetPtCut.size()-1][v1.size()];
    //TH2F* charged_hist[0];
    TH2F* pcut_hists[pcut.size()];
    TH2F* pcutregent[jetPtCut.size()-1][v1.size()][pcut.size()];

   for(int iJetPt = 0; iJetPt < jetPtCut.size() - 1; iJetPt++){
    for(int i = 0; i < v1.size(); i++){
        histname = "Charged_E" + to_string(v1[i]) + "_" + to_string(jetPtCut[iJetPt]) + "-" + to_string(jetPtCut[iJetPt+1]);
        histtitle = "Charged E" + to_string(v1[i]) + ", Jet pT: " + to_string(jetPtCut[iJetPt]) + "-" + to_string(jetPtCut[iJetPt+1]) + " histogram" ;
        hists[iJetPt][i] = new TH2F(histname.c_str(), histtitle.c_str(), 32, 0, 1.0, 32, 0, 1.0);
        cout << histname << " created" << endl;
        
        for(int j = 0; j < pcut.size(); j++){
            pcutname = "trackPt_" + floatToString(pcut[j]) + "_jetPt_" + to_string(jetPtCut[iJetPt]) + "-" + to_string(jetPtCut[iJetPt+1]) + "_Charged_E" + to_string(v1[i]);
            pcuttitle = "trackPt >= " + floatToString(pcut[j]) + ", Jet pT: " + to_string(jetPtCut[iJetPt]) + "-" + to_string(jetPtCut[iJetPt+1])+ ", Charged E" + to_string(v1[i]) + " histogram";
            pcutregent[iJetPt][i][j] = new TH2F(pcutname.c_str(), pcuttitle.c_str(), 32, 0, 1.0, 32, 0, 1.0);
            cout << pcutname << " created" << endl;
        
            } // Track pT
          } // Weight exponent
      } // Jet pt

//    charged_hist[0] = new TH1F("charged-delr", "charged delta r", 20, -2, 2);

    //Event loop
    for (int iEvent = 0; iEvent < nEvents; ++iEvent) {  
        if (!pythia.next()) continue;
        
        vector<fastjet::PseudoJet> event;
        
        vector<fastjet::PseudoJet> neutral_particles;
        vector<fastjet::PseudoJet> charged_particles;
        
        for (int i = 0; i < pythia.event.size(); ++i) {
            Particle& p = pythia.event[i];
            if (p.isFinal() && p.pT() > 0.3) {
	            event.push_back( PseudoJet(p.px(), p.py(), p.pz(), p.e()));
	            }
	        if(p.isCharged()){
	            charged_particles.push_back( PseudoJet(p.px(), p.py(), p.pz(), p.e()));
	        }
	        if(p.isNeutral()){
                neutral_particles.push_back( PseudoJet(p.px(), p.py(), p.pz(), p.e()));
	        }
            }
        
        fastjet::ClusterSequence cs(event, jet_def);
        vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt(cs.inclusive_jets());
        fastjet::ClusterSequence cs1(neutral_particles, jet_def);
        vector<fastjet::PseudoJet> neutral_jets = fastjet::sorted_by_pt(cs1.inclusive_jets());
        fastjet::ClusterSequence cs2(charged_particles, jet_def);
        vector<fastjet::PseudoJet> charged_jets = fastjet::sorted_by_pt(cs2.inclusive_jets());
        
        // Require atleast one jet
        if (jets.size() < 1) 
            continue;

        //Jet pT cut
	for(int iJetPt = 0; iJetPt < jetPtCut.size() - 1; iJetPt++){
        if(jets.at(0).pt() < jetPtCut[iJetPt] || jets.at(0).pt() > jetPtCut[iJetPt+1]) {continue;}
        
        vector<fastjet::PseudoJet> jet_constituents = jets.at(0).constituents();
        vector<fastjet::PseudoJet> neutral_constituents = neutral_jets.at(0).constituents();
        vector<fastjet::PseudoJet> charged_constituents = charged_jets.at(0).constituents();
        
        //Declaration for particles within a certain radius "_rad"
        
        vector<fastjet::PseudoJet> charged_rad;
        for (size_t c_i = 0; c_i < charged_particles.size(); c_i++){
            if (jets.at(0).delta_R(charged_particles.at(c_i)) < 0.8){ // Should this be < 0.8 && > 0.4 for track pt stuff?
                charged_rad.push_back(charged_particles.at(c_i));
            }
        }
        
        vector<fastjet::PseudoJet> neutral_rad;
        for (size_t c_i = 0; c_i < neutral_particles.size(); c_i++){
            if (jets.at(0).delta_R(neutral_particles.at(c_i)) < 0.8){
                neutral_rad.push_back(neutral_particles.at(c_i));
            }
        }

        for(int k = 0; k < v1.size(); k++){
            for (size_t i = 0; i < charged_rad.size(); i++) {
                for (size_t j = i + 1; j < charged_rad.size(); j++) {
		    for(size_t m = j + 1; m < charged_rad.size(); m++) {
                    
                    if (charged_rad.at(i).pt() > 1 && charged_rad.at(j).pt() > 1 && charged_rad.at(m).pt() > 1){
                        float eec = pow(charged_rad.at(i).pt(), v1[k]) * pow(charged_rad.at(j).pt(), v1[k]) * pow(charged_rad.at(m).pt(), v1[k]); // These powers are different than Jussi's - doesn't matter bc I'm just doing 1 rn

			// Determine RL, RM, RS
                        float deltaR12 = charged_rad.at(i).delta_R(charged_rad.at(j));
                        float deltaR13 = charged_rad.at(i).delta_R(charged_rad.at(m));
                        float deltaR23 = charged_rad.at(j).delta_R(charged_rad.at(m));
			
			std::array<float, 3> dRs = {deltaR12, deltaR13, deltaR23};
			std::sort(dRs.begin(), dRs.end());
			float RS = dRs[0];
			float RM = dRs[1];
			float RL = dRs[2];
	                
			// Calculate (x, y) for projection
			float projRL = 1.0; // 1 = RL / RL
			float projRM = RM / RL;
			float projRS = RS / RL; 
			
			float xCoord = (1 - (projRS * projRS) + (projRM * projRM)) / 2;
			float yCoord = TMath::Sqrt((projRM * projRM) - (xCoord * xCoord));
                        
			// Epow Epow filling
                        hists[iJetPt][k]->Fill(xCoord, yCoord, eec);
                       
                        const double& crpti = charged_rad.at(i).pt();
                        const double& crptj = charged_rad.at(j).pt();
                        const double& crptm = charged_rad.at(m).pt();
        
			
			for(int iTrackPt = 0; iTrackPt < pcut.size(); iTrackPt++){
				if (crpti >= pcut[iTrackPt] && crptj >= pcut[iTrackPt] && crptm >= pcut[iTrackPt]){
	                            pcutregent[iJetPt][k][iTrackPt]->Fill(xCoord, yCoord, eec);
	                            }
			}

			// Fill again with xCoord flipped over x=0.5
			xCoord = 1 -((1 - (projRS * projRS) + (projRM * projRM)) / 2); // x' = 1 - x
			
			// Epow Epow filling
                        hists[iJetPt][k]->Fill(xCoord, yCoord, eec);
                        
			for(int iTrackPt = 0; iTrackPt < pcut.size(); iTrackPt++){
				if ((crpti <= 1.0)  && (crptj <= 1.0) && (crptm <= 1.0)){ 
			  		cout << Form("%lf, %lf, %lf", crpti, crptj, crptm) << endl;}
				if (crpti >= pcut[iTrackPt] && crptj >= pcut[iTrackPt] && crptm >= pcut[iTrackPt]){
			  	    //cout << Form("%d, %d, %d", crpti, crptj, crptm) << endl;
	                            pcutregent[iJetPt][k][iTrackPt]->Fill(xCoord, yCoord, eec);
				    }
			}

                        
	              } // Charged pt > 1
                    } // First particle
		  } // Second particle
                } // Third particle
              } // Weight exponent
	    } // Jet pT loop

//        for (int cj_i = 0; cj_i < charged_jets.size(); ++cj_i) {
//            for (int nj_i = 0; nj_i < neutral_jets.size(); ++nj_i) {
//                float chargedelr = deltaR(charged_jets.at(cj_i), neutral_jets.at(nj_i));
//                charged_hist[0]->Fill(TMath::Log(chargedelr));
//                //charged_hist[0]->Fill(chargedelr);
//            }
//        }
   
   } //Event loop close
    
    out->Write();
  
    // Hist debugging
    for(int iJetPt = 0; iJetPt < jetPtCut.size() - 1; iJetPt++){
    	for(int i = 0; i < v1.size(); i++){
	  for(int iTrackPt = 0; iTrackPt < pcut.size(); iTrackPt++){
   		 cout << pcutregent[iJetPt][i][iTrackPt]->GetName() << ": " << pcutregent[iJetPt][i][iTrackPt]->GetEntries() << endl;
    }
    }
    }

    //Pythia cleanup
    pythia.stat();
    out->Close();
    return 0;    
        
}//Main close
