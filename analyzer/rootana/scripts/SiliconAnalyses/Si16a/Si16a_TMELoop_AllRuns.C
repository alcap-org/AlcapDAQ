#include "scripts/SiliconAnalyses/Si16a/Si16a_TMELoop.C"

void Si16a_TMELoop_AllRuns() {
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    Si16a_TMELoop(run_numbers[i_run]);
  }
}
