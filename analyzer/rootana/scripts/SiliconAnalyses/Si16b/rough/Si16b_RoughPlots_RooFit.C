#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"

#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include <iostream>
#include <sstream>

void Si16b_RoughPlots_RooFit() {


  RooWorkspace* ws = new RooWorkspace("ws", "ws");
  
  std::string xVar = "edep";
  
  std::stringstream cmd;
  cmd.str("");
  cmd << xVar << "[300 1500]";
  ws->factory(cmd.str().c_str());

  std::string filename = "~/data/results/Si16b/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  std::string treename = "siBlockTree_SiR";
  TTree* SiRTree = (TTree*) file->Get(treename.c_str());

  double min_energy = 0;
  double max_energy = 1500;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH1F* hSpectrum = new TH1F("hSpectrum", "", n_energy_bins,min_energy,max_energy);

  SiRTree->Draw("third_energy>>hSpectrum", "thin_tpi_id<0 && thick_tpi_id>=0 && third_tpi_id>=0", "goff");
  RooDataHist* rooHist = new RooDataHist("data", "", (ws->argSet(xVar.c_str())), hSpectrum);
  ws->import(*rooHist);  


  std::string resolutionName = "res";
  std::string resolutionMeanName = "gaus_mean";
  std::string resolutionSigmaName = "gaus_sigma";
  // Construct the resolution pdf (gaussian)
  cmd.str("");
  cmd << "Gaussian::" << resolutionName << "(" << xVar << ", " << resolutionMeanName << "[0], " << resolutionSigmaName << "[62, 0, 1000])";
  ws->factory(cmd.str().c_str());

  std::string landauName = "landau";
  std::string landauMpvName = "landau_mpv";
  std::string landauSigmaName = "landau_sigma";
  // Construct the landau pdf
  cmd.str("");
  cmd << "Landau::" << landauName << "(" << xVar << ", " << landauMpvName << "[392, 300, 450], " << landauSigmaName << "[31, 25, 1000])";
  ws->factory(cmd.str().c_str());

  std::string convName = "conv";
  // Now convolve the two
  cmd.str("");
  cmd << "FCONV::" << convName << "(" << xVar << "," << landauName << ", " << resolutionName << ")";
  ws->factory(cmd.str().c_str());

  ws->pdf(convName.c_str())->fitTo(*rooHist, RooFit::Range(300,1500));
    
  RooPlot* Eframe = (ws->var("edep"))->frame();
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf(convName.c_str()))->plotOn(Eframe);
  Eframe->Draw();
};
