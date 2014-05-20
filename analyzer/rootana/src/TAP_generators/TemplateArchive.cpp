#include "TemplateArchive.h"

TemplateArchive::TemplateArchive(const char* filename) {

  fTemplateFile = new TFile(filename, "UPDATE");

}

TemplateArchive::~TemplateArchive() {}
