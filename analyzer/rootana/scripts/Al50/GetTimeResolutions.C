#include <string>
#include <iostream>

#include "TFile.h"
#include "TDirectory.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TFitResult.h"

void GetTimeResolutions(std::string filename) {

  TCanvas* c1 = new TCanvas("c1", "c1");

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
	TH1D* projection = hist->ProjectionX();
	//	projection->Rebin(4);

	TFitResultPtr fit_result = projection->Fit("gaus", "QS", "", -150,150);
	std::cout << foldername << ": sigma = " << fit_result->Parameter(2) << " +- " << fit_result->ParError(2) << std::endl;
	//" chi^2 / ndf = " << fit_result->Chi2() << " / " << fit_result->Ndf() << " = " << fit_result->Chi2() / fit_result->Ndf() << std::endl;
	projection->SetTitle("");
	std::string xaxis_title = "Time Difference (t_{" + foldername + "} - t_{#muSc}) [ns]";
	projection->SetXTitle(xaxis_title.c_str());
	projection->SetLineWidth(2);
	projection->SetStats(false);
	projection->Draw();
	std::string picname = foldername + ".pdf";
	c1->Print(picname.c_str());
	
      }
    }
    
  }

}
