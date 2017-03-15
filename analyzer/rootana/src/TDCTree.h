#ifndef TDCTree_h__
#define TDCTree_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TTree.h"

#include <vector>

class TDCTree : public BaseModule{
 public:
  TDCTree(modules::options* opts);
  ~TDCTree();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};

  static const int      NCH;
  static const Double_t DT;

  std::vector<TTree*>   fTrees;
  std::vector<Double_t> fTimes;
  Int_t fBlock;
};

#endif

