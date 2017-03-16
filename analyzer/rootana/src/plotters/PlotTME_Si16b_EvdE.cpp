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

    fSiRHits = new TNtuple("SiRHits", "", "CentralMuonEnergy:SiL1Energy:SiL1Time:SiR1Energy:SiR2Energy:SiR3Energy:SiR1Time:SiR2Time:SiR3Time");

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

    // Want to only a single hit in SiL1
    int n_sil1_pulses = (*i_tme)->NumPulses(fSiL1);
    if (n_sil1_pulses != 1) {
      continue; // to the next TME
    }

    // Get the SiL1 pulse
    double sil1_energy = 0;
    double sil1_time = 0;
    for(DetectorList::const_iterator i_sil1_det=fSiL1.begin(); i_sil1_det!=fSiL1.end(); ++i_sil1_det){
      int sil1_source_index=(*i_tme)->GetFirstSourceIndex(*i_sil1_det); // I shouldn't have more than one source
      if (sil1_source_index<0) {
	continue; // this TME has no sources for this channel
      }
      const IDs::source& sil1_source=(*i_tme)->GetSource(sil1_source_index);	
      const TDetectorPulse* sil1_tdp=(*i_tme)->GetPulse(sil1_source,0); // we already know this only has one pulse
      sil1_energy = sil1_tdp->GetEnergy();
      sil1_time = sil1_tdp->GetTime() - central_muon_time;
    }

    // Now continue and get the SiR1 and SiR2 hits
    DetectorList layer1 = fRightArm.fLayer1;
    IDs::channel* layer2 = fRightArm.fLayer2;
    for(DetectorList::const_iterator i_det=layer1.begin(); i_det!=layer1.end(); ++i_det){
      
      const std::string& detname = i_det->str();
      
      int layer1_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (layer1_source_index<0) {
	continue; // this TME has no sources for this channel
      }
      const IDs::source& layer1_source=(*i_tme)->GetSource(layer1_source_index);
	
      // Loop through all the pulses
      int n_pulses_layer1 = (*i_tme)->NumPulses(layer1_source);
      for(int i=0; i<n_pulses_layer1; ++i){
	const TDetectorPulse* layer1_tdp=(*i_tme)->GetPulse(layer1_source,i);

	// Now get the layer 2 pulses
	int layer2_source_index=(*i_tme)->GetFirstSourceIndex(*layer2); // I shouldn't have more than one source
	if (layer2_source_index<0) {
	  continue; // this TME has no sources for this channel
	}
	const IDs::source& layer2_source=(*i_tme)->GetSource(layer2_source_index);
	
	// Loop through all the pulses
	int n_pulses_layer2 = (*i_tme)->NumPulses(layer2_source);
	for(int i=0; i<n_pulses_layer2; ++i){
	  const TDetectorPulse* layer2_tdp=(*i_tme)->GetPulse(layer2_source,i);

	  double layer1_energy = layer1_tdp->GetEnergy();
	  double layer2_energy = layer2_tdp->GetEnergy();

	  double layer1_time = layer1_tdp->GetTime() - central_muon_time;
	  double layer2_time = layer2_tdp->GetTime() - central_muon_time;

	  fSiRHits->Fill(central_muon_energy, sil1_energy, sil1_time, layer1_energy, layer2_energy, 0, layer1_time, layer2_time, 0);
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
