#include "TemplateArchive.h"

// option is standard ROOT file options
TemplateArchive::TemplateArchive(const char* filename, const char* option) {
  fTemplateFile = new TFile(filename, option);
  fDirectory=TDirectory::CurrentDirectory();
}

TemplateArchive::TemplateArchive(TDirectory* dir):fTemplateFile(NULL){
  fDirectory=dir;
}

TemplateArchive::~TemplateArchive() {
  if(fTemplateFile) fTemplateFile->Close();
}

// GetTemplate()
// -- gets a template from the template file
const TTemplate* TemplateArchive::GetTemplate(const char* template_name) {

  TTemplate* hTemplate = NULL;
  fDirectory->GetObject(template_name, hTemplate);
  return hTemplate;
}

// SaveTemplate()
// -- saves a template to the template file
void TemplateArchive::SaveTemplate(const TTemplate* hTemplate) {

  TDirectory* oldDir = TDirectory::CurrentDirectory(); // should be the directory we were already in
  if (hTemplate) {
    fDirectory->cd();
    hTemplate->Write();
  }
  oldDir->cd(); // go back to the old directory
}
