#include "SiL1_PunchThroughPeak.C"

void SiL1_PunchThroughPeak_MultipleRuns() {

  const int n_runs = 10;
  int first_run = 9735;
  int last_run = first_run+n_runs;
  float scale_factors[n_runs] = {1.035, 0.99, 0.98, 0, 1.01, 1.02, 1.03, 1.04, 1.05, 0};

  bool save_pdfs=true;
  for (int i_run = first_run; i_run < last_run; ++i_run) {
    std::cout << "AE: " << i_run - first_run << std::endl;
    SiL1_PunchThroughPeak(i_run, scale_factors[i_run-first_run], save_pdfs);

    energy_low = 400;
    energy_high = 2000;
    
    landau_mean_low = 400;
    landau_mean_high = 900;
    landau_mean_error = 0;
    
    landau_sigma_low = 0.1;
    landau_sigma_high = 200;
    landau_sigma_error = 0;
    
    gauss_mean_low = 0;
    gauss_mean_high = 0;
    gauss_mean_error = 0;
    
    gauss_sigma_low = 1;
    gauss_sigma_high = 250;
    gauss_sigma_error = 0;
    
    nbkg_low = 0;
    nbkg_high = 1000000;
    nbkg = 0;
    nbkg_error = 0;
    
    bulge_mean_low = 900;
    bulge_mean_high = 1300;
    bulge_mean_error = 0;
    
    bulge_sigma_low = 10;
    bulge_sigma_high = 250;
    bulge_sigma_error = 0;
    
    nsig_low = 0;
    nsig_high = 1000000;
    nsig = 0;
    nsig_error = 0;
    
    n_fit_params = 0;
    goodness_of_fit = 0;
  }  
}
