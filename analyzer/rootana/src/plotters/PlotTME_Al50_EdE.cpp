#include "PlotTME_Al50_EdE.h"
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

PlotTME_Al50_EdE::PlotTME_Al50_EdE(modules::options* opts):
   BaseModule("PlotTME_Al50_EdE",opts),fNullCount(0),fTdpCount(0){
}

PlotTME_Al50_EdE::~PlotTME_Al50_EdE(){
}

int PlotTME_Al50_EdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fSiT.push_back(IDs::channel (kSiT_1 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_2 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_3 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_4 , kNotApplicable ));

//    fSiL1.push_back(IDs::channel (kSiL1_1 , kNotApplicable )); //noisy channel
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
//    fSiL1.push_back(IDs::channel (kSiL1_16 , kNotApplicable )); //noisy channel

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

int PlotTME_Al50_EdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  double right_layer1_energy = 0.0f, left_layer1_energy = 0.0f;
  double right_layer2_energy = 0.0f, left_layer2_energy = 0.0f;
  double right_layer3_energy = 0.0f;

  double right_layer1_time = -99999.0f, left_layer1_time = -99999.0f;
  double right_layer2_time = -99999.0f, left_layer2_time = -99999.0f;
  double right_layer3_time = -99999.0f;

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

    DetectorList right_layer1 = fRightArm.fLayer1;
    IDs::channel* right_layer2 = fRightArm.fLayer2;
    IDs::channel* right_layer3 = fRightArm.fLayer3;
  
    for(DetectorList::const_iterator i_det=right_layer1.begin(); i_det!=right_layer1.end(); ++i_det){
      
      int right_layer1_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (right_layer1_source_index<0) {
        continue; // this TME has no sources for this channel
      }
      const IDs::source& right_layer1_source=(*i_tme)->GetSource(right_layer1_source_index);
        
      // Loop through all the pulses
      int n_pulses_right_layer1 = (*i_tme)->NumPulses(right_layer1_source);
      for(int i=0; i<n_pulses_right_layer1; ++i){
        const TDetectorPulse* right_layer1_tdp=(*i_tme)->GetPulse(right_layer1_source,i);
  
        // Now get the right_layer 2 pulses
        int right_layer2_source_index=(*i_tme)->GetFirstSourceIndex(*right_layer2); // I shouldn't have more than one source
        if (right_layer2_source_index<0) {
          continue; // this TME has no sources for this channel
        }
        const IDs::source& right_layer2_source=(*i_tme)->GetSource(right_layer2_source_index);
        
        // Loop through all the pulses
        int n_pulses_right_layer2 = (*i_tme)->NumPulses(right_layer2_source);
        for(int i=0; i<n_pulses_right_layer2; ++i){
       	  const TDetectorPulse* right_layer2_tdp=(*i_tme)->GetPulse(right_layer2_source,i);
       
       	  // Now get the right_layer 3 pulses
       	  int right_layer3_source_index=(*i_tme)->GetFirstSourceIndex(*right_layer3); // I shouldn't have more than one source
       	  if (right_layer3_source_index<0) {
              right_layer1_energy = right_layer1_tdp->GetEnergy();
              right_layer2_energy = right_layer2_tdp->GetEnergy();
              right_layer3_energy = 0.0f;
  
              right_layer1_time = right_layer1_tdp->GetTime() - central_muon_time;
              right_layer2_time = right_layer2_tdp->GetTime() - central_muon_time;
              right_layer3_time = -99999.0f;
              fSiRHits->Fill(right_layer1_energy, right_layer2_energy, right_layer3_energy, right_layer1_time, right_layer2_time, right_layer3_time);
       	  } else {
       	    const IDs::source& right_layer3_source=(*i_tme)->GetSource(right_layer3_source_index);
       	    
       	    // Loop through all the pulses
       	    int n_pulses_right_layer3 = (*i_tme)->NumPulses(right_layer3_source);
       	    for(int i=0; i<n_pulses_right_layer3; ++i){
              const TDetectorPulse* right_layer3_tdp=(*i_tme)->GetPulse(right_layer3_source,i);
  
              right_layer1_energy = right_layer1_tdp->GetEnergy();
              right_layer2_energy = right_layer2_tdp->GetEnergy();
              right_layer3_energy = right_layer3_tdp->GetEnergy();
  
              right_layer1_time = right_layer1_tdp->GetTime() - central_muon_time;
              right_layer2_time = right_layer2_tdp->GetTime() - central_muon_time;
              right_layer3_time = right_layer3_tdp->GetTime() - central_muon_time;
              fSiRHits->Fill(right_layer1_energy, right_layer2_energy, right_layer3_energy, right_layer1_time, right_layer2_time, right_layer3_time);
            }
          }
        }
      }
    }

    DetectorList left_layer1 = fLeftArm.fLayer1;
    IDs::channel* left_layer2 = fLeftArm.fLayer2;
  
    for(DetectorList::const_iterator i_det=left_layer1.begin(); i_det!=left_layer1.end(); ++i_det){
      
      int left_layer1_source_index=(*i_tme)->GetFirstSourceIndex(*i_det); // I shouldn't have more than one source
      if (left_layer1_source_index<0) {
        continue; // this TME has no sources for this channel
      }
      const IDs::source& left_layer1_source=(*i_tme)->GetSource(left_layer1_source_index);
        
      // Loop through all the pulses
      int n_pulses_left_layer1 = (*i_tme)->NumPulses(left_layer1_source);
      for(int i=0; i<n_pulses_left_layer1; ++i){
        const TDetectorPulse* left_layer1_tdp=(*i_tme)->GetPulse(left_layer1_source,i);
  
        // Now get the left_layer 2 pulses
        int left_layer2_source_index=(*i_tme)->GetFirstSourceIndex(*left_layer2); // I shouldn't have more than one source
        if (left_layer2_source_index<0) {
          continue; // this TME has no sources for this channel
        }
        const IDs::source& left_layer2_source=(*i_tme)->GetSource(left_layer2_source_index);
        
        // Loop through all the pulses
        int n_pulses_left_layer2 = (*i_tme)->NumPulses(left_layer2_source);
        for(int i=0; i<n_pulses_left_layer2; ++i){
       	  const TDetectorPulse* left_layer2_tdp=(*i_tme)->GetPulse(left_layer2_source,i);
       
          left_layer1_energy = left_layer1_tdp->GetEnergy();
          left_layer2_energy = left_layer2_tdp->GetEnergy();
  
          left_layer1_time = left_layer1_tdp->GetTime() - central_muon_time;
          left_layer2_time = left_layer2_tdp->GetTime() - central_muon_time;
          fSiLHits->Fill(left_layer1_energy, left_layer2_energy, left_layer1_time, left_layer2_time);
        }
      }
    }
  } //end MuonEventList for-loop
  return 0;
}

int PlotTME_Al50_EdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_Al50_EdE);
