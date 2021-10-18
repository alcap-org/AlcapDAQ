#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"
#include "TGraphAsymmErrors.h"

#include <iostream>
#include <sstream>

struct Systematic_SumPlotArgs {
  std::string gre_filename;
  std::vector<std::string> gre_names;

  std::string outfilename;
  std::string outdirname;
};

void Systematic_SumPlot(const Systematic_SumPlotArgs& args) {

  TFile* gre_file = new TFile(args.gre_filename.c_str(), "READ");
  if (gre_file->IsZombie()) {
    std::cout << "Problem opening syst file " << args.gre_filename << std::endl;
    return;
  }

  std::vector<double> sum_energies;
  std::vector<double> sum_energy_high_errs;
  std::vector<double> sum_energy_low_errs;
  std::vector<double> sum_rates;
  std::vector<double> sum_rate_high_errs;
  std::vector<double> sum_rate_low_errs;
  //  std::vector<double> sum_rate_high_stat_errs;
  //  std::vector<double> sum_rate_low_stat_errs;
  //  std::vector<double> sum_rate_high_syst_errs;
  //  std::vector<double> sum_rate_low_syst_errs;

  int n_components = args.gre_names.size();
  for (int i_component = 0; i_component < n_components; ++i_component) {
    std::string systhistname = args.gre_names.at(i_component) + "/hFinalStatSyst";
    std::cout << args.gre_names.at(i_component) << std::endl;

    TGraphAsymmErrors* gre = (TGraphAsymmErrors*) gre_file->Get(systhistname.c_str());

    for (int i_element = 0; i_element < gre->GetN(); ++i_element) {
      double this_energy = 0; double this_rate = 0;
      gre->GetPoint(i_element, this_energy, this_rate);

      double this_low_energy_err = gre->GetErrorXlow(i_element);
      double this_high_energy_err = gre->GetErrorXhigh(i_element);
      double this_low_rate_err = gre->GetErrorYlow(i_element);
      double this_high_rate_err = gre->GetErrorYhigh(i_element);

      if (i_component == 0) { // will be the same for all
	sum_energies.push_back(this_energy);
	sum_energy_high_errs.push_back(this_high_energy_err);
	sum_energy_low_errs.push_back(this_low_energy_err);

	sum_rates.push_back(this_rate);
	sum_rate_high_errs.push_back(this_high_rate_err*this_high_rate_err);
	sum_rate_low_errs.push_back(this_low_rate_err*this_low_rate_err);
      }
      else {
	if (i_element >= sum_energies.size()) { //  some graphs might have alrger energy range than others (e.g. alphas)
	  continue;
	}
	double& old_rate = sum_rates.at(i_element);
	double& old_rate_high_error = sum_rate_high_errs.at(i_element);
	double& old_rate_low_error = sum_rate_low_errs.at(i_element);
	//	std::cout << "Old Rate = " << old_rate << " + " << std::sqrt(old_rate_high_error) << " - " << std::sqrt(old_rate_low_error) << std::endl;

	old_rate += this_rate;
	old_rate_high_error += this_high_rate_err*this_high_rate_err;
	old_rate_low_error += this_low_rate_err*this_low_rate_err;

	//	std::cout << "New Rate = " << old_rate << " + " << std::sqrt(old_rate_high_error) << " - " << std::sqrt(old_rate_low_error) << std::endl;
      }
    }
  }

  // Now sqrt everything
  for (int i_element = 0; i_element < sum_energies.size(); ++i_element) {
    sum_rate_high_errs.at(i_element) = std::sqrt(sum_rate_high_errs.at(i_element));
    sum_rate_low_errs.at(i_element) = std::sqrt(sum_rate_low_errs.at(i_element));
  }


  TGraphAsymmErrors* hFinalStatSyst = new TGraphAsymmErrors(sum_energies.size(), &sum_energies[0], &sum_rates[0], &sum_energy_low_errs[0], &sum_energy_high_errs[0], &sum_rate_low_errs[0], &sum_rate_high_errs[0]);
  hFinalStatSyst->SetName("hFinalStatSyst");


  std::cout << "Systematic_SumPlot " << args.outdirname << std::endl;

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hFinalStatSyst->Write();
  outfile->Write();
  outfile->Close();
}
