#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

void ZoomAxis(TH1* hist);

// This will run all the plot generation macros
void GenerateAllPlots(const char* data_dir, const int n_run) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  std::stringstream filename;
  filename << data_dir << "/hist/hist0" << n_run << ".root";
  std::cout << "Filename: " << filename.str() << std::endl;

  std::string command = "GenerateFADCPlots(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());

  command = "GenerateDigitizerOverflowPlots(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());

  command = "GenerateRunTimePlot(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());

  command = "GenerateTDCPlots(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());

  command = "GenerateTPIPlots(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());

  command = "GeneratePulseShapesPlots(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());

  command = "GenerateTDiffPlots(\"" + filename.str() + "\");";
  gROOT->ProcessLine(command.c_str());
}

// void ZoomAxis(TH1* hist)
// -- Finds the range that we want on the x-axis for the individual plot
void ZoomAxis(TH1* hist) {

  int max_bin = 0;

  for (int iBin = hist->GetNbinsX(); iBin > 0; --iBin) {
	  
    if (hist->GetBinContent(iBin) >= 1) {
	      
      // See if this bin is higher than the previous
      if (iBin > max_bin) {
	max_bin = iBin;
	break;
      }
    }
  }
  
  hist->GetXaxis()->SetRange(1, max_bin); // set the range based on bin number
}
