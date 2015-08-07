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
// -- hDQ_DigitizerOverflow_Fraction.png
// -- hDQ_DigitizerOverflow_FractionByEvent.png
// -- hDT5720Errors
// -- hDT5730Errors
void GenerateDigitizerPlots(std::string filename) {

  std::cout << "Generating Digitizer Overflow plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1D *hDQ_DigitizerOverflow_Fraction;
  TH2D *hDQ_DigitizerOverflow_FractionByEvent;
  TH1I *hDT5720Errors;
  TH1I *hDT5730Errors;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_DigitizerOverflow_Fraction",hDQ_DigitizerOverflow_Fraction);
  file->GetObject("DataQuality_LowLevel/hDQ_DigitizerOverflow_FractionByEvent",hDQ_DigitizerOverflow_FractionByEvent);
  file->GetObject("DataQuality_LowLevel/DT5720Errors/hDT5720Errors",hDT5720Errors);
  file->GetObject("DataQuality_LowLevel/DT5730Errors/hDT5730Errors",hDT5730Errors);

  hDQ_DigitizerOverflow_Fraction->GetYaxis()->SetTitleOffset(1.3);
  hDQ_DigitizerOverflow_FractionByEvent->GetYaxis()->SetTitleOffset(1.3);
  hDT5720Errors->GetYaxis()->SetTitleOffset(1.3);
  hDT5730Errors->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_DigitizerOverflow_Fraction->Draw();
  c1->Print("data_quality_figs/hDQ_DigitizerOverflow_Fraction.png");

  hDQ_DigitizerOverflow_FractionByEvent->SetStats(false);
  hDQ_DigitizerOverflow_FractionByEvent->Draw("COLZ");
  c1->Print("data_quality_figs/hDQ_DigitizerOverflow_FractionByEvent.png");

  hDT5720Errors->Draw();
  c1->Print("data_quality_figs/hDQ_Digitizer_DT5720Errors.png");

  hDT5730Errors->Draw();
  c1->Print("data_quality_figs/hDQ_Digitizer_DT5730Errors.png");

}
