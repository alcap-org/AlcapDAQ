#ifndef FillHistBase_h__
#define FillHistBase_h__ 1

#include "TGlobalData.h"
#include "TSetupData.h"
#include "TDirectory.h"

#include <iostream>
// This is an efficient way to switch on and off printing text for debugging
#ifdef USE_PRINT_OUT
#define PrintOut(message) \
std::cout<<message; 
#warning Verbose is on
#else 
#define PrintOut(message) 
//#warning Verbose is off
#endif


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
  FillHistBase(const char *HistogramDirectoryName, TSetupData* setup=NULL);
  virtual ~FillHistBase();

  int ProcessGenericEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData){return 0;};
  virtual int AfterLastEntry(TGlobalData* gData){return 0;};
  const char* GetName()const{return dir->GetName();};
 
 protected:
  TDirectory *dir;
  TSetupData* fSetup;

 private:
   virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
