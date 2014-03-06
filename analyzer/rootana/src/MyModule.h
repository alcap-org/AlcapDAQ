#ifndef MyModule_h__
#define MyModule_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class MyModule : public FillHistBase{
 public:
  MyModule(const char *HistogramDirectoryName);
  ~MyModule();

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

