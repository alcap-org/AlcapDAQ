#include "../../Utils/TargetSpectrum.C"

void SiL3_TargetSpectrum(std::string infilename, std::string outfilename) {

  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_Target";
  args.layername = "thick";
  args.layerveto = false;
  args.layerreqd = false;
  args.vetolayername = "thin";
  args.reqdlayername = "thin";
  args.outfilename = outfilename;

  args.min_energy = 0;
  args.max_energy = 50;
  args.energy_width = 0.025;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 50;

  const int n_particles = 4;
  double particle_recoils[n_particles] = {1.0/27.0, 2.0/26.0, 3.0/25.0, 4.0/24.0};
  //  double allRecoil_ratios[n_particles] = {0.436, 0.341, 0.047, 0.176}; double allRecoil = 0;
  //  double allRecoil_ratios[n_particles] = {0.451, 0.319, 0.048, 0.182}; double allRecoil = 0;
  //  double allRecoil_ratio_errs[n_particles] = {0.058, 0.042, 0.015, 0.021};

  // old ratios
  // double allRecoil_ratios[n_particles] = {0.443, 0.323, 0.049, 0.185}; double allRecoil = 0;
  // double allRecoil_ratio_high_errs[n_particles] = {0.066, 0.051, 0.017, 0.063};
  // double allRecoil_ratio_low_errs[n_particles] = {0.064, 0.061, 0.023, 0.024};
  // double allRecoil_ratio_high_used_errs[n_particles] = {0.066, 0, 0.001, 0.063}; // because we can't use the full uncertainty without going over 100%
  // double allRecoil_ratio_low_used_errs[n_particles] = {0.064, 0.019, 0.023, 0.024};

  // new ratios (6th Feb 2020)
  // double allRecoil_ratios[n_particles] = {0.453, 0.343, 0.054, 0.150}; double allRecoil = 0;
  // double allRecoil_ratio_high_errs[n_particles] = {0.093, 0.067, 0.017, 0.039};
  // double allRecoil_ratio_low_errs[n_particles] = {0.099, 0.109, 0.023, 0.036};
  // double allRecoil_ratio_high_used_errs[n_particles] = {0.093, 0.043, 0.017, 0.039}; // because we can't use the full uncertainty without going over 100%
  // double allRecoil_ratio_low_used_errs[n_particles] = {0.099, 0.034, 0.023, 0.036};

  // new ratios (2nd July 2020)
  // updated slightly (23rd July 2020)
  // updated slightly again (17th August 2020)
  // updated slightly again again (7th December 2020)
  // updated slightly again again for final time (16th June 2021)
  double allRecoil_ratios[n_particles] = {0.502, 0.300, 0.043, 0.155}; double allRecoil = 0;
  double systRecoil_ratios[n_particles] = {0.745, 0.210, 0.045, 0.0}; double systRecoil = 0;
  //  double allRecoil_ratio_high_errs[n_particles] = {0.060, 0.063, 0.012, 0.053};
  //  double allRecoil_ratio_low_errs[n_particles] = {0.060, 0.062, 0.013, 0.042};
  //  double allRecoil_ratio_high_used_errs[n_particles] = {0.060, 0.000, 0.007, 0.053}; // because we can't use the full uncertainty without going over 100%
  //  double allRecoil_ratio_low_used_errs[n_particles] = {0.060, 0.005, 0.013, 0.042};

  //  double lowAllRecoil_ratios[n_particles] = {}; double lowAllRecoil = 0; //0.492, 0.319, 0.033, 0.156};
  //  double highAllRecoil_ratios[n_particles] = {}; double highAllRecoil = 0; //{0.380, 0.363, 0.061, 0.196};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    // if (i_particle == 0) { // proton
    //   lowAllRecoil_ratios[i_particle] = allRecoil_ratios[i_particle] + allRecoil_ratio_high_used_errs[i_particle]; // increase proton ratio
    //   highAllRecoil_ratios[i_particle] = allRecoil_ratios[i_particle] - allRecoil_ratio_low_used_errs[i_particle]; // decrease proton ratio
    // }
    // else {
    //   lowAllRecoil_ratios[i_particle] = allRecoil_ratios[i_particle] - allRecoil_ratio_low_used_errs[i_particle]; // decrease triton and alpha ratio by full amount
    //   highAllRecoil_ratios[i_particle] = allRecoil_ratios[i_particle] + allRecoil_ratio_high_used_errs[i_particle]; // increase triton and alpha ratio by full amount
    // }
      
    allRecoil += particle_recoils[i_particle] * allRecoil_ratios[i_particle];
    //    lowAllRecoil += particle_recoils[i_particle] * lowAllRecoil_ratios[i_particle];
    //    highAllRecoil += particle_recoils[i_particle] * highAllRecoil_ratios[i_particle];
    systRecoil += particle_recoils[i_particle] * systRecoil_ratios[i_particle];
  }
  
  const int n_recoils = 8;
  std::string recoil_names[n_recoils] = {"noRecoil", "nuRecoil", "pRecoil", "dRecoil", "tRecoil", "aRecoil", "allRecoil", "systRecoil"};//, "lowAllRecoil", "highAllRecoil"};
  double recoil_fractions[n_recoils] = {0, -0.190, particle_recoils[0], particle_recoils[1], particle_recoils[2], particle_recoils[3], allRecoil, systRecoil};//, lowAllRecoil, highAllRecoil};
  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    args.outdirname = "Target_" + recoil_names[i_recoil];
    args.recoil_fraction = recoil_fractions[i_recoil];

    std::cout << args.outdirname << ": Recoil Fraction = " << recoil_fractions[i_recoil] << std::endl;
    TargetSpectrum(args);
  }

  //  args.outdirname = "FlatBkg";
  //  args.min_time = -20000;
  //  args.max_time = -2000;
  //  TargetSpectrum(args);
}
