#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

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
}
