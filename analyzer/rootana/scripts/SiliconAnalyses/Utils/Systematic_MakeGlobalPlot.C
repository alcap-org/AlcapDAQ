#ifndef Systematic_MakeGlobalPlot_
#define Systematic_MakeGlobalPlot_
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Systematic_MakeGlobalPlotArgs {
  std::string central_val_filename; // just for the histogram dimensions
  std::string central_val_histname;

  double overall_systematic; // as a percentage
  
  std::string outfilename;
  std::string outdirname;
};

void Systematic_MakeGlobalPlot(const Systematic_MakeGlobalPlotArgs& args) {

  if (args.overall_systematic < 0 || args.overall_systematic > 100) {
    std::cout << "Systematic_MakeGlobalPlot: Overall systematic must be a percentage" << std::endl;
    return;
  }
  
  TFile* central_file = new TFile(args.central_val_filename.c_str(), "READ");
  if (central_file->IsZombie()) {
    std::cout << "Problem opening central file " << args.central_val_filename << std::endl;
    return;
  }
  TH1F* central_hist = (TH1F*) central_file->Get(args.central_val_histname.c_str());
  if (!central_hist) {
    std::cout << "Problem getting histogram " << args.central_val_histname << std::endl;
    return;
  }

  TH1D* hSystematic = (TH1D*) central_hist->Clone("hSystematic");
  for (int i_bin = 1; i_bin <= hSystematic->GetNbinsX(); ++i_bin) {
    hSystematic->SetBinContent(i_bin, args.overall_systematic);
  }
  hSystematic->SetYTitle("Systematic Error [%]");
  
  std::cout << "Systematic " << args.outdirname << std::endl;

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hSystematic->Write();
  outfile->Write();
  outfile->Close();
}
#endif
