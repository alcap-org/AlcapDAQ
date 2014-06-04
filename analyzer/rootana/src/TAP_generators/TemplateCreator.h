#ifndef TEMPLATECREATOR_H_
#define TEMPLATECREATOR_H_

#include "FillHistBase.h"
class gTGlobalData;
class gTSetupData;
class gModulesOptions;

class TemplateCreator : public FillHistBase{

 public:
  TemplateCreator(modules::options* opts);
  ~TemplateCreator();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);
};

#endif //TEMPLATECREATOR_H_
