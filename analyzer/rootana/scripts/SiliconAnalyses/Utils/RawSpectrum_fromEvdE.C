#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include <iostream>
#include <sstream>

struct RawSpectrum_fromEvdEArgs {

  std::string datafilename;
  std::vector<std::string> datahistnames;
  std::vector<double> scale_ratios;
  std::vector<double> scale_ratio_errors;
  std::string datacuttreename;

  std::string outfilename;
  std::string outdirname;

  bool projection_x;
  double max_energy_cutoff;

  double min_add_error_energy;
  double max_add_error_energy;
  double additional_error; // to inflate errors because of dead layer

  std::vector<double> energies_to_scale;
  std::vector<double> scales;
  std::vector<double> err_scales;
};

void RawSpectrum_fromEvdE(RawSpectrum_fromEvdEArgs& args) {

  TFile* data_file = new TFile(args.datafilename.c_str(), "READ");
  if (data_file->IsZombie()) {
    std::cout << "Problem opening file " << args.datafilename << std::endl;
    return;
  }

  TTree* cut_tree = ((TTree*) data_file->Get(args.datacuttreename.c_str()));
  if (!cut_tree) {
    std::cout << "Problem getting cut_tree " << args.datacuttreename << std::endl;
    return;
  }
  cut_tree = cut_tree->CloneTree();

  TH1D* raw_spectrum = NULL;
  for (std::vector<std::string>::const_iterator i_histname = args.datahistnames.begin(); i_histname != args.datahistnames.end(); ++i_histname) {
    TH2F* hEvdE = (TH2F*) data_file->Get(i_histname->c_str());
    if (!hEvdE) {
      std::cout << "Problem getting histogram " << i_histname->c_str() << std::endl;
      return;
    }
    std::cout << "hEvdE Entries = " << hEvdE->GetEntries() << std::endl;
    double scale_ratio = args.scale_ratios.at(i_histname - args.datahistnames.begin());
    double scale_ratio_error = args.scale_ratio_errors.at(i_histname - args.datahistnames.begin());
    std::cout << "scale ratio = " << scale_ratio  << " +/- " << scale_ratio_error << std::endl;
    if (!raw_spectrum) {
      if (args.projection_x) {
	raw_spectrum = hEvdE->ProjectionX();
      }
      else {
	raw_spectrum = hEvdE->ProjectionY();
      }
      std::string newname = "hRawSpectrum";
      raw_spectrum->SetName(newname.c_str());
      std::cout << "HRawSpcetrum Entries = " << raw_spectrum->GetEntries() << std::endl;
      // raw_spectrum->Rebin(50);
      // for (int i_bin = 1; i_bin <= raw_spectrum->GetNbinsX(); ++i_bin) {
      // 	double E = raw_spectrum->GetBinCenter(i_bin);
      // 	std::cout << E << " MeV = " << raw_spectrum->GetBinContent(i_bin) << " +/- " << raw_spectrum->GetBinError(i_bin) << std::endl;
      // }
    }
    else {
      //      raw_spectrum->Add(hEvdE->ProjectionX(), scale_ratio);
      TH1F* hNext = (TH1F*) hEvdE->ProjectionX("hNext");
      TH1F* hScale = (TH1F*) hNext->Clone("hScale");
      hScale->Reset();
      //      hScale->SetBit(TH1::kIsAverage);
      int debug_bin = raw_spectrum->FindBin(15);
      std::cout << "AE: DEBUG: hNext (before scale): " << hNext->GetBinContent(debug_bin) << " +/- " << hNext->GetBinError(debug_bin) << std::endl;
      for (int i_bin = 1; i_bin <= hScale->GetNbinsX(); ++i_bin) {
	hScale->SetBinContent(i_bin, scale_ratio);
	hScale->SetBinError(i_bin, scale_ratio_error);
	//	hNext->SetBinContent(i_bin, hNext->GetBinContent(i_bin)/scale_ratio);
	//	hNext->SetBinError(i_bin, ((hNext->GetBinError(i_bin)/hNext->GetBinContent(i_bin)) + (scale_ratio_error/scale_ratio))*hNext->GetBinContent(i_bin));
      }
      hNext->Divide(hScale);
      std::cout << "AE: DEBUG: hNext (after scale " << scale_ratio << " +/- " << scale_ratio_error << "): " << hNext->GetBinContent(debug_bin) << " +/- " << hNext->GetBinError(debug_bin) << std::endl;
      std::cout << "AE: DEBUG: raw_spectrum (before addition): " << raw_spectrum->GetBinContent(debug_bin) << " +/- " << raw_spectrum->GetBinError(debug_bin) << std::endl;
      raw_spectrum->Add(hNext);
      std::cout << "AE: DEBUG: raw_spectrum (after addition): " << raw_spectrum->GetBinContent(debug_bin) << " +/- " << raw_spectrum->GetBinError(debug_bin) << std::endl;
      // hNext->Rebin(50);
      // for (int i_bin = 1; i_bin <= hNext->GetNbinsX(); ++i_bin) {
      // 	double E = hNext->GetBinCenter(i_bin);
      // 	std::cout << E << " MeV = " << hNext->GetBinContent(i_bin) << " +/- " << hNext->GetBinError(i_bin) << std::endl;
      // }
    }
  }
  std::cout << "Before rebin: " << raw_spectrum->GetXaxis()->GetXmax() << ", " << raw_spectrum->GetNbinsX() << std::endl;
  raw_spectrum->Rebin(10);
  std::cout << "After rebin: " << raw_spectrum->GetXaxis()->GetXmax() << ", " << raw_spectrum->GetNbinsX() << std::endl;
  for (int i_bin = 1; i_bin <= raw_spectrum->GetNbinsX(); ++i_bin) {
    double bin_centre = raw_spectrum->GetBinCenter(i_bin);
    if (bin_centre > args.max_energy_cutoff) {
      raw_spectrum->SetBinContent(i_bin, 0);
      raw_spectrum->SetBinError(i_bin, 0);
    }
    //    std::cout << bin_centre << " " << args.min_add_error_energy << " " << args.max_add_error_energy << std::endl;
    if (bin_centre >= args.min_add_error_energy && bin_centre <= args.max_add_error_energy) {
      std::cout << "Bin Center = " << bin_centre << std::endl;
      std::cout << "Debug: Before = " << raw_spectrum->GetBinContent(i_bin) << " +/- " << raw_spectrum->GetBinError(i_bin) << std::endl;
      double old_frac_error = std::fabs(raw_spectrum->GetBinError(i_bin)/raw_spectrum->GetBinContent(i_bin));
      double new_frac_error = old_frac_error + args.additional_error;
      //      std::cout << "Debug: Old --> New = " << old_frac_error << " " << new_frac_error << std::endl;
      //      raw_spectrum->SetBinError(i_bin, raw_spectrum->GetBinError(i_bin)*args.additional_error);
      raw_spectrum->SetBinError(i_bin, raw_spectrum->GetBinContent(i_bin)*new_frac_error);
      std::cout << "Debug: After = " << raw_spectrum->GetBinContent(i_bin) << " +/- " << raw_spectrum->GetBinError(i_bin) << std::endl;
    }
  }
  std::cout << "Before loop: " << raw_spectrum->GetXaxis()->GetXmax() << ", " << raw_spectrum->GetNbinsX() << std::endl;
  // Correct for SiR3 punch-through
  for (int i_corr = 0; i_corr < args.scales.size(); ++i_corr) {
    double corr_energy = args.energies_to_scale.at(i_corr);
    double correction = args.scales.at(i_corr);
    double err_correction = args.err_scales.at(i_corr);

    int bin_to_correct = raw_spectrum->FindBin(corr_energy);
    std::cout << raw_spectrum->GetXaxis()->GetXmax() << ", " << raw_spectrum->GetNbinsX() << std::endl;
    double old_bin_content = raw_spectrum->GetBinContent(bin_to_correct);
    double new_bin_content = old_bin_content*(1.0 - correction);

    double old_frac_error = std::fabs(raw_spectrum->GetBinError(bin_to_correct)/raw_spectrum->GetBinContent(bin_to_correct));
    double new_frac_error = old_frac_error + err_correction;

    std::cout << "*** DEBUG SiR3" << std::endl;
    std::cout << "Corr: " << corr_energy << " MeV, " << correction << ", " << err_correction << std::endl;
    std::cout << "bin_to_correct = " << bin_to_correct << std::endl;
    std::cout << "Before correction: E = " << raw_spectrum->GetBinCenter(bin_to_correct) << ": " << raw_spectrum->GetBinContent(bin_to_correct) << " +/- " << raw_spectrum->GetBinError(bin_to_correct) << std::endl;
    raw_spectrum->SetBinContent(bin_to_correct, new_bin_content);
    raw_spectrum->SetBinError(bin_to_correct, raw_spectrum->GetBinContent(bin_to_correct)*new_frac_error);
    std::cout << "After correction: E = " << raw_spectrum->GetBinCenter(bin_to_correct) << ": " << raw_spectrum->GetBinContent(bin_to_correct) << " +/- " << raw_spectrum->GetBinError(bin_to_correct) << std::endl;
  }

  
  double integral_low = 0;
  double integral_high = 10;
  int integral_bin_low = raw_spectrum->FindBin(integral_low);
  int integral_bin_high = raw_spectrum->FindBin(integral_high)-1;
  std::cout << args.outdirname << ": " << integral_low << " -- " << integral_high << " MeV [" << integral_bin_low << ", " << integral_bin_high << "] : Integral = " << raw_spectrum->Integral(integral_bin_low, integral_bin_high) << std::endl;
  std::cout << args.outdirname << ": Full Integral = " << raw_spectrum->Integral() << std::endl;

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  raw_spectrum->Write();
  cut_tree->Write();
  
  outfile->Write();
  outfile->Close();
}
