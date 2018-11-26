#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"

#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"

#include "scripts/MuonBeamSimulation/RooGaussExp.h"

#include <iostream>
#include <sstream>

extern TROOT* gROOT;

struct FlatBkgFitArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outdirname;

  double edep_min;
  double edep_max;

  double landau_mean_min;
  double landau_mean_max;
  double landau_sigma_min;
  double landau_sigma_max;
  double resolution_sigma_min;
  double resolution_sigma_max;

  /*  double stop_edep_min;
  double stop_edep_max;
  double stop_sigma_min;
  double stop_sigma_max;
  double stop_alpha_min;
  double stop_alpha_max;

  double n_punch_min;
  double n_punch_max;
  double n_stop_min;
  double n_stop_max;
  */
};

void FlatBkgFit(const FlatBkgFitArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1D* hSpectrum = (TH1D*) file->Get(args.inhistname.c_str());
  if (!hSpectrum) {
    std::cout << "Error: Problem getting hSpectrum: " << args.inhistname.c_str() << std::endl;
    return;
  }
  
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  
  std::stringstream axislabel;
  axislabel << "Count / " << hSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
  hSpectrum->SetYTitle(axislabel.str().c_str());

  RooWorkspace* ws = new RooWorkspace("ws", "ws");
  
  std::string xVar = "edep";
  
  std::stringstream cmd;
  cmd.str("");
  cmd << xVar << "[" << args.edep_min << ", " << args.edep_max << "]";
  ws->factory(cmd.str().c_str());
  
  RooDataHist* rooHist = new RooDataHist("data", "", (ws->argSet(xVar.c_str())), hSpectrum);
  ws->import(*rooHist);  

  std::string fullPdfName = "full_pdf";

  std::string resolutionMeanName = "resolution_mean";
  std::string resolutionSigmaName = "resolution_sigma";
  std::string resolutionName = "resolution_pdf";
  
  std::string landauMeanName = "landau_mean";
  std::string landauSigmaName = "landau_sigma";
  std::string landauName = "landau_pdf";

  // Construct the resolution pdf (gaussian)
  cmd.str("");
  cmd << "Gaussian::" << resolutionName << "(" << xVar << ", " << resolutionMeanName << "[0], " << resolutionSigmaName << "[" << args.resolution_sigma_min << "," << args.resolution_sigma_max << "])";
  //    cmd << "Gaussian::" << resolutionName << "(" << xVar << ", " << resolutionMeanName << "[0], " << resolutionSigmaName << "[30])";
  ws->factory(cmd.str().c_str());

  // Construct the landau pdf landau
  cmd.str("");
  cmd << "Landau::" << landauName << "(" << xVar << ", " << landauMeanName << "[" << args.landau_mean_min << "," << args.landau_mean_max << "], " << landauSigmaName << "[" << args.landau_sigma_min << "," << args.landau_sigma_max << "])";
  ws->factory(cmd.str().c_str());

  // Now convolve the two
  cmd.str("");
  cmd << "FCONV::" << fullPdfName << "(" << xVar << "," << landauName << ", " << resolutionName << ")";
  ws->factory(cmd.str().c_str());

  
  ws->pdf(fullPdfName.c_str())->fitTo(*rooHist);
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  if (!outdir) {
    std::cout << "Problem openning output directory " << args.outdirname << std::endl;
    return;
  }
  outdir->cd();
  hSpectrum->Write();
  ws->Write();
  outfile->Write();
  outfile->Close();
};
