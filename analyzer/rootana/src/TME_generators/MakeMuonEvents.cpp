#include "MakeMuonEvents.h"
#include <TMuonEvent.h>
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <algorithm>

#include "MaxTimeDiffMEGenerator.h"
#include "RegisterModule.inc"
#include "TMEGeneratorFactory.h"

using std::string;
using std::map;
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;
extern SourceDetPulseMap gDetectorPulseMap;

MakeMuonEvents::MakeMuonEvents(modules::options* opts):
  BaseModule("MakeMuonEvents",opts,true),fOptions(opts){
      if(fOptions){
          fAlgorithm=fOptions->GetString("algorithm");
      }
  dir->cd("/");
}

MakeMuonEvents::~MakeMuonEvents(){
}

int MakeMuonEvents::BeforeFirstEntry(TGlobalData *aData, const TSetupData* aSetup){
    if(fAlgorithm.empty()) return 1;
    fGenerator=TMEGeneratorFactory::Instance()->createModule(
            fAlgorithm,new TMEGeneratorOptions(fAlgorithm,fOptions));
    if(!fGenerator) return 2;
    return 0;
}

int MakeMuonEvents::ProcessEntry(TGlobalData *aData, const TSetupData* aSetup){
  //gMuonEvents.clear();
  int retVal=fGenerator->ProcessPulses(gMuonEvents,gDetectorPulseMap);

  if (!gMuonEvents.empty()) {
    // time order the TMEs since we have used more than one SiT channel
    std::sort(gMuonEvents.begin(), gMuonEvents.end(), TMuonEvent::PointerCompare());
  }

  return retVal;
}

ALCAP_REGISTER_MODULE(MakeMuonEvents,algorithm);
