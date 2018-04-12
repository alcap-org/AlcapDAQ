void PunchThroughFit_SiTEDeps_Si16aDataset() {
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};

  std::stringstream macro;
  for (int i_run = 0;i_run < n_runs; ++i_run) {
    macro.str("");
    macro << ".x scripts/MuonBeamSimulation/PunchThroughFit_SiTEDeps_Si16a_Run" << run_numbers[i_run] << ".C";

    gROOT->ProcessLine(macro.str().c_str());
  }
}
