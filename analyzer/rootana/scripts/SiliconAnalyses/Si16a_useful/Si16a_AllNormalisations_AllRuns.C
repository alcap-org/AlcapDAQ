#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_AllNormalisations.C"

void Si16a_AllNormalisations_AllRuns() {
  //  const int n_runs = 8;
  //  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};
  const int n_runs = 8;
  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};
  double min_muon_energies[n_runs] = {400, 400, 700, 700, 700, 700, 800, 800};
  double max_muon_energies[n_runs] = {700, 800, 1000, 1000, 1000, 1000, 1100, 1100};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    Si16a_AllNormalisations(run_numbers[i_run], min_muon_energies[i_run], max_muon_energies[i_run]);
  }
}


  
