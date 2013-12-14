#include "FillHistBase.h"

FillHistBase::FillHistBase(char *HistogramDirectoryName,TSetupData* setup):fSetup(setup){
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
