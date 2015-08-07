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
// -- hDQ_TDCCheck_nMuons.png
// -- hDQ_TDCCheck_Muon_rate.png
// -- hDQ_TDCCheck_Muon_time.png
void GenerateTDCPlots(std::string filename) {

  std::cout << "Generating TDC plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1F *hDQ_TDCCheck_nMuons;
  TH1F *hDQ_TDCCheck_Muon_rate;
  TH1F *hDQ_TDCCheck_Muon_time;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_TDCCheck_Muon_rate",hDQ_TDCCheck_Muon_rate);
  file->GetObject("DataQuality_LowLevel/hDQ_TDCCheck_nMuons",hDQ_TDCCheck_nMuons);
  file->GetObject("DataQuality_LowLevel/hDQ_TDCCheck_Muon_time",hDQ_TDCCheck_Muon_time);

  hDQ_TDCCheck_Muon_rate->GetYaxis()->SetTitleOffset(1.3);
  hDQ_TDCCheck_nMuons->GetYaxis()->SetTitleOffset(1.3);
  hDQ_TDCCheck_Muon_time->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_TDCCheck_Muon_rate->Draw();
  c1->Print("data_quality_figs/hDQ_TDCCheck_Muon_rate.png");

  hDQ_TDCCheck_nMuons->Draw();
  c1->Print("data_quality_figs/hDQ_TDCCheck_nMuons.png");

  hDQ_TDCCheck_Muon_time->Draw();
  c1->Print("data_quality_figs/hDQ_TDCCheck_Muon_time.png");
}
