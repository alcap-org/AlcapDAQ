#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

// This will generate the following PDFs:
// -- hDQ_PulseShapes_[DetName]_[BankName].pdf
void GeneratePulseShapesPlots(std::string filename) {

  std::cout << "Generating pulse shape plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH2F *hDQ_PulseShapes;

  // Loop through the histograms in the file and get the island histograms (bank and channel names may differ between runs)
  TDirectoryFile* dir = (TDirectoryFile*) file->Get("DataQuality_LowLevel");
  
  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
  TKey *dirKey;
  
  while ( (dirKey = (TKey*)nextDirKey()) ) {
    // Get the names of all the histograms and count them up
    if (strcmp(dirKey->ReadObj()->ClassName(), "TH2F") == 0) {

      TCanvas *c1 = new TCanvas();

      std::string histogram_name = dirKey->ReadObj()->GetName();
      std::string histogram_location = "DataQuality_LowLevel/" + histogram_name;
      std::string pdfname = "data_quality_figs/" + histogram_name + ".pdf";

      if (histogram_name.find("PulseShapes") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hDQ_PulseShapes);

	hDQ_PulseShapes->GetYaxis()->SetTitleOffset(1.3);
	hDQ_PulseShapes->Draw("COLZ");
	c1->SetLogz();
	c1->Print(pdfname.c_str());
      }
    }
  }
}
