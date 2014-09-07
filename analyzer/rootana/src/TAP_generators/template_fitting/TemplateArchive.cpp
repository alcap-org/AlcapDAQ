#include "TemplateArchive.h"

// option is standard ROOT file options
TemplateArchive::TemplateArchive(const char* filename, const char* option) {

  fTemplateFile = new TFile(filename, option);

}

TemplateArchive::~TemplateArchive() {
  fTemplateFile->Close();
}

// GetTemplate()
// -- gets a template from the template file
const TTemplate* TemplateArchive::GetTemplate(const char* template_name) {

  TTemplate* hTemplate = NULL;
  fTemplateFile->GetObject(template_name, hTemplate);
  return hTemplate;
}

// SaveTemplate()
// -- saves a template to the template file
void TemplateArchive::SaveTemplate(const TTemplate* hTemplate) {

  TDirectory* oldDir = TDirectory::CurrentDirectory(); // should be the directory we were already in
  if (hTemplate) {
    fTemplateFile->cd();
    hTemplate->Write();
  }
  oldDir->cd(); // go back to the old directory
}
