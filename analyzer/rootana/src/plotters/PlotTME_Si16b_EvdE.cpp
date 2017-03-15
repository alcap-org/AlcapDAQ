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

    double min_x_energy = 0;
    double max_x_energy = 20000;
    double min_y_energy = 0;
    double max_y_energy = 3000;
    double energy_width = 10; // keV bins
    int n_bins_x = (max_x_energy - min_x_energy) / energy_width;
    int n_bins_y = (max_y_energy - min_y_energy) / energy_width;

    fEvdE_Right = new TH2F("hEvdE", "E v dE (right arm)", n_bins_x,min_x_energy,max_x_energy, n_bins_y,min_y_energy,max_y_energy);
    fEvdE_Right->SetXTitle("E_{1} + E_{2} [keV]");
    fEvdE_Right->SetYTitle("E_{1} [keV]");

    fEvdE_Right_wCoincCut = (TH2F*) fEvdE_Right->Clone("fEvdE_Right_wCoincCut");

    fTDiff_Layer1_Layer2 = new TH1F("hTDiff_Layer1_Layer2", "Time Difference between Layer 1 hit and Layer 2 hit", 20000,-10000,10000);
    fTDiff_Layer1_Layer2->SetXTitle("t_{2} - t_{1} [ns]");
   

  return 0;
}

int PlotTME_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){

    double central_muon_time = (*i_tme)->GetCentralMuon()->GetTime();

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

	  double dE = layer1_tdp->GetEnergy();
	  double E = layer2_tdp->GetEnergy() + dE;
	  fEvdE_Right->Fill(E, dE);

	  double layer1_time = layer1_tdp->GetTime();
	  double layer2_time = layer2_tdp->GetTime();
	  double tdiff = layer2_time - layer1_time;
	  fTDiff_Layer1_Layer2->Fill(tdiff);

	  if (tdiff > -200 && tdiff < 200) {
	    fEvdE_Right_wCoincCut->Fill(E, dE);
	  }
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
