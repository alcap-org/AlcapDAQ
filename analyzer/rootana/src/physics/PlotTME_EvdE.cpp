#include "PlotTME_EvdE.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TMuonEvent.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "debug_tools.h"

#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

PlotTME_EvdE::PlotTME_EvdE(modules::options* opts):
   BaseModule("PlotTME_EvdE",opts),
   fMinTime(opts->GetDouble("min_time",500)),
   fMuScMax(opts->GetDouble("muSc_max",1e9)),
   fMuScMin(opts->GetDouble("muSc_min",0)){
}

PlotTME_EvdE::~PlotTME_EvdE(){
}

int PlotTME_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    const char* titles[kNHists]={ "Muon Pile-up allowed",
                                  "Muon Pile-up protection",
                                  "Muon amp. cut and pile-up protection",
                                  Form("#mu PP, #mu amp. cut and time in thick > %gns",fMinTime)
                                };
    const char* names[kNHists]={"","_mu_PP",  "_mu_PP_mu_amp","_mu_PP_mu_amp_t_cut"};

std::vector<double> t_bins;
while(true){
double t=1;
for(; t<100; t+=10) t_bins.push_back(t);
for(; t<500; t+=20) t_bins.push_back(t);
for(; t<1000; t+=100) t_bins.push_back(t);
for(; t<10000; t+=1000) t_bins.push_back(t);
break;
}
for(unsigned i=0;i<t_bins.size();i++) cout<<i<<": "<<t_bins[i]<<endl;

    // for left and right packages
    for(int i=0;i<2;i++){
        TString title_kernel= i==0? "Left" : "Right";
        title_kernel+= " Silicon with ";
        TString name_kernel= i==0? "_L" : "_R";

        int min_dE=0, max_dE=7e3;
        int min_E=0, max_E=25e3;
        int min_dT=0, max_dT=10000;
        for(int i_hist=0; i_hist< kNHists; ++i_hist){
           for(int i_quad=0; i_quad< 4; ++i_quad){
              fHists[i][i_hist][i_quad].EvdE=new TH2F(
                      Form("hEvdE_%s%s_%d",name_kernel.Data(),names[i_hist],i_quad-1),
                      Form("E vs dE for %s %s %d",title_kernel.Data(),titles[i_hist],i_quad-1),
                      400,min_E,max_E,400,min_dE,max_dE);
              fHists[i][i_hist][i_quad].EvdE->SetXTitle("Total energy (KeV)");
              fHists[i][i_hist][i_quad].EvdE->SetYTitle("dE/dx (KeV)");

              fHists[i][i_hist][i_quad].time=new TH1F(
                      Form("hTime%s%s_%d",name_kernel.Data(),names[i_hist],i_quad-1),
                      Form("Timing for %s %s %d",title_kernel.Data(),titles[i_hist],i_quad-1),
                      600,min_dT,max_dT);
              fHists[i][i_hist][i_quad].time->SetXTitle("Time (ns)");

              fHists[i][i_hist][i_quad].EvdEvTime=new TH3I(
                      Form("hEvdEvTime%s%s_%d",name_kernel.Data(),names[i_hist],i_quad-1),
                      Form("E vs dE vs Timing for %s %s %d",title_kernel.Data(),titles[i_hist],i_quad-1),
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
        for(int i_side=0;i_side<2;++i_side){
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
    for(int side=0;side<2;++side){
      for(int pp=0;pp<3;++pp){
         for(int quad=0;quad<4;++quad){
            cout<<"PlotTME_EvdE: "<< fHists[side][pp][quad].EvdE->GetName() <<" has "<<fHists[side][pp][quad].EvdE->GetEntries()<<endl;
          }
       }
    }
  }
  return 0;
}

ALCAP_REGISTER_MODULE(PlotTME_EvdE, min_time,muSc_min, muSc_max)
