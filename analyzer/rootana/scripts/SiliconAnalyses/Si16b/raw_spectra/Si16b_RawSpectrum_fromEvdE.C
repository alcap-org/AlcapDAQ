#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEvdE.C"

void Si16b_RawSpectrum_fromEvdE(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEvdEArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  // For protons, deuterons etc
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};//, "proton_combined"};
  TH1D* hSum = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string i_particle_name = particle_names[i_particle];

    // Get the TCutG spectra
    args.outdirname = i_particle_name + "_TCutG";
    std::string indirname = i_particle_name + "_SiR_timecut400_10000ns_layerCoinc";
    args.datacuttreename = indirname + "/cuttree";

    args.datahistnames.clear();
    args.scale_ratios.clear();

    std::string datahistname = indirname + "/hEvdE_TwoLayer_12not3";
    double scale_ratio = 1;
    args.datahistnames.push_back(datahistname);
    args.scale_ratios.push_back(scale_ratio);
    args.projection_x = true;
    RawSpectrum_fromEvdE(args);

    // Get the PSel spectra
    args.outdirname = i_particle_name + "_PSel";
    indirname = i_particle_name + "_SiR_timecut400_10000ns_layerCoinc_PSel";
    args.datacuttreename = indirname + "/cuttree";

    args.datahistnames.clear();
    args.scale_ratios.clear();

    datahistname = indirname + "/hEvdE_TwoLayer_12not3";
    scale_ratio = 1;
    args.datahistnames.push_back(datahistname);
    args.scale_ratios.push_back(scale_ratio);
    args.projection_x = true;
    RawSpectrum_fromEvdE(args);

    // Combine a do individual channels
    std::stringstream channel_str;
    for (int i_channel = 2; i_channel < 16; ++i_channel) {
      channel_str.str("");
      channel_str << i_channel;
      args.outdirname = i_particle_name + "_SiL1_" + channel_str.str();
      std::string indirname = i_particle_name + "_SiR_timecut400_10000ns_layerCoinc_wSiL1_" + channel_str.str() + "Coinc_PSel";
      args.datacuttreename = indirname + "/cuttree";

      args.datahistnames.clear();
      args.scale_ratios.clear();

      datahistname = indirname + "/hEvdE_TwoLayer_123";
      scale_ratio = 1;
      args.datahistnames.push_back(datahistname);
      args.scale_ratios.push_back(scale_ratio);
      args.projection_x = true;
      RawSpectrum_fromEvdE(args);
    }
  }
  
    /* // for combined two layer and three layer analysis    
  args.datahistnames.push_back("proton_SiR/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  args.datahistnames.push_back("proton_SiR/hEvdE_ThreeLayer_123");
  args.scale_ratios.push_back(1.0 / 0.64);
  RawSpectrum_fromEvdE(args);
    */

  /* // for three layer only
  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "proton_ThreeLayer";
  args.datacuttreename = "proton_SiR/cuttree";
  args.datahistnames.push_back("proton_SiR/hEvdE_ThreeLayer_123");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);
  */
}
