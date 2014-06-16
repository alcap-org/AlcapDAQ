#ifndef %MODULE%_H_
#define %MODULE%_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

class %module% : public BaseModule{

 public:
  %module%(modules::options* opts);
  ~%module%();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  int fXMax;
};

#endif //%MODULE%_H_
