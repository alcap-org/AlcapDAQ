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

struct PunchthroughAndStopFitArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outdirname;

  int rebin_factor;

  double edep_min;
  double edep_max;

  bool add_punchthrough;
  bool add_stop;
  
  double landau_mean_min;
  double landau_mean_max;
  double landau_sigma_min;
  double landau_sigma_max;
  double resolution_sigma_min;
  double resolution_sigma_max;
  double stop_edep_min;
  double stop_edep_max;
  double stop_sigma_min;
  double stop_sigma_max;
  double stop_alpha_min;
  double stop_alpha_max;

  double n_punch_min;
  double n_punch_max;
  double n_stop_min;
  double n_stop_max;
};

void PunchthroughAndStopFit(const PunchthroughAndStopFitArgs& args) {

  if (!args.add_stop && !args.add_punchthrough) {
    std::cout << "Need to have either stop or punchthrough added" << std::endl;
    return;
  }
  
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
  
  hSpectrum->Rebin(args.rebin_factor);
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

  std::string nPunchName = "n_punch";
  std::string nStopName = "n_stop";
  std::string fullPdfName = "full_pdf";

  std::string resolutionMeanName = "resolution_mean";
  std::string resolutionSigmaName = "resolution_sigma";
  std::string resolutionName = "resolution_pdf";

  
  std::string punchLandauMeanName = "landau_punch_mean";
  std::string punchLandauSigmaName = "landau_punch_sigma";
  std::string punchLandauName = "landau_punch_pdf";
    
  std::string fullPunchName;
  if (args.add_stop) {
    fullPunchName = "punch_pdf";
  }
  else {
    fullPunchName = fullPdfName;
  }

  std::string stopGaussExpEdepName = "stop_edep";
  std::string stopGaussExpSigmaName = "stop_sigma";
  std::string stopGaussExpAlphaName = "stop_alpha";
  std::string stopGaussExpName = "stop_gaussexp_pdf";
  std::string fullStopName;
  if (args.add_punchthrough) {
    fullStopName = "stop_pdf";
  }
  else {
    fullStopName = fullPdfName; //  in case we don't want to fit a pucnh-through peak
  }

  // Construct the punch-through pdf first (landau X gauss)
  // The gauss is also used for the stop pdf
  cmd.str("");
  cmd << "Gaussian::" << resolutionName << "(" << xVar << ", " << resolutionMeanName << "[0], " << resolutionSigmaName << "[" << args.resolution_sigma_min << "," << args.resolution_sigma_max << "])";
  //    cmd << "Gaussian::" << resolutionName << "(" << xVar << ", " << resolutionMeanName << "[0], " << resolutionSigmaName << "[30])";
  ws->factory(cmd.str().c_str());

  if (args.add_punchthrough) {
    cmd.str("");
    cmd << "Landau::" << punchLandauName << "(" << xVar << ", " << punchLandauMeanName << "[" << args.landau_mean_min << "," << args.landau_mean_max << "], " << punchLandauSigmaName << "[" << args.landau_sigma_min << "," << args.landau_sigma_max << "])";
    ws->factory(cmd.str().c_str());
      
    cmd.str("");
    cmd << "FCONV::" << fullPunchName << "(" << xVar << "," << punchLandauName << ", " << resolutionName << ")";
    ws->factory(cmd.str().c_str());
  }
  
  // Now construct the stop pdf (GaussExp)
  if (args.add_stop) {
    cmd.str("");
    cmd << "GaussExp::" << stopGaussExpName << "(" << xVar << ", " << stopGaussExpEdepName << "[" << args.stop_edep_min << ", " << args.stop_edep_max << "], " << stopGaussExpSigmaName << "[" << args.stop_sigma_min << ", " << args.stop_sigma_max << "], " << stopGaussExpAlphaName << "[" << args.stop_alpha_min << ", " << args.stop_alpha_max << "])";
    ws->factory(cmd.str().c_str());
    //  cmd << "Gaussian::" << stopGaussExpName << "(" << xVar << ", " << stopGaussExpEdepName << "[" << args.stop_edep_min << ", " << args.stop_edep_max << "], " << stopGaussExpSigmaName << "[" << args.stop_sigma_min << ", " << args.stop_sigma_max << "])";
    //  ws->factory(cmd.str().c_str());
    //  cmd << "GaussExp::" << fullStopName << "(" << xVar << ", " << stopGaussExpEdepName << "[" << args.stop_edep_min << ", " << args.stop_edep_max << "], " << resolutionSigmaName << "[" << args.resolution_sigma_min << ", " << args.resolution_sigma_max << "], " << stopGaussExpAlphaName << "[" << args.stop_alpha_min << ", " << args.stop_alpha_max << "])";
    //  ws->factory(cmd.str().c_str());
    
    cmd.str("");
    cmd << "FCONV::" << fullStopName << "(" << xVar << "," << stopGaussExpName << ", " << resolutionName << ")";
    ws->factory(cmd.str().c_str());
  }
  
  // Now sum them...
  if (args.add_punchthrough && args.add_stop) {
    cmd.str("");
    cmd << "SUM::" << fullPdfName << "(" << nPunchName << "[" << args.n_punch_min << ", " << args.n_punch_max << "]*" << fullPunchName << ", " << nStopName << "[" << args.n_stop_min << ", " << args.n_stop_max << "]*" << fullStopName << ")";
    ws->factory(cmd.str().c_str());
  }
  
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
