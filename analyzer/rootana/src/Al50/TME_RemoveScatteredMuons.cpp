#include "TME_RemoveScatteredMuons.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
using std::cout;
using std::endl;

TME_RemoveScatteredMuons::TME_RemoveScatteredMuons(modules::options* opts):
   BaseModule("TME_RemoveScatteredMuons",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?

  fTimeCut = opts->GetDouble("time_cut", 0); // by default not used
}

TME_RemoveScatteredMuons::~TME_RemoveScatteredMuons(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TME_RemoveScatteredMuons::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----TME_RemoveScatteredMuons::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int TME_RemoveScatteredMuons::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TME_RemoveScatteredMuons::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----TME_RemoveScatteredMuons::AfterLastEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(TME_RemoveScatteredMuons,x_max);
