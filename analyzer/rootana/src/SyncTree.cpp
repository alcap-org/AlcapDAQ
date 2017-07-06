#include "SyncTree.h"
#include "definitions.h"
#include "EventNavigator.h"
#include "IdBoard.h"
#include "IdChannel.h"
#include "RegisterModule.inc"
#include "SetupNavigator.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
using IDs::board;
using IDs::channel;

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
using std::map;
using std::pair;
using std::string;
using std::vector;

extern SourceAnalPulseMap gAnalysedPulseMap;

SyncTree::SyncTree(modules::options* opts) :
    BaseModule("SyncTree",opts) {
}

int SyncTree::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup) {
  for (SourceAnalPulseMap::const_iterator taps = gAnalysedPulseMap.begin();
       taps != gAnalysedPulseMap.end();
       ++taps) {
    string bank  = SetupNavigator::Instance()->GetBank(taps->first.Channel());
    TTree* tr    = new TTree(bank.c_str(), bank.c_str());
    Double_t* t0 = new Double_t;
    Double_t* t1 = new Double_t;
    tr->Branch("t0",    t0);
    tr->Branch("t1",    t1);
    tr->Branch("block", &fBlock);
    fT0s  [bank] = t0;
    fT1s  [bank] = t1;
    fTrees[bank] = tr;
  }
  return 0;
}

int SyncTree::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup) {
  fBlock = EventNavigator::Instance().EntryNo();
  for (SourceAnalPulseMap::const_iterator taps = gAnalysedPulseMap.begin();
       taps != gAnalysedPulseMap.end();
       ++taps) {
    if (taps->second.empty()) continue;
    channel        ch    = taps->first.Channel();
    string         bank  = SetupNavigator::Instance()->GetBank(ch);
    pair<int, int> syncs = SetupNavigator::Instance()->GetChanSyncs(ch);
    std::cout << taps->first << ' ' << taps->second.size() << ' '
              << syncs.first << ' ' << syncs.second << std::endl;
    *fT0s[bank] = taps->second.at(syncs.first) ->GetTime();
    *fT1s[bank] = taps->second.at(syncs.second)->GetTime();
    fTrees[bank]->Fill();
  }
  return 0;
}

ALCAP_REGISTER_MODULE(SyncTree)
