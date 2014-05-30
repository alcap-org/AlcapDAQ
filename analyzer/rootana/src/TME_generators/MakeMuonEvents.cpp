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

using std::string;
using std::map;
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;
extern StringDetPulseMap gDetectorPulseMap;

MakeMuonEvents::MakeMuonEvents(modules::options* opts):
  FillHistBase("MakeMuonEvents",opts),fOptions(opts){
      if(fOptions){
	  fAlgorithm=fOptions->GetString("algorithm");
      }
  dir->cd("/");
}

MakeMuonEvents::~MakeMuonEvents(){
}

int MakeMuonEvents::BeforeFirstEntry(TGlobalData *aData, TSetupData* aSetup){
    fGenerator=MakeGenerator(fAlgorithm);
    if(!fGenerator) return 1;
    return 0;
}

int MakeMuonEvents::ProcessEntry(TGlobalData *aData, TSetupData* aSetup){
  gMuonEvents.clear();
  int retVal=fGenerator->ProcessPulses(gMuonEvents,gDetectorPulseMap);
  return retVal;
}

TVMuonEventGenerator* MakeMuonEvents::MakeGenerator(const std::string& algorithm){
	TVMuonEventGenerator* aGenerator=NULL;
	if(algorithm=="MaxTimeDiff"){
		aGenerator=new MaxTimeDiffMEGenerator(fOptions);
	}else{
		cout<<"Unknown TVMuonEventGenerator requested: '"<<algorithm<<"'"<<endl;
	}
	return aGenerator;
}

ALCAP_REGISTER_MODULE(MakeMuonEvents,algorithm);
