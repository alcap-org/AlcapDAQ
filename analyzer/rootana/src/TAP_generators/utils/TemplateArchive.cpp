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
TH1D* TemplateArchive::GetTemplate(const char* template_name) {

  TH1D* hTemplate = (TH1D*) fTemplateFile->Get(template_name);
}

// SaveTemplate()
// -- saves a template to the template file
void TemplateArchive::SaveTemplate(TH1* hTemplate) {

  if (hTemplate) {
    fTemplateFile->cd();
    hTemplate->Write();
  }
}
