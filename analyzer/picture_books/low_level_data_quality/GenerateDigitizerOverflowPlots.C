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
// -- hDQ_DigitizerOverflow_Fraction.pdf
// -- hDQ_DigitizerOverflow_FractionByEvent.pdf
void GenerateDigitizerOverflowPlots(std::string filename) {

  std::cout << "Generating Digitizer Overflow plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  TFile* file = new TFile(filename.c_str(), "READ");

  // The histograms
  TH1D *hDQ_DigitizerOverflow_Fraction;
  TH2D *hDQ_DigitizerOverflow_FractionByEvent;

  // The names of these histograms in the ROOT file should be the same regardless of the run
  file->GetObject("DataQuality_LowLevel/hDQ_DigitizerOverflow_Fraction",hDQ_DigitizerOverflow_Fraction);
  file->GetObject("DataQuality_LowLevel/hDQ_DigitizerOverflow_FractionByEvent",hDQ_DigitizerOverflow_FractionByEvent);

  hDQ_DigitizerOverflow_Fraction->GetYaxis()->SetTitleOffset(1.3);
  hDQ_DigitizerOverflow_FractionByEvent->GetYaxis()->SetTitleOffset(1.3);

  TCanvas *c1 = new TCanvas();

  hDQ_DigitizerOverflow_Fraction->Draw();
  c1->Print("data_quality_figs/hDQ_DigitizerOverflow_Fraction.pdf");

  hDQ_DigitizerOverflow_FractionByEvent->Draw("COLZ");
  c1->Print("data_quality_figs/hDQ_DigitizerOverflow_FractionByEvent.pdf");

}
