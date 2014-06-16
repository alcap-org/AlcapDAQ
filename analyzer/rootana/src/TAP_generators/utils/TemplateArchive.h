#ifndef TemplateArchive_h__
#define TemplateArchive_h__

#include "TFile.h"
#include "TH1.h"

class TemplateArchive {
  
 private:
  TFile* fTemplateFile;

 public:
  TemplateArchive(const char* filename, const char* option);
  ~TemplateArchive();

 public:
  TH1D* GetTemplate(const char* template_name);
  void SaveTemplate(TH1* hTemplate);

};

#endif
