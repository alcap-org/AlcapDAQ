#ifndef TemplateArchive_h__
#define TemplateArchive_h__

#include "TFile.h"
#include "TH1.h"

class TemplateArchive {
  
 private:
  TFile* fTemplateFile;

 public:
  TemplateArchive(const char* filename);
  ~TemplateArchive();

 public:
  TH1F* GetTemplate(const char* template_name);
  void SaveTemplate(TH1F* hTemplate, const char* template_name);

};

#endif
