#ifndef TEMPLATECREATOR_H_
#define TEMPLATECREATOR_H_

#include "BaseModule.h"
class gTGlobalData;
class gTSetupData;
class gModulesOptions;

#include "utils/TemplateArchive.h"

class TemplateCreator : public BaseModule{

 public:
  TemplateCreator(modules::options* opts);
  ~TemplateCreator();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  TemplateArchive* fTemplateArchive;
};

#endif //TEMPLATECREATOR_H_
