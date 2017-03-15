#include "TDCTree.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include "definitions.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"

using std::string;
using std::map;
using std::vector;

extern SourceAnalPulseMap gAnalysedPulseMap;

TDCTree::TDCTree(modules::options* opts) :
    BaseModule("TDCTree",opts), fTrees(NCH), fTimes(NCH) {
    for (int i = 0; i < NCH; ++i) {
      TString bank = TString::Format("T4%02d", i);
      fTrees[i] = new TTree(bank, bank);
      fTrees[i]->Branch("t", &fTimes[i]);
      fTrees[i]->Branch("block", &fBlock);
    }
}

TDCTree::~TDCTree(){
}

int TDCTree::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup) {
  fBlock = EventNavigator::Instance().EntryNo();
  for (map< string, vector<int64_t> >::const_iterator i_det = gData->fTDCHitsToChannelMap.begin();
       i_det != gData->fTDCHitsToChannelMap.end();
       i_det++) {
    int ch = std::atoi(i_det->first.substr(2).c_str());
    for (int i = 0; i < i_det->second.size(); ++i) {
      fTimes[ch] = i_det->second[i]*(1./40.96);
      fTrees[ch]->Fill();
    }
  }
  return 0;
}

const int      TDCTree::NCH = 16;
const Double_t TDCTree::DT  = 1./40.96; // ns

ALCAP_REGISTER_MODULE(TDCTree)
