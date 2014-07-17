#include "MakeMuonEvents.h"
#include <TMuonEvent.h>
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
#include <string>
#include <map>
#include <utility>

#include "MaxTimeDiffMEGenerator.h"
#include "RegisterModule.inc"
#include "TMEGeneratorFactory.h"

using std::string;
using std::map;
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;
extern StringDetPulseMap gDetectorPulseMap;

MakeMuonEvents::MakeMuonEvents(modules::options* opts):
  BaseModule("MakeMuonEvents",opts),fOptions(opts){
      if(fOptions){
	  fAlgorithm=fOptions->GetString("algorithm","MaxTimeDiff");
      }
  dir->cd("/");
}

MakeMuonEvents::~MakeMuonEvents(){
}

int MakeMuonEvents::BeforeFirstEntry(TGlobalData *aData, const TSetupData* aSetup){
    if(fAlgorithm.empty()) return 1;
    TMEGeneratorOptions* opts=NULL;
    if(Debug()){
       opts = new TMEGeneratorOptions(fAlgorithm);
       opts->SetOption("debug","");
    }
    fGenerator=TMEGeneratorFactory::Instance()->createModule(fAlgorithm,opts);
    if(!fGenerator) return 2;
    return 0;
}

int MakeMuonEvents::ProcessEntry(TGlobalData *aData, const TSetupData* aSetup){
  gMuonEvents.clear();
  int retVal=fGenerator->ProcessPulses(gMuonEvents,gDetectorPulseMap);
  return retVal;
}


ALCAP_REGISTER_MODULE(MakeMuonEvents,algorithm);
