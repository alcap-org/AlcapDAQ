#include "MakeSiliconEvents.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
using std::cout;
using std::endl;

MakeSiliconEvents::MakeSiliconEvents(modules::options* opts):
   BaseModule("MakeSiliconEvents",opts){
}

MakeSiliconEvents::~MakeSiliconEvents(){
}

int MakeSiliconEvents::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

int MakeSiliconEvents::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

int MakeSiliconEvents::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(MakeSiliconEvents);
