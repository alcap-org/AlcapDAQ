#include "TAPTree.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TAnalysedPulse.h"
#include "TTemplateFitAnalysedPulse.h"
#include "debug_tools.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
#include <cmath>
#include <algorithm>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;
extern SourceDetPulseMap gDetectorPulseMap;

TAPTree::TAPTree(modules::options* opts):
  BaseModule("TAPTree",opts){
}

TAPTree::~TAPTree(){
}

int TAPTree::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fAllDets.push_back(IDs::channel (kMuSc , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kSiT_1 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiT_2 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiT_3 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiT_4 , kSlow ));

    /*
    fAllDets.push_back(IDs::channel (kSiL1_1 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_2 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_3 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_4 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_5 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_6 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_7 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_8 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_9 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_10 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_11 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_12 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_13 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_14 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_15 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL1_16 , kSlow ));
    */
    fAllDets.push_back(IDs::channel (kSiL3 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiL3 , kFast ));

    fAllDets.push_back(IDs::channel (kSiR1_1, kSlow ));
    fAllDets.push_back(IDs::channel (kSiR1_2, kSlow ));
    fAllDets.push_back(IDs::channel (kSiR1_3 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiR1_4 , kSlow ));
    fAllDets.push_back(IDs::channel (kSiR2 , kSlow ));
    //    fAllDets.push_back(IDs::channel (kSiR3 , kSlow ));

    fAllDets.push_back(IDs::channel (kGeLoGain , kNotApplicable ));
    fAllDets.push_back(IDs::channel (kGeHiGain , kNotApplicable ));

    fTAPTree = new TTree("TAPTree", "");
    fTAPTree->Branch("runId", &fRunId);
    fTAPTree->Branch("blockId", &fBlockId);
    fTAPTree->Branch("TAPId", &fTAPId);
   
    for (DetectorList::const_iterator i_det=fAllDets.begin(); i_det!=fAllDets.end(); ++i_det){
      
      const std::string& i_detname = i_det->str();
      std::string branchname = modules::parser::ToCppValid(i_detname);
      fTAPTree->Branch(branchname.c_str(), &fChannels[i_detname]);
    }

    fRunId = SetupNavigator::Instance()->GetRunNumber();

  return 0;
}

int TAPTree::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  fBlockId = EventNavigator::Instance().EntryNo();

  // First loop through all TAPs and check with TPI IDs are double counted
  // (need to do this because otherwise the first occurrance of a double counted pulse won't be flagged correctly
  SimplePulse empty;
  for(SourceAnalPulseMap::const_iterator i_det=gAnalysedPulseMap.begin(); i_det!=gAnalysedPulseMap.end(); ++i_det){

      const IDs::source& i_src = i_det->first;
      const std::string& i_detname = i_src.Channel().str();
      const AnalysedPulseList *tap_list = &i_det->second;
        //if(Debug() && tap_list->empty()) DEBUG_PREFIX<<" no tap_list to fill for "<<i_det->first<<std::endl;

      SimplePulse& i_pulse = fChannels[i_detname];

      for (AnalysedPulseList::const_iterator i_tap = tap_list->begin(); i_tap != tap_list->end(); ++i_tap) {
	fTAPId = i_tap - tap_list->begin();

	SimplePulse pulse((*i_tap)->GetParentID(), (*i_tap)->GetTAPID(), (*i_tap)->GetEnergy(), (*i_tap)->GetAmplitude(), (*i_tap)->GetTime(), 0, 0);//, (*i_tap)->GetSamples());

	// Check to see if this is a "cut-off" pulse
	if (dynamic_cast<const TTemplateFitAnalysedPulse*>((*i_tap))) {
	  const TTemplateFitAnalysedPulse* tf_pulse =static_cast<const TTemplateFitAnalysedPulse*>((*i_tap));
	  if (tf_pulse->GetTemplateTimeOffset() > 3900) {
	    pulse.bit_mask |= kCutOff;
	  }
	}

	i_pulse = pulse;
	
	fTAPTree->Fill();

	i_pulse = empty;
      }
  }
    
  return 0;
}

int TAPTree::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TAPTree);
