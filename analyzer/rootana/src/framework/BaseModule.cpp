#include "BaseModule.h"
#include "ModulesOptions.h"
#include "TDirectory.h"
#include <iostream>

BaseModule::BaseModule(const char *HistogramDirectoryName,modules::options* opts,TSetupData* setup):
	fSetup(setup),fDebug(false){
  if(opts){
    fDebug=(opts->HasOption("debug") && (opts->GetOption("debug").empty() || opts->GetBool("debug")));
    //std::cout<<"Should we debug module: "<<HistogramDirectoryName<<": "<<(fDebug?"yes":"no")<<std::endl;
  }
  dir = gDirectory->mkdir(HistogramDirectoryName);
  dir->cd();
}

BaseModule::~BaseModule()
{
}

int BaseModule::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  // This is a virtual function and should be overwritten by the deriving analysis module!
  return 0;
}

int BaseModule::ProcessGenericEntry(TGlobalData *gData, TSetupData *gSetup){
  // This is called by our main routine and would allow later to split into different 
  // process routines if we have more than one Tree and hence different tpyes of data input.

  if(dir) dir->cd();
  int ret = ProcessEntry(gData, gSetup);
  gDirectory->cd("/");

  return ret;
}
