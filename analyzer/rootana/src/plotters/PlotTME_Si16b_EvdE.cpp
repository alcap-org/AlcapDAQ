#include "PlotTME_Si16b_EvdE.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

PlotTME_Si16b_EvdE::PlotTME_Si16b_EvdE(modules::options* opts):
   BaseModule("PlotTME_Si16b_EvdE",opts),fNullCount(0),fTdpCount(0){
}

PlotTME_Si16b_EvdE::~PlotTME_Si16b_EvdE(){
}

int PlotTME_Si16b_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fSiT.push_back(IDs::channel (kSiT_1 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_2 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_3 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_4 , kNotApplicable ));

    fSiR1.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));

    fSiR2 = new IDs::channel(kSiR2, kNotApplicable);
    fSiR3 = new IDs::channel(kSiR3, kNotApplicable);

    fRightArm.fLayer1 = fSiR1;
    fRightArm.fLayer2 = fSiR2;
    fRightArm.fLayer3 = fSiR3;

    fSiRHits = new TNtuple("SiRHits", "", "CentralMuonEnergy:SiR1Energy:SiR2Energy:SiR3Energy:SiR1Time:SiR2Time:SiR3Time");

    fSiL1.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_5 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_6 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_7 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_8 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_9 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_10 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_11 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_12 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_13 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_14 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_15 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_16 , kNotApplicable ));

  return 0;
}

int PlotTME_Si16b_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){

    double central_muon_time = (*i_tme)->GetCentralMuon()->GetTime();
    double central_muon_energy = (*i_tme)->GetCentralMuon()->GetEnergy();

    // Collect all the layer 1 hits
    DetectorList layer1 = fRightArm.fLayer1;
    std::vector<const TDetectorPulse*> layer1_tdps;
    for(DetectorList::const_iterator i_det=layer1.begin(); i_det!=layer1.end(); ++i_det){
      
      const std::string& detname = i_det->str();
      
      int layer1_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (layer1_source_index<0) {
	continue; // this TME has no sources for this channel
      }
      const IDs::source& layer1_source=(*i_tme)->GetSource(layer1_source_index);
	
      // Loop through all the pulses
      int n_pulses_layer1 = (*i_tme)->NumPulses(layer1_source);
      if (n_pulses_layer1 == 0) {
	n_pulses_layer1 = 1; // want to make sure we check the other layers
      }
      for(int i=0; i<n_pulses_layer1; ++i){ 
	const TDetectorPulse* layer1_tdp=(*i_tme)->GetPulse(layer1_source,i);
	layer1_tdps.push_back(layer1_tdp);
      }
    }

    // Collect all the layer two hits
    IDs::channel* layer2 = fRightArm.fLayer2;
    std::vector<const TDetectorPulse*> layer2_tdps;
    int layer2_source_index=(*i_tme)->GetFirstSourceIndex(*layer2); // I shouldn't have more than one source
    if (layer2_source_index>=0) {
      const IDs::source& layer2_source=(*i_tme)->GetSource(layer2_source_index);
      
      // Loop through all the pulses
      int n_pulses_layer2 = (*i_tme)->NumPulses(layer2_source);
      for(int i=0; i<n_pulses_layer2; ++i){
	const TDetectorPulse* layer2_tdp=(*i_tme)->GetPulse(layer2_source,i);
	layer2_tdps.push_back(layer2_tdp);
      }
    }

    // Collect all the layer 3 hits
    IDs::channel* layer3 = fRightArm.fLayer3;
    std::vector<const TDetectorPulse*> layer3_tdps;
    int layer3_source_index=(*i_tme)->GetFirstSourceIndex(*layer3); // I shouldn't have more than one source
    if (layer3_source_index>=0) {
      const IDs::source& layer3_source=(*i_tme)->GetSource(layer3_source_index);
	  
      // Loop through all the pulses
      int n_pulses_layer3 = (*i_tme)->NumPulses(layer3_source);
      for(int i=0; i<n_pulses_layer3; ++i){
	const TDetectorPulse* layer3_tdp=(*i_tme)->GetPulse(layer3_source,i);
	layer3_tdps.push_back(layer3_tdp);
      }
    }

    // Now fill the ntuple, taking into account layers where there are no hits
    double layer1_energy, layer2_energy, layer3_energy;
    double layer1_time, layer2_time, layer3_time;
    for (std::vector<const TDetectorPulse*>::const_iterator i_layer1_tdp = layer1_tdps.begin(); i_layer1_tdp != layer1_tdps.end()+1; ++i_layer1_tdp) {
      if ( i_layer1_tdp == layer1_tdps.end()) {
	layer1_energy = -1000;
	layer1_time = (*i_tme)->GetWindowWidth()*10; // put it will outside of the window
      }
      else {
	layer1_energy = (*i_layer1_tdp)->GetEnergy();
	layer1_time = (*i_layer1_tdp)->GetTime() - central_muon_time;
      }
      for (std::vector<const TDetectorPulse*>::const_iterator i_layer2_tdp = layer2_tdps.begin(); i_layer2_tdp != layer2_tdps.end()+1; ++i_layer2_tdp) {
	if ( i_layer2_tdp == layer2_tdps.end()) {
	  layer2_energy = -1000;
	  layer2_time = (*i_tme)->GetWindowWidth()*10; // put it will outside of the window
	}
	else {
	  layer2_energy = (*i_layer2_tdp)->GetEnergy();
	  layer2_time = (*i_layer2_tdp)->GetTime() - central_muon_time;
	}
	for (std::vector<const TDetectorPulse*>::const_iterator i_layer3_tdp = layer3_tdps.begin(); i_layer3_tdp != layer3_tdps.end()+1; ++i_layer3_tdp) {
	  if ( i_layer3_tdp == layer3_tdps.end()) {
	    layer3_energy = -1000;
	    layer3_time = (*i_tme)->GetWindowWidth()*10; // put it will outside of the window
	  }
	  else {
	    layer3_energy = (*i_layer3_tdp)->GetEnergy();
	    layer3_time = (*i_layer3_tdp)->GetTime() - central_muon_time;
	  }
	  
	  fSiRHits->Fill(central_muon_energy, layer1_energy, layer2_energy, layer3_energy, layer1_time, layer2_time, layer3_time);
	}
      }
    }
  }
  return 0;
}

int PlotTME_Si16b_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_Si16b_EvdE);
