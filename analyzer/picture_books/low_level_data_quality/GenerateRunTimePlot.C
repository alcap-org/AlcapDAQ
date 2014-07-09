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
// -- hDQ_RunTime.png
void GenerateRunTimePlot(std::string filename) {

  std::cout << "Generating Run Time plot..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1F *hDQ_RunTime;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_RunTime",hDQ_RunTime);

  hDQ_RunTime->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_RunTime->Draw();
  c1->Print("data_quality_figs/hDQ_RunTime.png");
}
