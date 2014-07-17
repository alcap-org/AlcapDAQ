#ifndef MakeTemplate_h__
#define MakeTemplate_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "PulseTemplate.h"

#include "TFile.h"

#include <map>
#include <string>

class MakeTemplate : public BaseModule {

 private:
  TFile* fTemplateFile;
  std::map<std::string, PulseTemplate*> fTemplates;

 public:
  MakeTemplate(char *HistogramDirectoryName);
  ~MakeTemplate();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
};

#endif

