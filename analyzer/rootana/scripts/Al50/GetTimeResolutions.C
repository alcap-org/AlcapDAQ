#include <string>
#include <iostream>

#include "TFile.h"
#include "TDirectory.h"
#include "TH2.h"
#include "TH1.h"

void GetTimeResolutions(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");

  std::string modulename = "PlotTAP_TDiff";
  TDirectory* module_dir = file->GetDirectory(modulename.c_str());

  TIter next_dir(module_dir->GetListOfKeys());
  while (TObject* dir_obj = next_dir()) {
    std::string foldername = dir_obj->GetName();
    //    std::string full_path = modulename + "/" + foldername;
    TDirectory* folder_dir = module_dir->GetDirectory(foldername.c_str());

    TIter next_plot(folder_dir->GetListOfKeys());
    while (TObject* det_obj = next_plot()) {
      std::string plotname = det_obj->GetName();
      if (plotname.find("AmpA") != std::string::npos) {
	TH2F* hist = (TH2F*) folder_dir->Get(plotname.c_str());
	std::cout << "AE: " << plotname << " has " << hist->GetEntries() << " entries" << std::endl;
	hist->Draw("COLZ");
      }
    }
    
  }

}
