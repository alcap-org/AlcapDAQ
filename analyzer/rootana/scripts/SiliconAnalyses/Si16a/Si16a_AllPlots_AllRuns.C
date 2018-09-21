#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_AllPlots.C"

void Si16a_AllPlots_AllRuns() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    Si16a_AllPlots(run_numbers[i_run]);
  }
}


  
