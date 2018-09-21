#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_AllNormalisations.C"

void Si16a_AllNormalisations_OneRunScan() {

  double min_muon_energy = 200;
  double max_muon_energy = 2000;
  double muon_energy_slice = 200;
  int rebin_factor = 2;
  for (double i_start_muon_slice = min_muon_energy; i_start_muon_slice < max_muon_energy;i_start_muon_slice += muon_energy_slice) {
    double i_end_muon_slice = i_start_muon_slice + muon_energy_slice;
    Si16a_AllNormalisations(9743, i_start_muon_slice, i_end_muon_slice, rebin_factor);
  }

}


  
