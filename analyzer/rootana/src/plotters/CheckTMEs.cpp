#include "CheckTMEs.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"

#include <TH1F.h>
#include <TH2F.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

CheckTMEs::CheckTMEs(modules::options* opts):
   BaseModule("CheckTMEs",opts),fNullCount(0),fTdpCount(0){
}

CheckTMEs::~CheckTMEs(){
}

int CheckTMEs::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    //fDetectors.push_back(IDs::channel (kGe     , kNotApplicable ));
    //fDetectors.push_back(IDs::channel (kScR    , kNotApplicable ));
    //fDetectors.push_back(IDs::channel (kScL    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScVe   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScGe   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR2   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL2   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kMuSc   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kMuScA  , kNotApplicable ));
    //fDetectors.push_back(IDs::channel (kNDet   , kNotApplicable ));
    //fDetectors.push_back(IDs::channel (kNDet2  , kNotApplicable ));

    // Plot the total number of pulses
    fTotalPulses=new TH1F("hTotalPulses", "Total number of pulses per TME", 100, 0 ,100);
    fTotalPulses->SetXTitle("Number of pulses");

    // Plot the number of pulses per channel
    fPulsesPerDetector=new TH2F("hPulsesPerChannel", "Pulses per channel per TME", 
            50, 0 ,50,fDetectors.size(),0,fDetectors.size());
    fPulsesPerDetector->SetXTitle("Number of pulses");
    //fPulsesPerDetector->SetYTitle("Detector");

    fTDiffPerDetector=new TH2F("hTDiffPerChannel", "TDiff to muSc for each channel per TME", 
            5000, -1.2e4,1.2e4,fDetectors.size(),0,fDetectors.size());
    fTDiffPerDetector->SetXTitle("TDiff to central muon (ns)");

    for(DetectorList::const_iterator i_det=fDetectors.begin();
            i_det!=fDetectors.end(); ++i_det){
        fPulsesPerDetector->GetYaxis()->SetBinLabel(i_det-fDetectors.begin()+1, i_det->str().c_str());
        fTDiffPerDetector->GetYaxis()->SetBinLabel(i_det-fDetectors.begin()+1, i_det->str().c_str());
    }
    fPulsesPerDetector->SetDrawOption("colz");
    fTDiffPerDetector->SetDrawOption("colz");

    // Plot the number of TME flags
    fFlags=new TH1F("hFlags", "Number of flagged per TME", 5,0,5);
    fFlags->SetYTitle("Number of pulses");
    int count=0;
    fFlags->GetXaxis()->SetBinLabel(++count,"Healthy");
    fFlags->GetXaxis()->SetBinLabel(++count,"Late");
    fFlags->GetXaxis()->SetBinLabel(++count,"Early");
    fFlags->GetXaxis()->SetBinLabel(++count,"Muon Hit");
    fFlags->GetXaxis()->SetBinLabel(++count,"Muon PileUp");

  return 0;
}

int CheckTMEs::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    // Loop over each TME
    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){
        // total pulses
        fTotalPulses->Fill((*i_tme)->TotalNumPulses());
        for(DetectorList::const_iterator i_det=fDetectors.begin();
                i_det!=fDetectors.end(); ++i_det){
            // pulses per channel
            int N=0, n;
            double tme_time= (*i_tme)->GetTime();
	    int source_index=(*i_tme)->GetSourceIndex(*i_det);
            while(source_index>-1){
	      const IDs::source& source=(*i_tme)->GetSource(source_index);
              n=(*i_tme)->NumPulses(source);
              N+=n;
              for(int i=0; i<n; ++i){
                 const TDetectorPulse* tdp=(*i_tme)->GetPulse(source,i);
	         if(tdp && (tdp->IsPairedPulse() || !tdp->CouldBePaired()) )
                    fTDiffPerDetector->Fill(tdp->GetTime() - tme_time, i_det - fDetectors.begin());
                 else if(!tdp)
                    ++fNullCount;
                ++fTdpCount;
	      }
	     source_index=(*i_tme)->GetSourceIndex(*i_det,source_index+1);
	    }
            fPulsesPerDetector->Fill(N, i_det - fDetectors.begin());
        }
        // fill flags hist
        if((*i_tme)->HasMuonHit()) fFlags->Fill("Muon Hit",1.);
        if((*i_tme)->HasMuonPileup()) fFlags->Fill("Muon PileUp",1.);
        //if((*i_tme)->WasEarlyInEvent()) fFlags->Fill("Early",1.);
        //else if((*i_tme)->WasLateInEvent()) fFlags->Fill("Late",1.);
        //else  
            fFlags->Fill("Healthy",1.);
    }
  return 0;
}

int CheckTMEs::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  cout<<"CheckTMEs::AfterLastEntry: NULL TDP in TME ("<<++fNullCount<<")"<<endl;
  cout<<"CheckTMEs::AfterLastEntry: Total number of TDPs ("<<++fTdpCount<<")"<<endl;
                  cout<<"CheckTMEs::AfterLastEntry: Total entries in fTDiffPerDetector: "<<fTDiffPerDetector->GetEntries()<<endl;
  return 0;
}

ALCAP_REGISTER_MODULE(CheckTMEs);
