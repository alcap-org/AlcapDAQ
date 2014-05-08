#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

extern void ZoomAxis(TH1* hist);

// This will generate the following PDFs:
// -- hDQ_IslandCounter_[DetName]_[BankName].png
// -- hDQ_IslandLength_[DetName]_[BankName].png
// -- hDQ_IslandRate_[DetName]_[BankName].png
// -- hDQ_IslandTimestamp_[DetName]_[BankName].png
// -- hDQ_Amplitude_[DetName]_[BankName].png
void GenerateTPIPlots(std::string filename) {

  std::cout << "Generating TPI plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1F *hDQ_IslandCounter;
  TH1F *hDQ_IslandLength;
  TH1F *hDQ_IslandRate;
  TH1F *hDQ_IslandTimestamp;
  TH1F *hDQ_Amplitude;

  // Loop through the histograms in the file and get the island histograms (bank and channel names may differ between runs)
  TDirectoryFile* dir = (TDirectoryFile*) file->Get("DataQuality_LowLevel");
  
  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
  TKey *dirKey;
  
  while ( (dirKey = (TKey*)nextDirKey()) ) {
    // Get the names of all the histograms and count them up
    if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0) {

      TCanvas *c1 = new TCanvas();

      std::string histogram_name = dirKey->ReadObj()->GetName();
      std::string histogram_location = "DataQuality_LowLevel/" + histogram_name;
      std::string pngname = "data_quality_figs/" + histogram_name + ".png";

      if (histogram_name.find("IslandCounter") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hDQ_IslandCounter);

	hDQ_IslandCounter->GetYaxis()->SetTitleOffset(1.3);
	ZoomAxis(hDQ_IslandCounter);
	hDQ_IslandCounter->Draw();
	c1->Print(pngname.c_str());
      }
      else if (histogram_name.find("IslandLength") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hDQ_IslandLength);

	hDQ_IslandLength->GetYaxis()->SetTitleOffset(1.3);
	ZoomAxis(hDQ_IslandLength);
	hDQ_IslandLength->Draw();
	c1->Print(pngname.c_str());
      }
      else if (histogram_name.find("IslandRate") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hDQ_IslandRate);

	hDQ_IslandRate->GetYaxis()->SetTitleOffset(1.3);
	ZoomAxis(hDQ_IslandRate);
	hDQ_IslandRate->Draw();
	c1->Print(pngname.c_str());
      }
      else if (histogram_name.find("IslandTimestamp") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hDQ_IslandTimestamp);

	hDQ_IslandTimestamp->GetYaxis()->SetTitleOffset(1.3);
	ZoomAxis(hDQ_IslandTimestamp);
	hDQ_IslandTimestamp->Draw();
	c1->Print(pngname.c_str());
      }
      else if (histogram_name.find("Amplitude") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hDQ_Amplitude);

	hDQ_Amplitude->GetYaxis()->SetTitleOffset(1.3);
	ZoomAxis(hDQ_Amplitude);
	hDQ_Amplitude->Draw();

	// Change Amplitude --> IslandAmplitude so that the PDF appears alphabetically next to
	// the others
	pngname.insert(pngname.find("Amplitude"), "Island");
	c1->Print(pngname.c_str());
      }
    }
  }
}
