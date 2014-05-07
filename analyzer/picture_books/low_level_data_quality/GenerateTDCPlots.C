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
// -- hDQ_TDCCheck_muSc_rate.png
// -- hDQ_TDCCheck_Unknown.png
// -- hDQ_TDCCheck_TDiff.png
void GenerateTDCPlots(std::string filename) {

  std::cout << "Generating TDC plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1F *hDQ_TDCCheck_muSc_rate;
  TH1F *hDQ_TDCCheck_Unknown;
  TH1F *hDQ_TDCCheck_TDiff;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_TDCCheck_muSc_rate",hDQ_TDCCheck_muSc_rate);
  file->GetObject("DataQuality_LowLevel/hDQ_TDCCheck_Unknown",hDQ_TDCCheck_Unknown);
  file->GetObject("DataQuality_LowLevel/hDQ_TDCCheck_TDiff",hDQ_TDCCheck_TDiff);

  hDQ_TDCCheck_muSc_rate->GetYaxis()->SetTitleOffset(1.3);
  hDQ_TDCCheck_Unknown->GetYaxis()->SetTitleOffset(1.3);
  hDQ_TDCCheck_TDiff->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_TDCCheck_muSc_rate->Draw();
  c1->Print("data_quality_figs/hDQ_TDCCheck_muSc_rate.png");

  hDQ_TDCCheck_Unknown->Draw();
  c1->Print("data_quality_figs/hDQ_TDCCheck_Unknown.png");

  hDQ_TDCCheck_TDiff->Draw();
  c1->Print("data_quality_figs/hDQ_TDCCheck_TDiff.png");
}
