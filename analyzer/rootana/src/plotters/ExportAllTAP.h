#pragma once

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class ExportAllTAP : public BaseModule {
 public:
  ExportAllTAP(modules::options* opts) : BaseModule("ExportAllTAP", opts) {}
  ~ExportAllTAP()                                                         {}

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup) { return 0; }
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup)     { return 0; }
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
};

