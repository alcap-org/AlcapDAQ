#ifndef FillHistBase_h__
#define FillHistBase_h__ 1

#include "TGlobalData.h"
#include "TDirectory.h"
#include <iostream>
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
  FillHistBase(const char *HistogramDirectoryName);
  virtual ~FillHistBase();

  virtual int ProcessGenericEntry(TGlobalData *gData);
  virtual int BeforeFirstEntry(TGlobalData* gData){return 0;};
  virtual int AfterLastEntry(TGlobalData* gData){return 0;};
  
 protected:
  TDirectory *dir;
  
 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif
