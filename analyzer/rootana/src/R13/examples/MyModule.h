#ifndef MyModule_h__
#define MyModule_h__

#include "BaseModule.h"
#include "TGlobalData.h"

class MyModule : public BaseModule{
 public:
  MyModule(const char *HistogramDirectoryName);
  ~MyModule();

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

