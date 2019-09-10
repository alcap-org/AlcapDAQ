#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TMath.h"
#include "TF1Convolution.h"

#include <iostream>
#include <sstream>

// par[0]: flat bkg
// par[1]: ignore point min
// par[2]: ignore point max
Double_t flat(Double_t* x, Double_t* par) {
  double xx = x[0];
  if (xx > par[1] && xx < par[2]) {
    TF1::RejectPoint();
    return 0;
  }  
  double result = par[0];
  return result;
}

// par[0]: ignore point min
// par[1]: ignore point max
// par[2]: normalisation
// par[3]: lifetime
Double_t lifetime_singleExp(Double_t* x, Double_t* par) {
  double xx = x[0];
  if (xx > par[0] && xx < par[1]) {
    TF1::RejectPoint();
    return 0;
  }  
  double result = par[2]*TMath::Exp(-xx/par[3]);
  return result;
}

// par[0]: ignore point min
// par[1]: ignore point max
// par[2]: normalisation
// par[3]: lifetime
// par[4]: flat bkg
Double_t lifetime_singleExp_flatBkg(Double_t* x, Double_t* par) {
  double xx = x[0];
  if (xx > par[0] && xx < par[1]) {
    TF1::RejectPoint();
    return 0;
  }  
  double result = par[2]*TMath::Exp(-xx/par[3]) + par[4];
  return result;
}

// par[0]: ignore point min
// par[1]: ignore point max
// par[2]: normalisation 1
// par[3]: lifetime 1
// par[4]: normalisation 2
// par[5]: lifetime 2
Double_t lifetime_doubleExp(Double_t* x, Double_t* par) {
  double xx = x[0];
  if (xx > par[0] && xx < par[1]) {
    TF1::RejectPoint();
    return 0;
  }  
  double result = par[2]*TMath::Exp(-xx/par[3]) + par[4]*TMath::Exp(-xx/par[5]);
  return result;
}

// par[0]: ignore point min
// par[1]: ignore point max
// par[2]: normalisation 1
// par[3]: lifetime 1
// par[4]: normalisation 2
// par[5]: lifetime 2
// par[6]: flat bkg
Double_t lifetime_doubleExp_flatBkg(Double_t* x, Double_t* par) {
  double xx = x[0];
  if (xx > par[0] && xx < par[1]) {
    TF1::RejectPoint();
    return 0;
  }  
  double result = par[2]*TMath::Exp(-xx/par[3]) + par[4]*TMath::Exp(-xx/par[5]) + par[6];
  return result;
}

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
  bool include_resolution;
  
  double ignore_region_min;
  double ignore_region_max;
  double ignore_region_flat_min;
  double ignore_region_flat_max;
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

  //  hTime->Sumw2();
  hTime->Rebin(args.rebin_factor);
  //  hTime->Scale(1.0 / hTime->GetXaxis()->GetBinWidth(1));
  std::stringstream axislabel;
  axislabel << "Count / " << hTime->GetXaxis()->GetBinWidth(1) << " ns";
  hTime->SetYTitle(axislabel.str().c_str());
  hTime->SetName("hTime");
  
  TF1* flatbkg;
  if (args.flat_bkg) {
    int npar = 3;
    flatbkg = new TF1("flatbkg", flat, args.min_flat_fit_time, args.max_flat_fit_time, npar);
    flatbkg->SetParameters(1, args.ignore_region_flat_min, args.ignore_region_flat_max);
    flatbkg->FixParameter(1, args.ignore_region_flat_min);
    flatbkg->FixParameter(2, args.ignore_region_flat_max);
    flatbkg->SetParLimits(0, 0, 10000);
    hTime->Fit(flatbkg, "RQL");
  }
  
  TF1* muonic_atom_lifetime;
  int i_lifetime_par = -1;
  if (!args.double_exp && !args.flat_bkg) {
    if (!args.include_resolution) {
      int npar = 4;
      muonic_atom_lifetime = new TF1("muonic_atom_lifetime", lifetime_singleExp, args.min_fit_time, args.max_fit_time, npar);
      muonic_atom_lifetime->SetParameters(args.ignore_region_min, args.ignore_region_max, 1000, 700);
      muonic_atom_lifetime->FixParameter(0, args.ignore_region_min);
      muonic_atom_lifetime->FixParameter(1, args.ignore_region_max);
      i_lifetime_par = 3;
    }
    else {
      int npar = 5;
      TF1Convolution* conv = new TF1Convolution("[0]*TMath::Exp(-x/[1])", "gaus", args.min_fit_time, args.max_fit_time, true);
      conv->SetRange(args.min_fit_time, args.max_fit_time);
      conv->SetNofPointsFFT(1000);
      muonic_atom_lifetime = new TF1("muonic_atom_lifetime", *conv, args.min_fit_time, args.max_fit_time, conv->GetNpar());
      muonic_atom_lifetime->SetParameters(1000, 700, 100, 0, 20);
      muonic_atom_lifetime->FixParameter(3, 0);
      muonic_atom_lifetime->FixParameter(4, 20);
      i_lifetime_par = 1;
    }
  }
  else if (!args.double_exp && args.flat_bkg) {
    int npar = 5;
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", lifetime_singleExp_flatBkg, args.min_fit_time, args.max_fit_time, npar);
    muonic_atom_lifetime->SetParameters(args.ignore_region_min, args.ignore_region_max, 1000, 700, 10);
    muonic_atom_lifetime->FixParameter(0, args.ignore_region_min);
    muonic_atom_lifetime->FixParameter(1, args.ignore_region_max);
    //    muonic_atom_lifetime->FixParameter(4, flatbkg->GetParameter(0));
    muonic_atom_lifetime->SetParameter(4, flatbkg->GetParameter(0));
    muonic_atom_lifetime->SetParLimits(4, 0, 10000);
    i_lifetime_par = 3;
  }
  else if (args.double_exp && !args.flat_bkg) {
    int npar = 6;
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", lifetime_doubleExp, args.min_fit_time, args.max_fit_time, npar);
    muonic_atom_lifetime->SetParameters(args.ignore_region_min, args.ignore_region_max, 1000, 700, 1000, 100);
    muonic_atom_lifetime->FixParameter(0, args.ignore_region_min);
    muonic_atom_lifetime->FixParameter(1, args.ignore_region_max);
    i_lifetime_par = 3;
  }
  else if (args.double_exp && args.flat_bkg) {
    int npar = 7;
    muonic_atom_lifetime = new TF1("muonic_atom_lifetime", lifetime_doubleExp_flatBkg, args.min_fit_time, args.max_fit_time, npar);
    muonic_atom_lifetime->SetParameters(args.ignore_region_min, args.ignore_region_max, 1000, 700, 1000, 100, 10);
    muonic_atom_lifetime->FixParameter(0, args.ignore_region_min);
    muonic_atom_lifetime->FixParameter(1, args.ignore_region_max);
    muonic_atom_lifetime->FixParameter(6, flatbkg->GetParameter(0));
    i_lifetime_par = 3;
  }
  hTime->Fit(muonic_atom_lifetime, "RQL");

  std::cout << args.outdirname << ": tau_1 = " << muonic_atom_lifetime->GetParameter(i_lifetime_par) << " +/- " << muonic_atom_lifetime->GetParError(i_lifetime_par);
  if (args.double_exp) {
    std::cout << "\t tau_2 = " << muonic_atom_lifetime->GetParameter(5) << " +/- " << muonic_atom_lifetime->GetParError(5);
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
