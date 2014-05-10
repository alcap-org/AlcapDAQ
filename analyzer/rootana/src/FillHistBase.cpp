#include "FillHistBase.h"
#include "ModulesOptions.h"
#include <iostream>

FillHistBase::FillHistBase(const char *HistogramDirectoryName,modules::options* opts,TSetupData* setup):
	fSetup(setup),fDebug(false){
  if(opts){
    fDebug=opts->HasOption("debug");
    std::cout<<"Should we debug module: "<<HistogramDirectoryName<<": "<<(fDebug?"yes":"no")<<std::endl;
  }
  dir = gDirectory->mkdir(HistogramDirectoryName);
  dir->cd();
}

FillHistBase::~FillHistBase()
{
}

int FillHistBase::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  // This is a virtual function and should be overwritten by the deriving analysis module!
  return 0;
}

int FillHistBase::ProcessGenericEntry(TGlobalData *gData, TSetupData *gSetup){
  // This is called by our main routine and would allow later to split into different 
  // process routines if we have more than one Tree and hence different tpyes of data input.

  if(dir) dir->cd();
  int ret = ProcessEntry(gData, gSetup);
  gDirectory->cd("/");

  return ret;
}
