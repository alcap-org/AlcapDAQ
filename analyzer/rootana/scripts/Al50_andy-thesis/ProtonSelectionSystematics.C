#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"

void ProtonSelectionSystematics() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::string version = "v92";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* SiL_EvdE = (TH2F*) (file->Get("TME_Al50_EvdE/all_particles/SiL_EvdE"))->Clone();
  TH2F* SiR_EvdE = (TH2F*) (file->Get("TME_Al50_EvdE/all_particles/SiR_EvdE"))->Clone();
  SiL_EvdE->SetDirectory(0);
  SiR_EvdE->SetDirectory(0);
  file->Close();

  filename = "~/g4sim/alcap/scripts/Al50/PIDProfiles.root";
  file = new TFile(filename.c_str(), "READ");
  TH1D* SiL_PID_profile = (TH1D*) (file->Get("profile_MC-scaled_SiL_p_stop"))->Clone();
  TH1D* SiR_PID_profile = (TH1D*) (file->Get("profile_MC-scaled_SiR_p_stop"))->Clone();
  SiL_PID_profile->SetDirectory(0);
  SiR_PID_profile->SetDirectory(0);
  file->Close();

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  TH2F* evde_hists_rescaled[n_arms];
  TH2F* evde_hists_rescaled_selected_band[n_arms];
  TH1D* pid_profiles[n_arms] = {SiL_PID_profile, SiR_PID_profile};
  double data_mean_x[n_arms] = {5847, 5951};
  double data_mean_y[n_arms] = {856.4, 779.1};
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  int n_selected[n_arms] = {0, 0};

  TFile* output_file = new TFile("systematics_Al50.root", "RECREATE");

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    int n_bins_x = evde_hists[i_arm]->GetXaxis()->GetNbins();
    double max_x = evde_hists[i_arm]->GetXaxis()->GetXmax();
    int n_bins_y = evde_hists[i_arm]->GetYaxis()->GetNbins();
    double max_y = evde_hists[i_arm]->GetYaxis()->GetXmax();

    std::string histname = arm_names[i_arm] + "_EvdE_rescaled";
    evde_hists_rescaled[i_arm] = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,0,max_x/data_mean_x[i_arm], n_bins_y,0,max_y/data_mean_y[i_arm]);
      
    // Loop through the bins and rescale
    for (int i_bin = 1; i_bin <= n_bins_x; ++i_bin) {
      for (int j_bin = 1; j_bin <= n_bins_y; ++j_bin) {
	double old_x = evde_hists[i_arm]->GetXaxis()->GetBinCenter(i_bin);
	double old_y = evde_hists[i_arm]->GetYaxis()->GetBinCenter(j_bin);
	double content = evde_hists[i_arm]->GetBinContent(i_bin, j_bin);

	int new_i_bin = evde_hists_rescaled[i_arm]->GetXaxis()->FindBin(old_x / data_mean_x[i_arm]);
	int new_j_bin = evde_hists_rescaled[i_arm]->GetYaxis()->FindBin(old_y / data_mean_y[i_arm]);
	evde_hists_rescaled[i_arm]->SetBinContent(new_i_bin, new_j_bin, content);
      }
    }
      //    evde_hists[i_arm]->Draw("COLZ");
    evde_hists_rescaled[i_arm]->Draw("COLZ");
    pid_profiles[i_arm]->Draw("SAME");

    histname = arm_names[i_arm] + "_EvdE_rescaled_selected_band";
    evde_hists_rescaled_selected_band[i_arm] = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,0,max_x, n_bins_y,0,max_y);

    // Now loop through the PID profile and see how many entries in the data are selected
    n_bins_x = pid_profiles[i_arm]->GetXaxis()->GetNbins();
    for (int i_bin = 1; i_bin <= n_bins_x; ++i_bin) {
      double mean = pid_profiles[i_arm]->GetBinContent(i_bin);
      double rms = pid_profiles[i_arm]->GetBinError(i_bin);
      double i_scaled_energy = pid_profiles[i_arm]->GetBinCenter(i_bin);
      //      std::cout << "Bin #" << i_bin << ": E(scaled) = " << i_scaled_energy << ", mean = " << mean << ", rms = " << rms << std::endl;

      double new_bin = evde_hists_rescaled[i_arm]->GetXaxis()->FindBin(i_scaled_energy);
      TH1D* hProjection = evde_hists_rescaled[i_arm]->ProjectionY("_py", new_bin, new_bin);
      int integral_low = hProjection->FindBin(mean - 2*rms);
      int integral_high = hProjection->FindBin(mean + 2*rms);
      //      std::cout << "New Bin #" << new_bin << ": low_bin = " << integral_low << ", high_bin = " << integral_high << std::endl;
      n_selected[i_arm] += hProjection->Integral(integral_low, integral_high);

      // Now loop through the projection and add to the new plot
      for (int j_bin = integral_low; j_bin <= integral_high; ++j_bin) {
	evde_hists_rescaled_selected_band[i_arm]->SetBinContent(i_bin, j_bin, hProjection->GetBinContent(j_bin));
      }
    }
    std::cout << arm_names[i_arm] << ": " << n_selected[i_arm] << ", " << evde_hists_rescaled_selected_band[i_arm]->ProjectionX()->Integral() << std::endl;

    evde_hists[i_arm]->Write();
    evde_hists_rescaled[i_arm]->Write();
    evde_hists_rescaled_selected_band[i_arm]->Write();
    pid_profiles[i_arm]->Write();
  }
}
