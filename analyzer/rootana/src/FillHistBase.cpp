#include "FillHistBase.h"

FillHistBase::FillHistBase(char *HistogramDirectoryName){
  dir = gDirectory->mkdir(HistogramDirectoryName);
  gDirectory->cd("/");
}

FillHistBase::~FillHistBase()
{
}

int FillHistBase::ProcessEntry(TGlobalData *gData){
  // This is a virtual function and should be overwritten by the deriving analysis module!
  return 0;
}

int FillHistBase::ProcessGenericEntry(TGlobalData *gData){
  // This is called by our main routine and would allow later to split into different 
  // process routines if we have more than one Tree and hence different tpyes of data input.

  if(dir) dir->cd();
  int ret = ProcessEntry(gData);
  gDirectory->cd("/");

  return ret;
}
