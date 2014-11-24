#include "PlotTME_activeSiRmuStops.h"
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
#include <TLine.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

PlotTME_activeSiRmuStops::PlotTME_activeSiRmuStops(modules::options* opts):
   BaseModule("PlotTME_activeSiRmuStops",opts),
   fStoppedMus(0),fStoppedMus_PP(0), fNStopsThisEvent(0),fNStopsThisEvent_PP(0), fEventNo(1),
   fMuSc(opts->GetString("second","muSc")),
   fSiR2(opts->GetString("first","SiR2")),
   fChannel((TDetectorPulse::ParentChannel_t) (IDs::channel::GetSlowFastEnum(opts->GetString("channel","fast"))-IDs::kFast)),
   fMuScMax(opts->GetDouble("muSc_max",1e9)),
   fMuScMin(opts->GetDouble("muSc_min",0)),
   fSiR2Max(opts->GetDouble("SiR2_max",1e9)),
   fSiR2Min(opts->GetDouble("SiR2_min",0))
{
DEBUG_VALUE(fMuSc, fSiR2,fChannel);
}

PlotTME_activeSiRmuStops::~PlotTME_activeSiRmuStops(){
}

int PlotTME_activeSiRmuStops::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

    fTDiff=new TH1F("hTDiff", "TDiff of SiR2 hits compared to muSc (with pile-up)", 5000, -1.2e4,1.2e4);
    fTDiff->SetXTitle("t_{SiR2} - t_{muSc} (ns)");
    fTDiff_PP=new TH1F("hTDiff_PP", "TDiff of SiR2 hits compared to muSc (pile-up protected)", 5000, -1.2e4,1.2e4);
    fTDiff_PP->SetXTitle(fTDiff->GetXaxis()->GetTitle());

    fTDiffMuons=new TH1F("hTDiffMuons", "TDiff of muons considered to have stopped (with pile-up)", 5000, -1.2e4,1.2e4);
    fTDiffMuons->SetXTitle("t_{SiR2} - t_{muSc} (ns)");
    fTDiffMuons_PP=new TH1F("hTDiffMuons_PP", "TDiff of muons considered to have stopped (pile-up protected)", 5000, -1.2e4,1.2e4);
    fTDiffMuons_PP->SetXTitle(fTDiffMuons->GetXaxis()->GetTitle());

    fTDiffVsAmpSiR2=new TH2F("hTDiffVsAmpSiR2",
                             "Amplitudes of SiR2 hits (with pile-up) against the time difference compared to muSc ",
                              400, -1.1e4, 1.1e4, 400,0,4000);
    fTDiffVsAmpSiR2->SetXTitle(fTDiff->GetXaxis()->GetTitle());
    fTDiffVsAmpSiR2->SetYTitle("Amplitude in SiR2 (ADC units)");
    fTDiffVsAmpSiR2_PP=new TH2F("hTDiffVsAmpSiR2_PP",
                             "Amplitudes of SiR2 hits (pile-up protected) against the time diference compared to muSc",
                              400, -1.1e4, 1.1e4, 400,0,4000);
    fTDiffVsAmpSiR2_PP->SetXTitle(fTDiffVsAmpSiR2->GetXaxis()->GetTitle());
    fTDiffVsAmpSiR2_PP->SetYTitle(fTDiffVsAmpSiR2->GetYaxis()->GetTitle());

    //fTDiffVsAmpSiR2_MuStop=new TH2F("hTDiffVsAmpSiR2_MuStop",
    //                                "Amplitudes of SiR2 hits compared to muSc (with pile-up) against the time diference",
    //                                 400, -1.1e4, 1.1e4, 400,0,4000);
    //fTDiffVsAmpSiR2_MuStop->SetXTitle(fTDiff->GetXaxis()->GetTitle());
    //fTDiffVsAmpSiR2_MuStop->SetYTitle("Amplitude in SiR2 (ADC units)");
    fTDiffVsAmpSiR2_MuStop_PP=new TH2F("hTDiffVsAmpSiR2_MuStop_PP",
                                    "Amplitudes of SiR2 hits compared to muSc (pile-up protected) against the time diference",
                                     400, -1.1e4, 1.1e4, 400,0,4000);
    fTDiffVsAmpSiR2_MuStop_PP->SetXTitle(fTDiffVsAmpSiR2->GetXaxis()->GetTitle());
    fTDiffVsAmpSiR2_MuStop_PP->SetYTitle(fTDiffVsAmpSiR2->GetYaxis()->GetTitle());


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

int PlotTME_activeSiRmuStops::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
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

void PlotTME_activeSiRmuStops::FillHistograms(const TMuonEvent* tme, const IDs::source& muSc_source, const IDs::source& sir2_source){
  //check if we pass the muSc muon cut
  const double muSc_amp=tme->GetCentralMuon()->GetAmplitude(TDetectorPulse::kFast);

  const double time=tme->GetTime();
  const int N_muSc=tme->NumPulses(muSc_source);
  const int N_sir2=tme->NumPulses(sir2_source);

  // Scan for a muon hit in the SiR2
  bool a_stop=false;
  if(muSc_amp>fMuScMin && muSc_amp< fMuScMax){
    for(int i=0; i< N_sir2; ++i){
      const double sir2_amp=tme->GetPulse(sir2_source,i)->GetAmplitude(fChannel);
      if(sir2_amp > fSiR2Min && sir2_amp < fSiR2Max){
         a_stop=true;
         break;
      }
    }
    if(a_stop) {
      ++fNStopsThisEvent;
      if(!tme->HasMuonPileup()) ++fNStopsThisEvent_PP;
    }
  }

  // Fill all plots for SiR2
  for(int i=0; i< N_sir2; ++i){
    const TDetectorPulse* sir2_tdp=tme->GetPulse(sir2_source,i);
    const double t_diff=sir2_tdp->GetTime(fChannel) - time;
    const double sir2_amp=sir2_tdp->GetAmplitude(fChannel);
    fTDiff->Fill(t_diff);
    fTDiffVsAmpSiR2->Fill(t_diff, sir2_amp );
    if(!tme->HasMuonPileup()){
      if(muSc_amp>fMuScMin && muSc_amp< fMuScMax){
         fTDiff_PP->Fill(t_diff);
         fTDiffVsAmpSiR2_PP->Fill(t_diff, sir2_amp );
         if(a_stop){
           fTDiffMuons_PP->Fill(t_diff);
           fTDiffVsAmpSiR2_MuStop_PP->Fill(t_diff,sir2_amp);
         }
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
}

int PlotTME_activeSiRmuStops::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  double y_min=fAmplitudes->GetYaxis()->GetXmin();
  double y_max=fAmplitudes->GetYaxis()->GetXmax();
  TLine* muSc_cut=new TLine(fMuScMin, y_min, fMuScMin, y_max);
  muSc_cut->SetLineColor(kRed);
  fAmplitudes->GetListOfFunctions()->Add(muSc_cut->Clone());
  fAmplitudes_PP->GetListOfFunctions()->Add(muSc_cut->Clone());
  muSc_cut->SetX1(fMuScMax);
  muSc_cut->SetX2(fMuScMax);
  fAmplitudes->GetListOfFunctions()->Add(muSc_cut->Clone());
  fAmplitudes_PP->GetListOfFunctions()->Add(muSc_cut->Clone());
  fTDiffVsAmpSiR2_MuStop_PP->ProjectionY("_py",236);

  cout<<"PlotTME_activeSiRmuStops::AfterLastEntry: Total number of muon stops = "<<fStoppedMus<<endl;
  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_activeSiRmuStops,  muSc_min,muSc_max, SiR2_min, SiR2_max, first, channel);
