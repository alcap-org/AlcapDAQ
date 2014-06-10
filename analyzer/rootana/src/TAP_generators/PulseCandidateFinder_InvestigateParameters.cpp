#include "PulseCandidateFinder_InvestigateParameters.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
using std::cout;
using std::endl;

PulseCandidateFinder_InvestigateParameters::PulseCandidateFinder_InvestigateParameters(modules::options* opts):
   FillHistBase("PulseCandidateFinder_InvestigateParameters",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
  fXMax=opts->GetDouble("x_max",100); 
  
}

PulseCandidateFinder_InvestigateParameters::~PulseCandidateFinder_InvestigateParameters(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PulseCandidateFinder_InvestigateParameters::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PulseCandidateFinder_InvestigateParameters::BeforeFirstEntry(): I'm debugging!"<<endl;
     cout<<"-----PulseCandidateFinder_InvestigateParameters::BeforeFirstEntry(): x_max is: "<<fXMax<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PulseCandidateFinder_InvestigateParameters::ProcessEntry(TGlobalData* gData,TSetupData *setup){
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PulseCandidateFinder_InvestigateParameters::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PulseCandidateFinder_InvestigateParameters::AfterLastEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PulseCandidateFinder_InvestigateParameters,x_max);
