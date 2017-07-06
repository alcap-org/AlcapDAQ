#pragma once

#include "BaseModule.h"
#include "IdBoard.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TTree.h"

#include <map>

class SyncTree : public BaseModule {
 public:
  SyncTree(modules::options* opts);
  ~SyncTree(){};

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;}
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup);

  std::map< std::string, TTree*    > fTrees;
  std::map< std::string, Double_t* > fT0s;
  std::map< std::string, Double_t* > fT1s;
  Int_t fBlock;
};
