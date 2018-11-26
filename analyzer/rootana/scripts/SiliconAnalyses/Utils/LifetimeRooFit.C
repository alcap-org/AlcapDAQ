#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"

#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include "scripts/MuonBeamSimulation/RooGaussExp.h"

#include <iostream>
#include <sstream>

extern TROOT* gROOT;

struct LifetimeRooFitArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outdirname;

  double time_min;
  double time_max;

  double landau_mean_min;
  double landau_mean_max;
  double landau_sigma_min;
  double landau_sigma_max;
  double resolution_sigma_min;
  double resolution_sigma_max;

  double lifetime_min;
  double lifetime_max;
  
  int rebin_factor;
};

void LifetimeRooFit(const LifetimeRooFitArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH2F* hEnergyTime = (TH2F*) file->Get(args.inhistname.c_str());
  if (!hEnergyTime) {
    std::cout << "Error: Problem getting hEnergyTime: " << args.inhistname.c_str() << std::endl;
    return;
  }


  double min_full_energy = 3000;
  double max_full_energy = 5000;
  int min_full_energy_bin = hEnergyTime->GetYaxis()->FindBin(min_full_energy);
  int max_full_energy_bin = hEnergyTime->GetYaxis()->FindBin(max_full_energy)-1;
  TH1D* hSpectrum = hEnergyTime->ProjectionX("hSpectrum", min_full_energy_bin, max_full_energy_bin);
  hSpectrum->Rebin(args.rebin_factor);
  
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  
  std::stringstream axislabel;
  axislabel << "Count / " << hSpectrum->GetXaxis()->GetBinWidth(1) << " ns";
  hSpectrum->SetYTitle(axislabel.str().c_str());

  RooWorkspace* ws = new RooWorkspace("ws", "ws");
  
  std::string xVar = "time";
  
  std::stringstream cmd;
  cmd.str("");
  cmd << xVar << "[" << args.time_min << ", " << args.time_max << "]";
  ws->factory(cmd.str().c_str());
  
  RooDataHist* rooHist = new RooDataHist("data", "", (ws->argSet(xVar.c_str())), hSpectrum);
  ws->import(*rooHist);  

  std::string fullPdfName = "full_pdf";

  std::string beamPdfName = "beam_pdf";

  std::string resolutionMeanName = "resolution_mean";
  std::string resolutionSigmaName = "resolution_sigma";
  std::string resolutionName = "resolution_pdf";
  
  std::string landauMeanName = "landau_mean";
  std::string landauSigmaName = "landau_sigma";
  std::string landauName = "landau_pdf";
  
  // Conrtruct and fit the flat bkg
  std::string flatPdfName = "flat_bkg_pdf";
  cmd.str("");
  cmd << "Polynomial::" << flatPdfName << "(" << xVar << ")";
  ws->factory(cmd.str().c_str());
  ws->var(xVar.c_str())->setRange("flat_range", 10000,20000);

  // Construct the resolution pdf (gaussian)
  cmd.str("");
  cmd << "Gaussian::" << resolutionName << "(" << xVar << ", " << resolutionMeanName << "[0], " << resolutionSigmaName << "[" << args.resolution_sigma_min << "," << args.resolution_sigma_max << "])";
  ws->factory(cmd.str().c_str());
  // Construct the landau pdf
  cmd.str("");
  cmd << "Landau::" << beamPdfName << "(" << xVar << ", " << landauMeanName << "[" << args.landau_mean_min << "," << args.landau_mean_max << "], " << landauSigmaName << "[" << args.landau_sigma_min << "," << args.landau_sigma_max << "])";
  ws->factory(cmd.str().c_str());
  // Now convolve the two
  //  cmd.str("");
  //  cmd << "FCONV::" << beamPdfName << "(" << xVar << "," << landauName << ", " << resolutionName << ")";
  //  ws->factory(cmd.str().c_str());

  // Now sum the background
  std::string bkgPdfName = "full_bkg_pdf";
  cmd.str("");
  cmd << "SUM::" << bkgPdfName << "(nBeam[0,100000000]*" << beamPdfName << ", nFlat[0,1000000]*" << flatPdfName << ")";
  ws->factory(cmd.str().c_str());
  ws->var(xVar.c_str())->setRange("beam_range", -1000,1000);
  ws->pdf(bkgPdfName.c_str())->fitTo(*rooHist, RooFit::Range("flat_range, beam_range"));
  /*
  // Now add the exponential
  std::string expoName = "expo_pdf";
  std::string lifetimePdfName = "lifetime_pdf";
  std::string lifetimeName = "lifetime";
  std::string resolutionModelName = "resolution_model";
  std::string lifetimeShiftName = "lifetime_shift";
  cmd.str("");
  cmd << "GaussModel::" << resolutionModelName << "(" << xVar << ", " << lifetimeShiftName << "[0, 5000], " << resolutionSigmaName << ")";
  ws->factory(cmd.str().c_str());
  cmd.str("");
  cmd << "Decay::" << lifetimePdfName << "(" << xVar << ", " << lifetimeName << "[" << args.lifetime_min << ", " << args.lifetime_max << "], " << resolutionModelName << ", SingleSided)";
  //  cmd << "Exponential::" << lifetimePdfName << "(" << xVar << ", " << lifetimeName << "[" << -1.0/args.lifetime_min << ", " << -1.0/args.lifetime_max << "])";
  ws->factory(cmd.str().c_str());

  // Now convolve the two
  //  cmd.str("");
  //  cmd << "FCONV::" << lifetimePdfName << "(" << xVar << "," << expoName << ", " << resolutionName << ")";
  //  ws->factory(cmd.str().c_str());

  
  // Now sum them all
  cmd.str("");
  cmd << "SUM::" << fullPdfName << "(nMuon[0,10000000]*" << lifetimePdfName << ", " << bkgPdfName << ")";
  ws->factory(cmd.str().c_str());
  
  ws->var(xVar.c_str())->setRange("lifetime_range", 800,4000);
  ws->pdf(lifetimePdfName.c_str())->fitTo(*rooHist, RooFit::Range("lifetime_range"));
  ws->pdf(fullPdfName.c_str())->fitTo(*rooHist, RooFit::Constrained());
  */
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  if (!outdir) {
    std::cout << "Problem openning output directory " << args.outdirname << std::endl;
    return;
  }
  outdir->cd();
  hEnergyTime->Write();
  ws->Write();
  outfile->Write();
  outfile->Close();
};
