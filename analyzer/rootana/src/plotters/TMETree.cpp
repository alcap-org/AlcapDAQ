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

#include "TTemplateFitAnalysedPulse.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
#include <cmath>
#include <algorithm>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;
extern SourceDetPulseMap gDetectorPulseMap;

TMETree::TMETree(modules::options* opts):
  BaseModule("TMETree",opts){
}

TMETree::~TMETree(){
}

int TMETree::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fAllDets.push_back(IDs::channel (kMuSc , kNotApplicable ));
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

    fAllDets.push_back(IDs::channel (kSiR1_1, kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR1_2, kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR2 , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiR3 , kNotApplicable ));

    fAllDets.push_back(IDs::channel (kGeLoGain , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kGeHiGain , kNotApplicable ));


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

    fNonTMETree = new TTree("NonTMETree", "");
    fNonTMETree->Branch("runId", &fRunId);
    fNonTMETree->Branch("blockId", &fBlockId);
    
    for (DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){
      
      const std::string& i_detname = i_det->str();

      std::string branchname = modules::parser::ToCppValid(i_detname);
      fTMETree->Branch(branchname.c_str(), &fChannels[i_detname]);

      fNonTMETree->Branch(branchname.c_str(), &fChannels_NonTME[i_detname]);
    }

    fRunId = SetupNavigator::Instance()->GetRunNumber();

  return 0;
}

int TMETree::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  fBlockId = EventNavigator::Instance().EntryNo();

  // Reset the TPIsSeen vector, we want to be able to look over multiple TMEs
  for(DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){   
    const std::string& i_detname = i_det->str();
    fTPIsSeen[i_detname].clear();
    fTPIsDoubleCounted[i_detname].clear();
  }

  // First loop through all TMEs and check with TPI IDs are double counted
  // (need to do this because otherwise the first occurrance of a double counted pulse won't be flagged correctly
  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){
    for(DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){

      const std::string& i_detname = i_det->str();
      std::vector<std::pair<int,int> >& tpis_seen = fTPIsSeen[i_detname];
      std::vector<std::pair<int,int> >& tpis_doubleCounted = fTPIsDoubleCounted[i_detname];
      //      std::vector<int>& taps_cutOff = fTAPsCutOff[i_detname];

      int i_det_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (i_det_source_index<0) {
	continue; // this TME has no sources for this channel
      }
      const IDs::source& i_det_source=(*i_tme)->GetSource(i_det_source_index);
	
      // Loop through all the pulses
      int n_pulses_i_det = (*i_tme)->NumPulses(i_det_source);
      for(int i=0; i<n_pulses_i_det; ++i){ 
	const TDetectorPulse* tdp=(*i_tme)->GetPulse(i_det_source,i);
	int this_tpi_id = tdp->GetTAP(TDetectorPulse::kSlow)->GetParentID();
	int this_tap_id = tdp->GetTAP(TDetectorPulse::kSlow)->GetTAPID();

	// Check to see if this TPI has already been used in another TME
	// go in reverse since it is more likely to be immediately previously
	bool seen_before = false;
	for (std::vector<std::pair<int,int> >::const_reverse_iterator i_tpi_id = tpis_seen.rbegin(); i_tpi_id != tpis_seen.rend(); ++i_tpi_id) { 
	  if ((*i_tpi_id).first == this_tpi_id && (*i_tpi_id).second == this_tap_id) {
	    // This TPI has been seen before
	    seen_before = true;
	    tpis_doubleCounted.push_back(*i_tpi_id);
	    break;
	  }
	  else if ( (*i_tpi_id).first < this_tpi_id) {
	    // tpi ids will be inserted in order and so we can break from this loop since we know we won't find it
	    break; // from for loop
	  }
	}
	if ( !seen_before ) {
	  // not seen this before so put it in the vector
	  tpis_seen.push_back(std::pair<int,int>(this_tpi_id, this_tap_id));
	}

	// Check to see if this is a "cut-off" pulse
	/*	const TAnalysedPulse* tap = tdp->GetTAP(TDetectorPulse::kSlow);
	if (dynamic_cast<const TTemplateFitAnalysedPulse*>(tap)) {
	  const TTemplateFitAnalysedPulse* tf_pulse =static_cast<const TTemplateFitAnalysedPulse*>(tap);
	  if (tf_pulse->GetTemplateTimeOffset() > 3900) {
	    taps_cutOff.push_back(this_tpi_id);
	  }
	}
	*/
      }
    }
  }

  // Now go through and create the actual branches
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
      std::vector<SimplePulse>& i_pulse_list = fChannels[i_detname];
      i_pulse_list.clear();

      std::vector<std::pair<int,int> >& tpis_doubleCounted = fTPIsDoubleCounted[i_detname];
      //      std::vector<int>& taps_cutOff = fTAPsCutOff[i_detname];
      
      int i_det_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (i_det_source_index<0) {
	continue; // this TME has no sources for this channel
      }
      const IDs::source& i_det_source=(*i_tme)->GetSource(i_det_source_index);
	
      // Loop through all the pulses
      int n_pulses_i_det = (*i_tme)->NumPulses(i_det_source);
      for(int i=0; i<n_pulses_i_det; ++i){ 
	const TDetectorPulse* tdp=(*i_tme)->GetPulse(i_det_source,i);

	SimplePulse pulse(tdp->GetTAP(TDetectorPulse::kSlow)->GetParentID(), tdp->GetTAP(TDetectorPulse::kSlow)->GetTAPID(), tdp->GetEnergy(), tdp->GetAmplitude(), tdp->GetTime(), tdp->GetTime() - fCentralMuonTime, 0);

	for (std::vector<std::pair<int,int> >::const_iterator i_tpi_doubleCounted = tpis_doubleCounted.begin(); i_tpi_doubleCounted != tpis_doubleCounted.end(); ++i_tpi_doubleCounted) {
	  if ( pulse.tpi_id == (*i_tpi_doubleCounted).first && pulse.tap_id == (*i_tpi_doubleCounted).second) {
	    pulse.bit_mask |= kDoubleCounted;
	    fAnyDoubleCountedPulses = true;
	  }
	}
	//	for (std::vector<int>::const_iterator i_tap_cutOff = taps_cutOff.begin(); i_tap_cutOff != taps_cutOff.end(); ++i_tap_cutOff) {
	//	  if ( pulse.tpi_id == *i_tap_cutOff) {
	//	    pulse.bit_mask |= kCutOff;
	//	  }
	//	}

	// can check for other things here
	// and then if there's nothing on it, set it to OK
	if (pulse.bit_mask == 0) {
	  pulse.bit_mask |= kOK;
	}	
	//	std::cout << &fEnergies[i_detname] << std::endl;
	//	std::cout << "Pulse #" << i << ", " << i_detname << ", " << tdp->GetEnergy() << ", " << *(fEnergies[i_detname].end()-1) << std::endl;
	
	i_pulse_list.push_back(pulse);
      }
    }

    fTMETree->Fill();
  }

  // Now create the NonTMETree for pulses that aren't included in any TME
  for(SourceDetPulseMap::const_iterator i_source=gDetectorPulseMap.begin();
      i_source!= gDetectorPulseMap.end(); ++i_source){

    IDs::channel i_channel = (i_source->first).Channel();
    const std::string& i_detname = IDs::channel::GetDetectorString(i_channel.Detector());

    // Check that we want to look at this detector
    bool want = false;
    for(DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){   
      if ( i_det->str() == i_detname) {
	want = true;
	break;
      }
    }
    if (!want) {
      continue;
    }

    const DetectorPulseList& i_tdp_list = i_source->second;

    std::vector<SimplePulse>& i_nontme_pulse_list = fChannels_NonTME[i_detname];
    i_nontme_pulse_list.clear();
    std::vector<std::pair<int,int> >& tpis_seen = fTPIsSeen[i_detname];

    //    std::cout << i_tdp_list << std::endl;
    for (DetectorPulseList::const_iterator i_tdp = i_tdp_list.begin(); i_tdp != i_tdp_list.end(); ++i_tdp) {
      int i_tpi_id = (*i_tdp)->GetTAP(TDetectorPulse::kSlow)->GetParentID();
      int i_tap_id = (*i_tdp)->GetTAP(TDetectorPulse::kSlow)->GetTAPID();

      bool seen = false;
      //      std::cout << "Looking for " << i_detname << " TPI ID = " << i_tpi_id << std::endl;
      //      std::cout << "Checking... ";
      for (std::vector<std::pair<int,int> >::const_iterator i_tpi_seen = tpis_seen.begin(); i_tpi_seen != tpis_seen.end(); ++i_tpi_seen) {
	//	std::cout << *i_tpi_seen << ", ";
	if ((*i_tpi_seen).first == i_tpi_id && (*i_tpi_seen).second == i_tap_id) {
	  //	  std::cout << " Found!" << std::endl;
	  seen = true;
	  break;
	}
      }

      if (!seen) {
	//	std::cout << "Not found! Pushing back..." << std::endl;
	SimplePulse pulse(i_tpi_id, (*i_tdp)->GetTAP(TDetectorPulse::kSlow)->GetTAPID(), (*i_tdp)->GetEnergy(), (*i_tdp)->GetAmplitude(), (*i_tdp)->GetTime(), 0, 0);
	i_nontme_pulse_list.push_back(pulse);
      }
    }
  }
  fNonTMETree->Fill();
    
  return 0;
}

int TMETree::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TMETree);
