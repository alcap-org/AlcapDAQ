#include "PlotTME_EdE.h"
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

PlotTME_EdE::PlotTME_EdE(modules::options* opts):
   BaseModule("PlotTME_EdE",opts),fNullCount(0),fTdpCount(0){
}

PlotTME_EdE::~PlotTME_EdE(){
}

int PlotTME_EdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fSiT.push_back(IDs::channel (kSiT_1 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_2 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_3 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_4 , kNotApplicable ));

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

    fSiL2 = new IDs::channel(kSiL2, kNotApplicable);
    fSiL3 = new IDs::channel(kSiL3, kNotApplicable);

    fLeftArm.fLayer1 = fSiL1;
    fLeftArm.fLayer2 = fSiL3;
    fLeftArm.fLayer3 = fSiL2; //turned off

    fSiR1.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));

    fSiR2 = new IDs::channel(kSiR2, kNotApplicable);
    fSiR3 = new IDs::channel(kSiR3, kNotApplicable);

    fRightArm.fLayer1 = fSiR1;
    fRightArm.fLayer2 = fSiR2;
    fRightArm.fLayer3 = fSiR3;

    fGe = new IDs::channel(kGeLoGain, kNotApplicable);
    fSiLHits = new TNtuple("SiLHits", "", "SiL1Energy:SiL2Energy:SiL1Time:SiL2Time");
    fSiRHits = new TNtuple("SiRHits", "", "SiR1Energy:SiR2Energy:SiR3Energy:SiR1Time:SiR2Time:SiR3Time");
//    fGeHits = new TNtuple("GeHits", "", "Energy:Time");

  return 0;
}

int PlotTME_EdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  double layer1_energy = 0.0f, llayer1_energy = 0.0f;
  double layer2_energy = 0.0f, llayer2_energy = 0.0f;
  double layer3_energy = 0.0f;

  double layer1_time = -99999.0f, llayer1_time = -99999.0f;
  double layer2_time = -99999.0f, llayer2_time = -99999.0f;
  double layer3_time = -99999.0f;

  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){

    double central_muon_time = (*i_tme)->GetCentralMuon()->GetTime();

//     // Now get the Ge pulses
//     int ge_source_index=(*i_tme)->GetFirstSourceIndex(*fGe); // I shouldn't have more than one source
//     if (ge_source_index<0) {
//       continue; // this TME has no sources for this channel
//     }
//     const IDs::source& ge_source=(*i_tme)->GetSource(ge_source_index);
//     
//     // Loop through all the pulses
//     int n_pulses_ge = (*i_tme)->NumPulses(ge_source);
//     for(int i=0; i<n_pulses_ge; ++i){
//	const TDetectorPulse* ge_tdp=(*i_tme)->GetPulse(ge_source,i);
//	      double layer_energy = ge_tdp->GetEnergy();
//	      double layer_time = ge_tdp->GetTime() - central_muon_time;
//	      fGeHits->Fill(layer_energy, layer_time);
//     }

    DetectorList layer1 = fRightArm.fLayer1;
    IDs::channel* layer2 = fRightArm.fLayer2;
    IDs::channel* layer3 = fRightArm.fLayer3;
  
    for(DetectorList::const_iterator i_det=layer1.begin(); i_det!=layer1.end(); ++i_det){
      
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
       
       	  // Now get the layer 3 pulses
       	  int layer3_source_index=(*i_tme)->GetFirstSourceIndex(*layer3); // I shouldn't have more than one source
       	  if (layer3_source_index<0) {
              layer1_energy = layer1_tdp->GetEnergy();
              layer2_energy = layer2_tdp->GetEnergy();
              layer3_energy = 0.0f;
  
              layer1_time = layer1_tdp->GetTime() - central_muon_time;
              layer2_time = layer2_tdp->GetTime() - central_muon_time;
              layer3_time = -99999.0f;
              fSiRHits->Fill(layer1_energy, layer2_energy, layer3_energy, layer1_time, layer2_time, layer3_time);
       	  } else {
       	    const IDs::source& layer3_source=(*i_tme)->GetSource(layer3_source_index);
       	    
       	    // Loop through all the pulses
       	    int n_pulses_layer3 = (*i_tme)->NumPulses(layer3_source);
       	    for(int i=0; i<n_pulses_layer3; ++i){
              const TDetectorPulse* layer3_tdp=(*i_tme)->GetPulse(layer3_source,i);
  
              layer1_energy = layer1_tdp->GetEnergy();
              layer2_energy = layer2_tdp->GetEnergy();
              layer3_energy = layer3_tdp->GetEnergy();
  
              layer1_time = layer1_tdp->GetTime() - central_muon_time;
              layer2_time = layer2_tdp->GetTime() - central_muon_time;
              layer3_time = layer3_tdp->GetTime() - central_muon_time;
              fSiRHits->Fill(layer1_energy, layer2_energy, layer3_energy, layer1_time, layer2_time, layer3_time);
            }
          }
        }
      }
    }

    DetectorList llayer1 = fLeftArm.fLayer1;
    IDs::channel* llayer2 = fLeftArm.fLayer2;
  
    for(DetectorList::const_iterator i_det=llayer1.begin(); i_det!=llayer1.end(); ++i_det){
      
      int llayer1_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (llayer1_source_index<0) {
        continue; // this TME has no sources for this channel
      }
      const IDs::source& llayer1_source=(*i_tme)->GetSource(llayer1_source_index);
        
      // Loop through all the pulses
      int n_pulses_llayer1 = (*i_tme)->NumPulses(llayer1_source);
      for(int i=0; i<n_pulses_llayer1; ++i){
        const TDetectorPulse* llayer1_tdp=(*i_tme)->GetPulse(llayer1_source,i);
  
        // Now get the llayer 2 pulses
        int llayer2_source_index=(*i_tme)->GetFirstSourceIndex(*llayer2); // I shouldn't have more than one source
        if (llayer2_source_index<0) {
          continue; // this TME has no sources for this channel
        }
        const IDs::source& llayer2_source=(*i_tme)->GetSource(llayer2_source_index);
        
        // Loop through all the pulses
        int n_pulses_llayer2 = (*i_tme)->NumPulses(llayer2_source);
        for(int i=0; i<n_pulses_llayer2; ++i){
       	  const TDetectorPulse* llayer2_tdp=(*i_tme)->GetPulse(llayer2_source,i);
       
          llayer1_energy = llayer1_tdp->GetEnergy();
          llayer2_energy = llayer2_tdp->GetEnergy();
  
          llayer1_time = llayer1_tdp->GetTime() - central_muon_time;
          llayer2_time = llayer2_tdp->GetTime() - central_muon_time;
          fSiLHits->Fill(llayer1_energy, llayer2_energy, llayer1_time, llayer2_time);
        }
      }
    }
  } //end MuonEventList for-loop
  return 0;
}

int PlotTME_EdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_EdE);
