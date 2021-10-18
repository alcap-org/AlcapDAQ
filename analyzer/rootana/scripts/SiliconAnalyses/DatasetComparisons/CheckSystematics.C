#include "../Utils/IntegrateRates.C"

void CheckSystematics() {

  TFile* central_file = new TFile("/data/ssd3/R15bAnalysis/hists/al50-unfolded.root", "READ");
  TFile* syst_file = new TFile("/data/ssd3/R15bAnalysis/hists/al50-unfolded-systematics.root", "READ");
  //    TFile* combined_file = new TFile("/data/ssd3/R15bAnalysis/hists/final-al50-plots.root", "READ");
  TFile* combined_file = new TFile("final-al50-plots.root", "READ");

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  double min_energies[n_particles] = {3.5, 4.5, 5.0, 15.0};
  double max_energies[n_particles] = {20.0, 17.0, 17.0, 20.0};

  const int n_systematics = 4;
  std::string systematics[n_systematics] = {"MisCalib", "NumIter", "BinSize", "BeamEnergy"};

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    double min_E = min_energies[i_particle];
    double max_E = max_energies[i_particle];


    std::string histname = "h" + particle + "_SiR";
    TH1D* hCentral = (TH1D*) central_file->Get(histname.c_str());

    histname = "gCombined-al50-" + particle;
    TGraphErrors* gCombinedOriginal = (TGraphErrors*) combined_file->Get(histname.c_str());
    histname = "gStats-al50-" + particle;
    TGraphErrors* gStatOriginal = (TGraphErrors*) combined_file->Get(histname.c_str());

    
    TGraphAsymmErrors* gCombined =  new TGraphAsymmErrors(gCombinedOriginal->GetN(),
							  gCombinedOriginal->GetX(),
							  gCombinedOriginal->GetY(),
							  gCombinedOriginal->GetEX(),
							  gCombinedOriginal->GetEX(),
							  gCombinedOriginal->GetEY(),
							  gCombinedOriginal->GetEY()
							  );
    TGraphAsymmErrors* gStat =  new TGraphAsymmErrors(gStatOriginal->GetN(),
						      gStatOriginal->GetX(),
						      gStatOriginal->GetY(),
						      gStatOriginal->GetEX(),
						      gStatOriginal->GetEX(),
						      gStatOriginal->GetEY(),
						      gStatOriginal->GetEY()
						      );

    for (int i_systematic = 0; i_systematic < n_systematics; ++i_systematic) {
      std::string systematic = systematics[i_systematic];

      histname = "h" + particle + "_" + systematic + "ErrSiR";
      TH1D* hSyst = (TH1D*) syst_file->Get(histname.c_str());
      if (!hSyst) {
	continue;
      }
      //      hCentral->Draw("HIST E");
      //hSyst->Draw("HIST E SAME");
      double total_sq_err = 0;
      double total_central = 0;
      double total_syst = 0;
      for (int i_bin = 1; i_bin <= hCentral->GetNbinsX(); ++i_bin) {
	double central = hCentral->GetBinContent(i_bin);
	double syst = hSyst->GetBinContent(i_bin);
	double E = hCentral->GetXaxis()->GetBinCenter(i_bin);
    
	if (E>min_E && E<max_E) {
	  //	  std::cout << "E = " << E << ": Central = " << central << ", Syst = " << syst << ", frac error = " << syst/central << std::endl;
	  total_sq_err += syst*syst;
	  total_central += central;
	  total_syst += syst;
	}
      }
      std::cout << particle << ": " << systematic << ": range " << min_E << " MeV and " << max_E << " MeV = " << std::sqrt(total_sq_err) << " / " << total_central << " = " << std::sqrt(total_sq_err) / total_central * 100 << "%" << std::endl;
      //      std::cout << "Total Central = " << total_central << std::endl;
      //      std::cout << "Total Syst = " << total_syst << std::endl;
      //      std::cout << "Total Err (" << min_E << " MeV -- " << max_E << " MeV) = " << total_syst/total_central << std::endl;
      //      std::cout << "Total Sq Err = " << total_sq_err << std::endl;
      //      std::cout << "sqrt(Total Sq Err) = " << std::sqrt(total_sq_err) << " / " << total_central << " = " << std::sqrt(total_sq_err) / total_central << std::endl;
    }
    // total_central = 0;
    // total_syst = 0;
    // total_sq_err = 0;
    // for (int i_point = 0; i_point < gCombined->GetN(); ++i_point) {
    //   double E = *(gCombined->GetX()+i_point);
    //   double central = *(gCombined->GetY()+i_point);
    //   double syst = std::sqrt(*(gCombined->GetEYhigh()+i_point)* *(gCombined->GetEYhigh()+i_point) - *(gStat->GetEYhigh()+i_point)* *(gStat->GetEYhigh()+i_point));
    //   if (E>min_E && E<max_E) {
    // 	std::cout << "E = " << E << ": Central = " << central << ", Syst = " << syst << ", frac error = " << syst/central << std::endl;
    // 	total_central += central;
    // 	total_syst += syst;
    // 	total_sq_err += syst*syst;
    //   }
    // }
    // std::cout << "Total Central = " << total_central << std::endl;
    // std::cout << "Total Syst = " << total_syst << std::endl;
    // std::cout << "Total Sq Err = " << total_sq_err << std::endl;
    // std::cout << "sqrt(Total Sq Err) = " << std::sqrt(total_sq_err) << " / " << total_central << " = " << std::sqrt(total_sq_err) / total_central << std::endl;
    // std::cout << "Total Err (" << min_E << " MeV -- " << max_E << " MeV) = " << total_syst/total_central << std::endl;

    double rate = 0; double total_stat_error = 0;
    double total_high_syst_error = 0; double total_low_syst_error = 0;

    IntegrateRates_wStatAndStatSystSpectra(gStat, gCombined,
					   min_E, max_E,
					     rate, total_stat_error,
					     total_high_syst_error, total_low_syst_error);
    //    std::cout << "Total From IntegrateRates: " << total_high_syst_error << " / " << rate << " = " << total_high_syst_error / rate << std::endl;
    std::cout << "Total From IntegrateRates.C = ^" <<  total_high_syst_error / rate * 100 << "% _" << total_low_syst_error / rate * 100 << "%" << std::endl;
    std::cout << "====================" << std::endl;
  }
}
