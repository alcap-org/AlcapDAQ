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
// -- hSyncCount

void GenerateTimingPlots(std::string filename) {

  std::cout << "Generating Timing plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  TFile* file = new TFile(filename.c_str(), "READ");

  //the histograms
  TH1I *hSyncCount;

   // Loop through the histograms in the file and get the island histograms (bank and channel names may differ between runs)
  TDirectoryFile* dir = (TDirectoryFile*) file->Get("DataQuality_LowLevel/SyncCount");
  
  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
  TKey *dirKey;
  
  while ( (dirKey = (TKey*)nextDirKey()) ) {
    // Get the names of all the histograms and count them up
    if (strcmp(dirKey->ReadObj()->ClassName(), "TH1I") == 0) {

      TCanvas *c1 = new TCanvas();

      std::string histogram_name = dirKey->ReadObj()->GetName();
      std::string histogram_location = "DataQuality_LowLevel/SyncCount/" + histogram_name;
      std::string pngname = "data_quality_figs/" + histogram_name + ".png";

      if (histogram_name.find("SyncCount") != std::string::npos) {
	file->GetObject(histogram_location.c_str(),hSyncCount);

	hSyncCount->GetYaxis()->SetTitleOffset(1.3);


	hSyncCount->Draw();
	c1->Print(pngname.c_str());
      }
    }
  }
}

