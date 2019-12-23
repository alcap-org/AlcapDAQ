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
  args.max_energy = 50000;
  args.energy_width = 25;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 50;

  const int n_particles = 4;
  double particle_recoils[n_particles] = {1.0/27.0, 2.0/26.0, 3.0/25.0, 4.0/24.0};
  double allRecoil_ratios[n_particles] = {0.436, 0.341, 0.047, 0.176}; double allRecoil = 0;
  double lowAllRecoil_ratios[n_particles] = {0.492, 0.319, 0.033, 0.156}; double lowAllRecoil = 0;
  double highAllRecoil_ratios[n_particles] = {0.380, 0.363, 0.061, 0.196}; double highAllRecoil = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    allRecoil += particle_recoils[i_particle] * allRecoil_ratios[i_particle];
    lowAllRecoil += particle_recoils[i_particle] * lowAllRecoil_ratios[i_particle];
    highAllRecoil += particle_recoils[i_particle] * highAllRecoil_ratios[i_particle];
  }
  
  const int n_recoils = 8;
  std::string recoil_names[n_recoils] = {"noRecoil", "pRecoil", "dRecoil", "tRecoil", "aRecoil", "allRecoil", "lowAllRecoil", "highAllRecoil"};
  double recoil_fractions[n_recoils] = {0, particle_recoils[0], particle_recoils[1], particle_recoils[2], particle_recoils[3], allRecoil, lowAllRecoil, highAllRecoil};
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
