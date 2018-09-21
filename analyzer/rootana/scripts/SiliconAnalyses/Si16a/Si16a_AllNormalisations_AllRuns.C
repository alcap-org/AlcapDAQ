#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_AllNormalisations.C"

void Si16a_AllNormalisations_AllRuns() {
  const int n_runs = 8;
  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};

  //  const int n_runs = 1;
  //  int run_numbers[n_runs] = {9740};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    Si16a_AllNormalisations(run_numbers[i_run]);
  }
}


  
