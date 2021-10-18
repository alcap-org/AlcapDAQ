#ifndef Systematic_MakePlot_
#define Systematic_MakePlot_

#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Systematic_MakePlotArgs {
  std::string central_val_filename;
  std::string central_val_histname;

  std::string syst_val_filename;
  std::string syst_val_histname;

  int rebin_factor;
  int central_rebin_factor;
  
  std::string outfilename;
  std::string outdirname;
};

void Systematic_MakePlot(const Systematic_MakePlotArgs& args) {

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
  central_hist->SetName("central_hist");

  TFile* syst_file = new TFile(args.syst_val_filename.c_str(), "READ");
  if (syst_file->IsZombie()) {
    std::cout << "Problem opening syst file " << args.syst_val_filename << std::endl;
    return;
  }
  TH1F* syst_hist = (TH1F*) syst_file->Get(args.syst_val_histname.c_str());
  if (!syst_hist) {
    std::cout << "Problem getting histogram " << args.syst_val_histname << std::endl;
    return;
  }
  syst_hist->SetName("syst_hist");

  central_hist->Rebin(args.central_rebin_factor);
  central_hist->Scale(1.0 / args.central_rebin_factor);
  syst_hist->Rebin(args.rebin_factor);
  syst_hist->Scale(1.0 / args.rebin_factor);

  TH1D* hSystematic = (TH1D*) syst_hist->Clone("hSystematic");
  hSystematic->Add(central_hist, -1);
  hSystematic->Divide(central_hist);
  hSystematic->Scale(100);
  hSystematic->SetYTitle("Systematic Error [%]");
  
  std::cout << "Systematic " << args.outdirname << std::endl;
  std::cout << "Central Val: File = " << args.central_val_filename << ", Hist = " << args.central_val_histname << std::endl;
  std::cout << "Syst Val: File = " << args.syst_val_filename << ", Hist = " << args.syst_val_histname << std::endl;

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  central_hist->Write();
  syst_hist->Write();
  hSystematic->Write();
  outfile->Write();
  outfile->Close();

  central_file->Close();
  syst_file->Close();
}

#endif
