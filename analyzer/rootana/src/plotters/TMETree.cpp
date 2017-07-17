#include "TMETree.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
#include <cmath>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

TMETree::TMETree(modules::options* opts):
  BaseModule("TMETree",opts){
}

TMETree::~TMETree(){
}

int TMETree::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fAllDets.push_back(IDs::channel (kSiT_1 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiT_2 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiT_3 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiT_4 , kNotApplicable ));

    //    fAllDets.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_5 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_6 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_7 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_8 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_9 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_10 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_11 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_12 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_13 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_14 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL1_15 , kNotApplicable ));
    //    fAllDets.push_back(IDs::channel (kSiL1_16 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiL3 , kNotApplicable ));
    //    fAllDets.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));

    fAllDets.push_back(IDs::channel (kSiR1_1, kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR1_2, kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR2 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR3 , kNotApplicable ));

    fAllDets.push_back(IDs::channel (kGeLoGain , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kGeHiGain , kNotApplicable ));


    //    fDetNames = new std::vector<std::string>;
    //    fEnergies = new std::vector<double>;
    //    fTimes = new std::vector<double>;

    fTMETree = new TTree("TMETree", "");
    fTMETree->Branch("runId", &fRunId);
    fTMETree->Branch("blockId", &fBlockId);
    fTMETree->Branch("TMEId", &fTMEId);

    fTMETree->Branch("TMEWindowWidth", &fTMEWindowWidth);
    fTMETree->Branch("centralMuonChannel", &fCentralMuonChannel);
    fTMETree->Branch("centralMuonTPIID", &fCentralMuonTPIID);
    fTMETree->Branch("centralMuonEnergy", &fCentralMuonEnergy);
    fTMETree->Branch("centralMuonTime", &fCentralMuonTime);

    fTMETree->Branch("timeToPrevTME", &fTimeToPrevTME);
    fTMETree->Branch("timeToNextTME", &fTimeToNextTME);
    fTMETree->Branch("anyDoubleCountedPulses", &fAnyDoubleCountedPulses);
    
    for (DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){
      
      const std::string& i_detname = i_det->str();

      std::string branchname = modules::parser::ToCppValid(i_detname);
      fTMETree->Branch(branchname.c_str(), &fChannels[i_detname]);

      branchname = modules::parser::ToCppValid(i_detname+"_nPulses");
      fTMETree->Branch(branchname.c_str(), &fNPulses[i_detname]);
    }
    //    fTMETree->Branch("NPulses", &fNPulses);
    //    fTMETree->Branch("DetName", fDetNames);
    //    fTMETree->Branch("Energy", fEnergies);
    //    fTMETree->Branch("Time", fTimes);

  fRunId = SetupNavigator::Instance()->GetRunNumber();

  return 0;
}

int TMETree::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  fBlockId = EventNavigator::Instance().EntryNo();

  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){

    fTMEId = i_tme - gMuonEvents.begin();
    fTMEWindowWidth = (*i_tme)->GetWindowWidth();

    const TDetectorPulse* central_muon = (*i_tme)->GetCentralMuon();
    fCentralMuonChannel = central_muon->GetSource().Channel().str();
    fCentralMuonTPIID = central_muon->GetTAP(TDetectorPulse::kSlow)->GetParentID();
    fCentralMuonEnergy = central_muon->GetEnergy();
    fCentralMuonTime = central_muon->GetTime();

    fTimeToPrevTME = -1;
    fTimeToNextTME = -1;
    if (i_tme != gMuonEvents.begin()) {
      fTimeToPrevTME = std::fabs(fCentralMuonTime - (*(i_tme-1))->GetCentralMuon()->GetTime());
    }
    if (i_tme != gMuonEvents.end()-1) {
      fTimeToNextTME = std::fabs(fCentralMuonTime - (*(i_tme+1))->GetCentralMuon()->GetTime());
    }
    
    fAnyDoubleCountedPulses = false;

    for(DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){
      
      const std::string& i_detname = i_det->str();
      fNPulses[i_detname] = 0;
      fChannels[i_detname].clear();
      
      int i_det_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (i_det_source_index<0) {
	continue; // this TME has no sources for this channel
      }
      const IDs::source& i_det_source=(*i_tme)->GetSource(i_det_source_index);
	
      // Loop through all the pulses
      int n_pulses_i_det = (*i_tme)->NumPulses(i_det_source);
      fNPulses[i_detname] = n_pulses_i_det;
      for(int i=0; i<n_pulses_i_det; ++i){ 
	const TDetectorPulse* tdp=(*i_tme)->GetPulse(i_det_source,i);

	SimplePulse* pulse = new SimplePulse();
	pulse->tpi_id = tdp->GetTAP(TDetectorPulse::kSlow)->GetParentID();
	pulse->E = tdp->GetEnergy();
	pulse->Amp = tdp->GetAmplitude();
	pulse->tblock = tdp->GetTime();
	pulse->tTME = (tdp->GetTime() - fCentralMuonTime);

	//	std::cout << &fEnergies[i_detname] << std::endl;
	//	std::cout << "Pulse #" << i << ", " << i_detname << ", " << tdp->GetEnergy() << ", " << *(fEnergies[i_detname].end()-1) << std::endl;


	// Now check to see if we have put this pulse in another TME (it will be either the TME before or the TME after)
	int n_ends = 2; //  want to check each "end" of the TME
	for (int i_end = 0; i_end < n_ends; ++i_end) {
	  MuonEventList::const_iterator j_tme;
	  if (i_end == 0) {
	    if (i_tme == gMuonEvents.begin()) {
	      continue;
	    }
	    j_tme = i_tme-1;
	  }
	  else if (i_end == 1) {
	    if (i_tme == gMuonEvents.end()-1) {
	      continue;
	    }
	    j_tme = i_tme+1;
	  }

	  int j_det_source_index=(*j_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
	  if (j_det_source_index<0) {
	    continue; // this TME has no sources for this channel
	  }
	  const IDs::source& j_det_source=(*j_tme)->GetSource(j_det_source_index);

	  int n_pulses_prev_tme = (*j_tme)->NumPulses(j_det_source);
	  if (n_pulses_prev_tme == 0) {
	    continue; // we are fine
	  }
	  else {
	    for(int j=0; j<n_pulses_prev_tme; ++j){ 
	      const TDetectorPulse* prev_tdp=(*j_tme)->GetPulse(j_det_source,j);
	      
	      if (prev_tdp->GetTAP(TDetectorPulse::kSlow)->GetParentID() == tdp->GetTAP(TDetectorPulse::kSlow)->GetParentID()) {
		pulse->bit_mask.SetBitNumber(SimplePulse::kDoubleCounted);
		fAnyDoubleCountedPulses = true;
	      }
	    }
	  }
	}

	fChannels[i_detname].push_back(pulse);
      }
    }

    fTMETree->Fill();
  }
  return 0;
}

int TMETree::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TMETree);
