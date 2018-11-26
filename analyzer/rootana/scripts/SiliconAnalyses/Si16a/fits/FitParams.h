#ifndef FitParams_hh_
#define FitParams_hh_

namespace SiL1FitParams {
  bool add_stop = true;
  int rebin_factor = 2;
  
  double edep_min_wPunchthrough = 200;
  double edep_max_wPunchthrough = 1200;
  double edep_min_woutPunchthrough = 100;
  double edep_max_woutPunchthrough = 1200;

  double landau_mean_min = 200;
  double landau_mean_max = 750;
  double landau_sigma_min = 10;
  double landau_sigma_max = 200;
  
  double resolution_sigma_min = 30;
  double resolution_sigma_max = 30;
  
  double stop_edep_min_wPunchthrough = 750;
  double stop_edep_min_woutPunchthrough = 300;
  double stop_edep_max = 1200;
  double stop_sigma_min = 10;
  double stop_sigma_max = 200;
  double stop_alpha_min = 0.00001;
  double stop_alpha_max = 0.9;//0.5

  int n_punch_min = 0;
  int n_punch_max = 3000000;
  int n_stop_min = 1;
  int n_stop_max = 3000000;
}

namespace SiTFitParams {
  bool add_stop = false;
  int rebin_factor = 1;
    
  double edep_min_wPunchthrough = 200;
  double edep_max_wPunchthrough = 1200;
  double edep_min_woutPunchthrough = 100;
  double edep_max_woutPunchthrough = 1000;

  double landau_mean_min = 200;
  double landau_mean_max = 750;
  double landau_sigma_min = 10;
  double landau_sigma_max = 200;

  double resolution_sigma_min = 41;
  double resolution_sigma_max = 41;
  
  double stop_edep_min = 900;
  double stop_edep_max = 1100;
  double stop_sigma_min = 1.0;
  double stop_sigma_max = 200;
  double stop_alpha_min = 0.00001;
  double stop_alpha_max = 0.9;//0.5

  int n_punch_min = 0;
  int n_punch_max = 3000000;
  int n_stop_min = 1;
  int n_stop_max = 3000000;
}

#endif
