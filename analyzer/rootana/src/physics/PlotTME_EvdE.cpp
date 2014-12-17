#include "PlotTME_EvdE.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TMuonEvent.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "debug_tools.h"
#include "ModulesNavigator.h"

#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;

extern SourceDetPulseMap gDetectorPulseMap;
extern MuonEventList gMuonEvents;

PlotTME_EvdE::PlotTME_EvdE(modules::options* opts):
   BaseModule("PlotTME_EvdE",opts),
   fMinTime(opts->GetDouble("min_time",500)),
   fMuScMax(opts->GetDouble("muSc_max",1e9)),
   fMuScMin(opts->GetDouble("muSc_min",0)),
   fActiveStops(IDs::source(),fMuScMin,fMuScMax,opts->GetDouble("SiR2_min",0),opts->GetDouble("SiR2_max",1e9)){
}

PlotTME_EvdE::~PlotTME_EvdE(){
}

int PlotTME_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

    // Make sure we've got MakeSiliconEvents being used
    if(!modules::navigator::Instance()->Before("MakeSiliconEvents","PlotTME_EvdE")){
        cout<<"It's meaningless to use the PlotTME_EvdE module without the MakeSiliconEvents module running first as well"<<endl;
        return 1;
    }

    // Find the SiR2 TDP generator
    IDs::source tmp(IDs::kSiR2, IDs::kAnySlowFast, IDs::kAnyGenerator, IDs::kAnyConfig);
    for(SourceDetPulseMap::iterator i_source=gDetectorPulseMap.begin();
            i_source!= gDetectorPulseMap.end(); ++i_source){
        if(i_source->first.matches(tmp)){
            tmp=i_source->first;
            break;
        }
    }
    fActiveStops.SetSiR2Source(tmp);

    std::string titles[kNHists];
    titles[kNoCuts]         = "Muon Pile-up allowed";
    titles[kMuPP]           = "Muon Pile-up protection";
    titles[kMuPP_muAmp]     = "Muon amp. cut and pile-up protection";
    titles[kMuPP_muAmp_T500]= Form("#mu PP, #mu amp. cut and time in thick > %gns",fMinTime);
    titles[kActiveStop]     = "All cuts including active Si hit";

    std::string names[kNHists];
    names[kNoCuts]         ="";
    names[kMuPP]           ="_mu_PP";
    names[kMuPP_muAmp]     ="_mu_PP_mu_amp";
    names[kMuPP_muAmp_T500]="_mu_PP_mu_amp_t_cut";
    names[kActiveStop]     ="_active_stop";

    std::vector<double> t_bins;
    while(true){
    double total=1e4;
    double tstep=total/16.;
    for(double t=0; t<total; t+=tstep) t_bins.push_back(t);
    break;
    }
    //for(unsigned i=0;i<t_bins.size();i++) cout<<i<<": "<<t_bins[i]<<endl;

    // for left and right packages
    for(int i=0;i<1;i++){
    //for(int i=0;i<2;i++){
        TString title_kernel= i==0? "Left" : "Right";
        title_kernel+= " Silicon with ";
        TString name_kernel= i==0? "_L" : "_R";

        int min_dE=0, max_dE=8000;
        int min_E=0, max_E=25000;
        int min_dT=0, max_dT=10000;
        for(int i_hist=0; i_hist< kNHists; ++i_hist){
           for(int i_quad=0; i_quad< 5; ++i_quad){
              fHists[i][i_hist][i_quad].EvdE=new TH2F(
                      Form("hEvdE_%s%s_%d",name_kernel.Data(),names[i_hist].c_str(),i_quad),
                      Form("E vs dE for %s %s %d",title_kernel.Data(),titles[i_hist].c_str(),i_quad),
                      400,min_E,max_E,400,min_dE,max_dE);
              fHists[i][i_hist][i_quad].EvdE->SetXTitle("Total energy (KeV)");
              fHists[i][i_hist][i_quad].EvdE->SetYTitle("dE/dx (KeV)");

              fHists[i][i_hist][i_quad].time=new TH1F(
                      Form("hTime%s%s_%d",name_kernel.Data(),names[i_hist].c_str(),i_quad),
                      Form("Timing for %s %s %d",title_kernel.Data(),titles[i_hist].c_str(),i_quad),
                      600,min_dT,max_dT);
              fHists[i][i_hist][i_quad].time->SetXTitle("Time (ns)");

              fHists[i][i_hist][i_quad].EvdEvTime=new TH3I(
                      Form("hEvdEvTime%s%s_%d",name_kernel.Data(),names[i_hist].c_str(),i_quad),
                      Form("E vs dE vs Timing for %s %s %d",title_kernel.Data(),titles[i_hist].c_str(),i_quad),
                      100,min_E,max_E,100,min_dE,max_dE,t_bins.size(),min_dT,max_dT);
              fHists[i][i_hist][i_quad].EvdEvTime->GetZaxis()->Set(t_bins.size()-1, t_bins.data());
              fHists[i][i_hist][i_quad].EvdEvTime->SetXTitle("Total energy (KeV)");
              fHists[i][i_hist][i_quad].EvdEvTime->SetYTitle("dE/dx (KeV)");
              fHists[i][i_hist][i_quad].EvdEvTime->SetZTitle("Time (ns)");
           }
       }
    }

  return 0;
}

int PlotTME_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    // for each TME
    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){

        const TMuonEvent* tme=*i_tme;
        double tme_time=tme->GetTime();
        double tme_amp=tme->GetAmplitude();
        // for each Si packet
        for(int i_side=0;i_side<1;++i_side){
        //for(int i_side=0;i_side<2;++i_side){
            TMuonEvent::LeftRight_t side=(TMuonEvent::LeftRight_t) i_side;
            const SiliconHitList::const_iterator begin=tme->BeginSiEvents( side);
            const SiliconHitList::const_iterator end=tme->EndSiEvents( side);
            for(SiliconHitList::const_iterator i_SHit=begin; i_SHit!=end; ++i_SHit){
               const double deltaT=i_SHit->GetThickTime()- tme_time; 
               const double tot_E=i_SHit->GetTotalE();
               const double del_E=i_SHit->GetDeltaE();
               int quad=i_SHit->GetLastQuad()-1;
	       if(quad<0) continue;

               // Now fill all the histograms
               fHists[i_side][kNoCuts][quad].Fill(tot_E,del_E,deltaT);
               if(i_SHit->GetNThinHits()== 0) continue; // don't keep hits with nothing in the thin
               if(!tme->HasMuonPileup()) {
                  fHists[i_side][kMuPP][quad].Fill(tot_E,del_E,deltaT);
	          if( tme_amp > fMuScMin && tme_amp < fMuScMax){
                     fHists[i_side][kMuPP_muAmp][quad].Fill(tot_E,del_E,deltaT);
	             if(deltaT > fMinTime) {
                        fHists[i_side][kMuPP_muAmp_T500][quad].Fill(tot_E,del_E,deltaT);
                        if(fActiveStops(tme, TDetectorPulse::kFast)) {
                            fHists[i_side][kActiveStop][quad].Fill(tot_E,del_E,deltaT);
                        }
	             }
	          }
               }
            }
        } // each side
    } // each TME
  return 0;
}

int PlotTME_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  if(Debug()){
    for(int side=0;side<1;++side){
    //for(int side=0;side<2;++side){
      for(int pp=0;pp<kNHists;++pp){
         for(int quad=0;quad<4;++quad){
            fHists[side][pp][4].EvdE     ->Add(fHists[side][pp][quad].EvdE);
            fHists[side][pp][4].time     ->Add(fHists[side][pp][quad].time);
            fHists[side][pp][4].EvdEvTime->Add(fHists[side][pp][quad].EvdEvTime);
            cout<<"PlotTME_EvdE: "<< fHists[side][pp][quad].EvdE->GetName() <<" has "<<fHists[side][pp][quad].EvdE->GetEntries()<<endl;
          }
          cout<<"PlotTME_EvdE: "<< fHists[side][pp][4].EvdE->GetName() <<" has "<<fHists[side][pp][4].EvdE->GetEntries()<<endl;
       }
    }
  }
  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_EvdE, min_time,muSc_min, muSc_max,SiR2_min,SiR2_max)
