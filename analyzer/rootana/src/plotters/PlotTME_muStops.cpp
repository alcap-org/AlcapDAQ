#include "PlotTME_muStops.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"
#include "EventNavigator.h"

#include <TH1F.h>
#include <TH2F.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

PlotTME_muStops::PlotTME_muStops(modules::options* opts):
   BaseModule("PlotTME_muStops",opts),
   fStoppedMus(0),fStoppedMus_PP(0), fNStopsThisEvent(0),fNStopsThisEvent_PP(0), fEventNo(1),
   fMuSc(IDs::kMuSc, IDs::kNotApplicable),
   fSiR2(IDs::kSiR2, IDs::kNotApplicable),
   fMuScMax(opts->GetDouble("muSc_max",1e9)),
   fMuScMin(opts->GetDouble("muSc_min",0)),
   fSiR2Max(opts->GetDouble("SiR2_max",1e9)),
   fSiR2Min(opts->GetDouble("SiR2_min",0))
{
}

PlotTME_muStops::~PlotTME_muStops(){
}

int PlotTME_muStops::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

    fTDiff=new TH1F("hTDiff", "TDiff of SiR2 hits compared to muSc (with pile-up)", 5000, -1.2e4,1.2e4);
    fTDiff->SetXTitle("t_{SiR2} - t_{muSc} (ns)");
    fTDiff_PP=new TH1F("hTDiff_PP", "TDiff of SiR2 hits compared to muSc (pile-up protected)", 5000, -1.2e4,1.2e4);
    fTDiff_PP->SetXTitle(fTDiff->GetXaxis()->GetTitle());

    fTDiffVsAmpSiR2=new TH2F("hTDiffVsAmpSiR2",
                             "Amplitudes of SiR2 hits compared to muSc (with pile-up) against the time diference",
                              400, -1.1e4, 1.1e4, 400,0,4000);
    fTDiffVsAmpSiR2->SetXTitle(fTDiff->GetXaxis()->GetTitle());
    fTDiffVsAmpSiR2->SetYTitle("Amplitude in SiR2 (ADC units)");
    fTDiffVsAmpSiR2_PP=new TH2F("hTDiffVsAmpSiR2_PP",
                             "Amplitudes of SiR2 hits compared to muSc (pile-up protected) against the time diference",
                              400, -1.1e4, 1.1e4, 400,0,4000);
    fTDiffVsAmpSiR2_PP->SetXTitle(fTDiffVsAmpSiR2->GetXaxis()->GetTitle());
    fTDiffVsAmpSiR2_PP->SetYTitle(fTDiffVsAmpSiR2->GetYaxis()->GetTitle());


    fAmplitudes=new TH2F("hAmp2d", "Amplitudes of SiR2 hits compared to muSc (with pile-up)", 400, 0,4000, 400,0,4000);
    fAmplitudes->SetXTitle("Amplitude in muSc (ADC units)");
    fAmplitudes->SetYTitle("Amplitude in SiR2 (ADC units)");
    fAmplitudes_PP=new TH2F("hAmp2d_PP", "Amplitudes of SiR2 hits compared to muSc (pile-up protected)", 400, 0,4000, 400,0,4000);
    fAmplitudes_PP->SetXTitle(fAmplitudes->GetXaxis()->GetTitle());
    fAmplitudes_PP->SetYTitle(fAmplitudes->GetYaxis()->GetTitle());

    const EventNavigator& event_ngtr= EventNavigator::Instance();
    fStopsPerEvent=new TH1F("hStoppedPerEvent","Number of #mu stops per midas event (with pile-up)",
                  event_ngtr.GetLoopNEntries(), event_ngtr.GetStartEntry(), event_ngtr.GetStopEntry());
    fStopsPerEvent->SetXTitle("Event Number");
    fStopsPerEvent->SetYTitle("Number of #mu stops");

    fStopsPerEvent_PP=new TH1F("hStoppedPerEvent_PP","Number of #mu stops per midas event (pile-up protected)",
                  event_ngtr.GetLoopNEntries(), event_ngtr.GetStartEntry(), event_ngtr.GetStopEntry());
    fStopsPerEvent_PP->SetXTitle("Event Number");
    fStopsPerEvent_PP->SetYTitle("Number of #mu stops");

    fStops=new TH1F("hStopped","Number of #mu stops per midas event (pile-up protected)", 250, 0, 250);
    fStops->SetXTitle("Number of #mu stops");
    fStops_PP=new TH1F("hStopped_PP","Number of #mu stops per midas event (pile-up protected)", 250, 0, 250);
    fStops_PP->SetXTitle("Number of #mu stops");


  return 0;
}

int PlotTME_muStops::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    // Reset the number of mu stops in the event
    fNStopsThisEvent=0;
    fNStopsThisEvent_PP=0;

    // Loop over each TME
    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){
       // Loop over each muSc source
       for(int muSc_source_index = (*i_tme)->GetFirstSourceIndex(fMuSc);
               muSc_source_index < (*i_tme)->GetLastSourceIndex(fMuSc) 
                  && muSc_source_index > -1; ++muSc_source_index){
         const IDs::source& muSc_source=(*i_tme)->GetSource(muSc_source_index);
         // Loop over each SiR2 source
         for(int siR2_source_index = (*i_tme)->GetFirstSourceIndex(fSiR2);
                 siR2_source_index < (*i_tme)->GetLastSourceIndex(fSiR2) 
                    && siR2_source_index > -1; ++siR2_source_index){
           const IDs::source& siR2_source=(*i_tme)->GetSource(siR2_source_index);
           // Fill histograms from the various sources
           FillHistograms(*i_tme, muSc_source, siR2_source);
         } // loop over SiR2 sources
       } // loop over muSc sources
    } // loop over each TME

    fStopsPerEvent->Fill(fEventNo, fNStopsThisEvent);
    fStopsPerEvent_PP->Fill(fEventNo, fNStopsThisEvent_PP);
    fStops->Fill( fNStopsThisEvent);
    fStops_PP->Fill( fNStopsThisEvent_PP);
    ++fEventNo;
    fStoppedMus+=fNStopsThisEvent;
    fStoppedMus_PP+=fNStopsThisEvent_PP;
  return 0;
}

void PlotTME_muStops::FillHistograms(const TMuonEvent* tme, const IDs::source& muSc_source, const IDs::source& sir2_source){
  //check if we pass the muSc muon cut
  const double amplitude=tme->GetCentralMuon()->GetAmplitude(TDetectorPulse::kFast);
  if(amplitude<fMuScMin || amplitude> fMuScMax) return;

  const double time=tme->GetTime();
  const int N_muSc=tme->NumPulses(muSc_source);
  const int N_sir2=tme->NumPulses(sir2_source);
  bool a_stop=false, a_stop_PP=false;
  for(int i=0; i< N_sir2; ++i){
    const TDetectorPulse* sir2_tdp=tme->GetPulse(sir2_source,i);
    const double t_diff=sir2_tdp->GetTime(TDetectorPulse::kFast) - time;
    const double sir2_amp=sir2_tdp->GetAmplitude(TDetectorPulse::kFast);
    fTDiff->Fill(t_diff);
    fTDiffVsAmpSiR2->Fill(t_diff, sir2_amp );
    if(sir2_amp > fSiR2Min && sir2_amp < fSiR2Max){
       a_stop=true;
    }
    if(!tme->HasMuonPileup()){
      fTDiff_PP->Fill(t_diff);
      fTDiffVsAmpSiR2_PP->Fill(t_diff, sir2_amp );
      if(sir2_amp > fSiR2Min && sir2_amp < fSiR2Max){
         a_stop_PP=true;
      }
    }

    for(int j=0; j< N_muSc; ++j){
      const double muSc_amp=tme->GetPulse(muSc_source,j)->GetAmplitude(TDetectorPulse::kFast);
      fAmplitudes->Fill(muSc_amp, sir2_amp );
      if(!tme->HasMuonPileup()){
         fAmplitudes_PP->Fill(muSc_amp, sir2_amp );
      }
    }
  }
  if(a_stop) ++fNStopsThisEvent;
  if(a_stop_PP) ++fNStopsThisEvent_PP;
}

int PlotTME_muStops::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  cout<<"PlotTME_muStops::AfterLastEntry: Total number of muon stops = "<<fStoppedMus<<endl;
  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_muStops,  muSc_min,muSc_max, SiR2_min, SiR2_max);
