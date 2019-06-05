#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"

#include <iostream>
#include <sstream>

struct LifetimeFitArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outdirname;

  double min_fit_time;
  double max_fit_time;
  double min_flat_fit_time;
  double max_flat_fit_time;
  int rebin_factor;

  bool project_x;
  bool project_y;
  bool double_exp;
  bool flat_bkg;
  double min_energy_cut;
  double max_energy_cut;
};

void LifetimeFit(const LifetimeFitArgs& args) {
  TFile* file = new TFile(args.infilename.c_str(), "READ");
  TH1D* hTime = NULL;
  if (!args.project_x && !args.project_y) {
    hTime = (TH1D*) file->Get(args.inhistname.c_str());
    if (!hTime) {
      std::cout << "Error: Problem getting hTime: " << args.inhistname.c_str() << std::endl;
      return;
    }
  }
  else if (args.project_x && !args.project_y) {
    TH2F* h2D = (TH2F*) file->Get(args.inhistname.c_str());
    if (!h2D) {
      std::cout << "Error: Problem getting h2D: " << args.inhistname.c_str() << std::endl;
      return;
    }
    std::string projname = args.inhistname + "_px";
    int min_cut_bin = 1;
    int max_cut_bin = h2D->GetYaxis()->GetNbins();
    if (args.min_energy_cut > 0) {
      min_cut_bin = h2D->GetYaxis()->FindBin(args.min_energy_cut);
    }
    if (args.max_energy_cut > 0) {
      max_cut_bin = h2D->GetYaxis()->FindBin(args.max_energy_cut);
    }
    hTime = h2D->ProjectionX(projname.c_str(), min_cut_bin, max_cut_bin);
  }
  else if (!args.project_x && args.project_y) {
    TH2F* h2D = (TH2F*) file->Get(args.inhistname.c_str());
    if (!h2D) {
      std::cout << "Error: Problem getting h2D: " << args.inhistname.c_str() << std::endl;
      return;
    }
    std::string projname = args.inhistname + "_px";
    hTime = h2D->ProjectionY(projname.c_str(), 1, h2D->GetXaxis()->GetNbins());
  }
  else if (args.project_x && args.project_y) {
    std::cout << "ERROR: can't project onto both x and y" << std::endl;
    return;
  }
  
  hTime->Rebin(args.rebin_factor);
  std::stringstream axislabel;
  axislabel << "Count / " << hTime->GetXaxis()->GetBinWidth(1) << " ns";
  hTime->SetYTitle(axislabel.str().c_str());
  //  hTime->Scale(1.0 / hTime->GetXaxis()->GetBinWidth(1));
  hTime->SetName("hTime");
  
  TF1* flatbkg;
  if (args.flat_bkg) {
    flatbkg = new TF1("flatbkg", "[0]", args.min_flat_fit_time, args.max_flat_fit_time);
    hTime->Fit(flatbkg, "RQ");
  }
  
  TF1* muonic_atom_lifetime;
  if (!args.double_exp && !args.flat_bkg) {
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", "[0]*TMath::Exp(-x/[1])", args.min_fit_time, args.max_fit_time);
    muonic_atom_lifetime->SetParameters(1000, 700);
  }
  else if (!args.double_exp && args.flat_bkg) {
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", "[0]*TMath::Exp(-x/[1]) + [2]", args.min_fit_time, args.max_fit_time);
    muonic_atom_lifetime->SetParameters(1000, 700, 10);
    muonic_atom_lifetime->FixParameter(2, flatbkg->GetParameter(0));
  }
  else if (args.double_exp && !args.flat_bkg) {
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3])", args.min_fit_time, args.max_fit_time);
    muonic_atom_lifetime->SetParameters(1000, 700, 1000, 100);
  }
  else if (args.double_exp && args.flat_bkg) {
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]) + [4]", args.min_fit_time, args.max_fit_time);
    muonic_atom_lifetime->SetParameters(1000, 700, 1000, 100, 10);
    muonic_atom_lifetime->FixParameter(4, flatbkg->GetParameter(0));
  }
  hTime->Fit(muonic_atom_lifetime, "RQ");

  std::cout << args.outdirname << ": tau_1 = " << muonic_atom_lifetime->GetParameter(1) << " +/- " << muonic_atom_lifetime->GetParError(1);
  if (args.double_exp) {
    std::cout << "\t tau_2 = " << muonic_atom_lifetime->GetParameter(3) << " +/- " << muonic_atom_lifetime->GetParError(3);
  }
  std::cout << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hTime->Write();
  muonic_atom_lifetime->Write();
  if (args.flat_bkg && flatbkg) {
    flatbkg->Write();
  }
  outfile->Write();
  outfile->Close();

};
