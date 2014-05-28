#include "%module%.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include <iostream>
using std::cout;
using std::endl;

%module%::%module%(modules::options* opts):
   FillHistBase("%module%",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
  fXMax=opts->GetDouble("x_max",100); 
  
}

%module%::~%module%(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int %module%::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----I'm debugging %module%::BeforeFirstEntry() "<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int %module%::ProcessEntry(TGlobalData* gData,TSetupData *setup){
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int %module%::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----I'm debugging %module%::AfterLastEntry()"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
//ALCAP_REGISTER_MODULE(%module% ,slow_gen,fast_gen);
ALCAP_REGISTER_MODULE(%module%);
