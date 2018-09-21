#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_AllPlots.C"

void Si16a_AllPlots_OneRunScan() {

  double min_muon_energy = 200;
  double max_muon_energy = 2000;
  double muon_energy_slice = 200;
  for (double i_start_muon_slice = min_muon_energy; i_start_muon_slice < max_muon_energy;i_start_muon_slice += muon_energy_slice) {
    double i_end_muon_slice = i_start_muon_slice + muon_energy_slice;
    Si16a_AllPlots(9743, i_start_muon_slice, i_end_muon_slice);
  }
}


  
