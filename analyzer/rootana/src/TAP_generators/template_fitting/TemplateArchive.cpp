#include "TemplateArchive.h"

// option is standard ROOT file options
TemplateArchive::TemplateArchive(const char* filename, const char* option) {
  TDirectory* tmp=gDirectory;
  fTemplateFile = new TFile(filename, option);
  fDirectory=TDirectory::CurrentDirectory();
  tmp->cd();
}

TemplateArchive::TemplateArchive(TDirectory* dir):fTemplateFile(NULL){
  fDirectory=dir;
}

TemplateArchive::~TemplateArchive() {
  if(fTemplateFile) fTemplateFile->Close();
}

TTemplate* TemplateArchive::GetTemplate(const IDs::channel& ch) {
  return GetTemplate(TTemplate::MakeName(ch).c_str());
}

// GetTemplate()
// -- gets a template from the template file
TTemplate* TemplateArchive::GetTemplate(const char* template_name) {

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
