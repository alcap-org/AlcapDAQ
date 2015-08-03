#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

void GenerateMuPCPlots(std::string filename) {

  std::cout << "Generating MuPC plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH2F *hDQ_MuPC_XvsY_Total;
  TH2F *hDQ_MuPC_XvsY_ExclHotWires;
  TH2F *hDQ_MuPC_XvsT;
  TH2F *hDQ_MuPC_YvsT;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_MuPC_XvsY_Total",hDQ_MuPC_XvsY_Total);
  file->GetObject("DataQuality_LowLevel/hDQ_MuPC_XvsY_ExclHotWires",hDQ_MuPC_XvsY_ExclHotWires);
  file->GetObject("DataQuality_LowLevel/hDQ_MuPC_XvsT",hDQ_MuPC_XvsT);
  file->GetObject("DataQuality_LowLevel/hDQ_MuPC_YvsT",hDQ_MuPC_YvsT);

//   hDQ_MuPC_XvsY_Total->GetYaxis()->SetTitleOffset(1.3);
//   hDQ_MuPC_XvsT->GetYaxis()->SetTitleOffset(1.3);
//   hDQ_MuPC_YvsT->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_MuPC_XvsY_Total->Draw("colz");
  c1->Print("data_quality_figs/hDQ_MuPC_XvsY_Total.png");

  hDQ_MuPC_XvsY_ExclHotWires->Draw("colz");
  c1->Print("data_quality_figs/hDQ_MuPC_XvsY_ExclHotWires.png");

  hDQ_MuPC_XvsT->Draw("colz");
  c1->Print("data_quality_figs/hDQ_MuPC_XvsT.png");

  hDQ_MuPC_YvsT->Draw("colz");
  c1->Print("data_quality_figs/hDQ_MuPC_YvsT.png");

}
