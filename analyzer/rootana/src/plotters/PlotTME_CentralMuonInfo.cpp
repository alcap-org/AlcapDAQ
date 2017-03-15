#include "PlotTME_CentralMuonInfo.h"
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

PlotTME_CentralMuonInfo::PlotTME_CentralMuonInfo(modules::options* opts):
   BaseModule("PlotTME_CentralMuonInfo",opts),fNullCount(0),fTdpCount(0){
}

PlotTME_CentralMuonInfo::~PlotTME_CentralMuonInfo(){
}

int PlotTME_CentralMuonInfo::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fSiT.push_back(IDs::channel (kSiT_1 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_2 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_3 , kNotApplicable ));
    fSiT.push_back(IDs::channel (kSiT_4 , kNotApplicable ));

    double min_time = 0;
    double max_time = 120e6;
    double time_width = 20e3;
    int n_time_bins = (max_time - min_time) / time_width;

    fCentralMuonEnergyTime = new TH2F("hCentralEnergyTime", "Central Muon in the TME (no pileup protection)", n_time_bins, min_time, max_time, 1000,0,10000);
    fCentralMuonEnergyTime->SetXTitle("MIDAS Block Time [ns]");
    fCentralMuonEnergyTime->SetYTitle("Energy [keV]");

    fCentralMuonEnergyTime_PP = (TH2F*) fCentralMuonEnergyTime->Clone("hCentralEnergyTime_PP");
    fCentralMuonEnergyTime_PP->SetTitle("Central Muon in the TME (pileup protected)");

  return 0;
}

int PlotTME_CentralMuonInfo::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){
    int central_muon_slow_id = (*i_tme)->GetCentralMuon()->GetTAP(TDetectorPulse::kSlow)->GetParentID();
    
    double central_muon_time = (*i_tme)->GetCentralMuon()->GetTime();
    double central_muon_energy = (*i_tme)->GetCentralMuon()->GetEnergy();
    const IDs::Detector_t& central_muon_channel = (*i_tme)->GetCentralMuon()->GetSource().Channel().Detector();
    fCentralMuonEnergyTime->Fill(central_muon_time, central_muon_energy);
    
    if (!(*i_tme)->HasMuonPileup()) {
      fCentralMuonEnergyTime_PP->Fill(central_muon_time, central_muon_energy);
    }
  }      
  
  return 0;
}

int PlotTME_CentralMuonInfo::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_CentralMuonInfo);
