#ifndef FillHistBase_h__
#define FillHistBase_h__ 1

#include "TGlobalData.h"
#include "TSetupData.h"
#include "TDirectory.h"

///////////////////////////////////////////////////
// This class is the generic analysis module     //
// which takes the TGlobalData pointer for the   //
// current tree event and processes it.          //
// Analyzers need to derive their real analysis  //
// modules from this class.                      //
///////////////////////////////////////////////////   

class FillHistBase
{
 public:
  FillHistBase(char *HistogramDirectoryName);
  virtual ~FillHistBase();

  int ProcessGenericEntry(TGlobalData *gData, TSetupData *gSetup);
 
 protected:
  TDirectory *dir;

 private:
   virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
