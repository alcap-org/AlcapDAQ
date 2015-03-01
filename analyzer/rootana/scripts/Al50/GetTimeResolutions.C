#include <string>
#include <iostream>

#include "TFile.h"
#include "TDirectory.h"

void GetTimeResolutions(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");

  TDirectory* dir = file->GetDirectory("PlotTAP_TDiff");
  dir->ls();
  TIter next(dir->GetListOfKeys());
  while (TObject* obj = next()) {
    std::cout << obj->GetName() << std::endl;
  }

}
