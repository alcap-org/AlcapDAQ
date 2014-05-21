#include "TemplateArchive.h"

TemplateArchive::TemplateArchive(const char* filename) {

  fTemplateFile = new TFile(filename, "UPDATE");

}

TemplateArchive::~TemplateArchive() {
  fTemplateFile->Close();
}

// GetTemplate()
// -- gets a template from the template file
TH1F* TemplateArchive::GetTemplate(const char* template_name) {

  TH1F* hTemplate = (TH1F*) fTemplateFile->Get(template_name);
}

// SaveTemplate()
// -- saves a template to the template file
void TemplateArchive::SaveTemplate(TH1* hTemplate) {

  fTemplateFile->cd();
  hTemplate->Write();
}
