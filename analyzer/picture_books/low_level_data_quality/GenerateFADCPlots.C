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
// -- hDQ_FADCPacketLoss_Fraction.pdf
// -- hDQ_FADCPacketLoss_TotalByEvent.pdf
// -- hDQ_FADCBufferOverflow_Fraction.pdf
// -- hDQ_FADCBufferOverflow_TotalByEvent.pdf
void GenerateFADCPlots(std::string filename) {

  std::cout << "Generating FADC plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1F *hDQ_FADCPacketLoss_Fraction;
  TH2F *hDQ_FADCPacketLoss_TotalByEvent;
  TH1F *hDQ_FADCBufferOverflow_Fraction;
  TH2F *hDQ_FADCBufferOverflow_TotalByEvent;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_FADCPacketLoss_Fraction",hDQ_FADCPacketLoss_Fraction);
  file->GetObject("DataQuality_LowLevel/hDQ_FADCPacketLoss_TotalByEvent",hDQ_FADCPacketLoss_TotalByEvent);
  file->GetObject("DataQuality_LowLevel/hDQ_FADCBufferOverflow_Fraction",hDQ_FADCBufferOverflow_Fraction);
  file->GetObject("DataQuality_LowLevel/hDQ_FADCBufferOverflow_TotalByEvent",hDQ_FADCBufferOverflow_TotalByEvent);

  hDQ_FADCPacketLoss_Fraction->GetYaxis()->SetTitleOffset(1.3);
  hDQ_FADCPacketLoss_TotalByEvent->GetYaxis()->SetTitleOffset(1.3);
  hDQ_FADCBufferOverflow_Fraction->GetYaxis()->SetTitleOffset(1.3);
  hDQ_FADCBufferOverflow_TotalByEvent->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_FADCPacketLoss_Fraction->Draw();
  c1->Print("data_quality_figs/hDQ_FADCPacketLoss_Fraction.pdf");

  hDQ_FADCPacketLoss_TotalByEvent->Draw("COLZ");
  c1->Print("data_quality_figs/hDQ_FADCPacketLoss_TotalByEvent.pdf");

  hDQ_FADCBufferOverflow_Fraction->Draw();
  c1->Print("data_quality_figs/hDQ_FADCBufferOverflow_Fraction.pdf");

  hDQ_FADCBufferOverflow_TotalByEvent->Draw("COLZ");
  c1->Print("data_quality_figs/hDQ_FADCBufferOverflow_TotalByEvent.pdf");
}
