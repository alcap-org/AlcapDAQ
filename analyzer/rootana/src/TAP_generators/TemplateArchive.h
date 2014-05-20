#ifndef TemplateArchive_h__
#define TemplateArchive_h__

#include "TFile.h"

class TemplateArchive {
  
 private:
  TFile* fTemplateFile;

 public:
  TemplateArchive(const char* filename);
  ~TemplateArchive();

};

#endif
