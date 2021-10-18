#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"
#include "TGraphAsymmErrors.h"

#include <iostream>
#include <sstream>

struct Systematic_FinalPlotArgs {
  std::string central_val_filename;
  std::string central_val_histname;

  std::string syst_val_filename;
  std::vector<std::string> syst_val_histnames;
  std::vector<bool> symmetric;

  std::string outfilename;
  std::string outdirname;
};

void Systematic_FinalPlot(const Systematic_FinalPlotArgs& args) {

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
  TH1F* hFinalSystHist = (TH1F*) central_hist->Clone("hFinalSystHist");

  TFile* syst_file = new TFile(args.syst_val_filename.c_str(), "READ");
  if (syst_file->IsZombie()) {
    std::cout << "Problem opening syst file " << args.syst_val_filename << std::endl;
    return;
  }

  std::vector<double> energies;
  std::vector<double> energy_high_errs;
  std::vector<double> energy_low_errs;
  std::vector<double> rates;
  std::vector<double> rate_high_errs;
  std::vector<double> rate_low_errs;
  std::vector<double> rate_high_stat_errs;
  std::vector<double> rate_low_stat_errs;
  std::vector<double> rate_high_syst_errs;
  std::vector<double> rate_low_syst_errs;

  for (int i_bin = 1; i_bin <= central_hist->GetNbinsX(); ++i_bin) {
    energies.push_back(central_hist->GetBinCenter(i_bin));
    energy_high_errs.push_back(central_hist->GetBinWidth(i_bin)/2);
    energy_low_errs.push_back(central_hist->GetBinWidth(i_bin)/2);

    rates.push_back(central_hist->GetBinContent(i_bin));
    rate_high_errs.push_back(central_hist->GetBinError(i_bin)*central_hist->GetBinError(i_bin));
    rate_low_errs.push_back(central_hist->GetBinError(i_bin)*central_hist->GetBinError(i_bin));
    rate_high_stat_errs.push_back(central_hist->GetBinError(i_bin));
    rate_low_stat_errs.push_back(central_hist->GetBinError(i_bin));
    rate_high_syst_errs.push_back(0.0);
    rate_low_syst_errs.push_back(0.0);
  }

  int n_systematics = args.syst_val_histnames.size();
  std::vector<TH1F*> hSysts;
  for (int i_systematic = 0; i_systematic < n_systematics; ++i_systematic) {
    std::string systhistname = args.syst_val_histnames.at(i_systematic) + "/hSystematic";
    std::cout << args.syst_val_histnames.at(i_systematic) << std::endl;
    TH1F* hSyst = (TH1F*) syst_file->Get(systhistname.c_str());
    if (!hSyst) {
      std::cout << "Problem getting systematic histogram " << systhistname << std::endl;
    }
    hSyst->SetName(args.syst_val_histnames.at(i_systematic).c_str());

    for (int i_bin = 1; i_bin <= central_hist->GetNbinsX(); ++i_bin) {
      double& old_low_error2 = rate_low_errs.at(i_bin-1);
      double& old_high_error2 = rate_high_errs.at(i_bin-1);

      double& old_low_syst_error2 = rate_low_syst_errs.at(i_bin-1);
      double& old_high_syst_error2 = rate_high_syst_errs.at(i_bin-1);
      
      double syst_error = hSyst->GetBinContent(i_bin)/100 * central_hist->GetBinContent(i_bin);
      if (args.symmetric.at(i_systematic) == false) {
	if (syst_error > 0) {
	  old_high_error2 = old_high_error2 + syst_error*syst_error;
	  old_high_syst_error2 = old_high_syst_error2 + syst_error*syst_error;
	}
	else {
	  old_low_error2 = old_low_error2 + syst_error*syst_error;
	  old_low_syst_error2 = old_low_syst_error2 + syst_error*syst_error;
	}
      }
      else {
	old_high_error2 = old_high_error2 + syst_error*syst_error;
	old_high_syst_error2 = old_high_syst_error2 + syst_error*syst_error;

	old_low_error2 = old_low_error2 + syst_error*syst_error;
	old_low_syst_error2 = old_low_syst_error2 + syst_error*syst_error;
      }

      // if (rates.at(i_bin-1) > 0) {
      // 	std::cout << "E = " << central_hist->GetBinCenter(i_bin) << std::endl;
      // 	std::cout << "Old Rate = " << rates.at(i_bin-1) << " + " << std::sqrt(old_high_error2) << " - " << std::sqrt(old_low_error2) << std::endl;
      // 	std::cout << "syst error = " << hSyst->GetBinContent(i_bin) << "% = " << syst_error << std::endl;
      // 	std::cout << "New Rate = " << rates.at(i_bin-1) << " + " << std::sqrt(old_high_error2) << " - " << std::sqrt(old_low_error2) << std::endl;
      // }

      // double current_error = hFinalSystHist->GetBinError(i_bin);
      // double new_error = std::sqrt(current_error*current_error + syst_error*syst_error);
      // hFinalSystHist->SetBinError(i_bin, new_error);
    }
    hSysts.push_back(hSyst);
  }

  // Now sqrt everything
  for (int i_bin = 1; i_bin <= central_hist->GetNbinsX(); ++i_bin) {
    rate_high_errs.at(i_bin-1) = std::sqrt(rate_high_errs.at(i_bin-1));
    rate_low_errs.at(i_bin-1) = std::sqrt(rate_low_errs.at(i_bin-1));
    rate_high_syst_errs.at(i_bin-1) = std::sqrt(rate_high_syst_errs.at(i_bin-1));
    rate_low_syst_errs.at(i_bin-1) = std::sqrt(rate_low_syst_errs.at(i_bin-1));
  }

  TGraphAsymmErrors* hFinalSyst = new TGraphAsymmErrors(energies.size(), &energies[0], &rates[0], &energy_low_errs[0], &energy_high_errs[0], &rate_low_syst_errs[0], &rate_high_syst_errs[0]);
  hFinalSyst->SetName("hFinalSyst");
  
  TGraphAsymmErrors* hFinalStatSyst = new TGraphAsymmErrors(energies.size(), &energies[0], &rates[0], &energy_low_errs[0], &energy_high_errs[0], &rate_low_errs[0], &rate_high_errs[0]);
  hFinalStatSyst->SetName("hFinalStatSyst");

  TGraphAsymmErrors* hFinalStat = new TGraphAsymmErrors(energies.size(), &energies[0], &rates[0], &energy_low_errs[0], &energy_high_errs[0], &rate_low_stat_errs[0], &rate_high_stat_errs[0]);
  hFinalStat->SetName("hFinalStat");

  std::cout << "Systematic " << args.outdirname << std::endl;

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  central_hist->Write();
  for (int i_systematic = 0; i_systematic < n_systematics; ++i_systematic) {
    hSysts.at(i_systematic)->Write();
  }
  hFinalStatSyst->Write();
  hFinalSyst->Write();
  hFinalStat->Write();
  hFinalSystHist->Write();
  outfile->Write();
  outfile->Close();
}
