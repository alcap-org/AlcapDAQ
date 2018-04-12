
void SiTEDepsFromTMETree_Si16aDataset() {

  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/SiTEDepsFromTMETree.C++");

  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};

  std::stringstream filename, out_filename;
  for (int i_run = 0;i_run < n_runs; ++i_run) {
    filename.str("");
    filename << "~/data/out/v10/out0" << run_numbers[i_run] << ".root";

    out_filename.str("");
    out_filename << "scripts/MuonBeamSimulation/SiTEDepsFromTMETree_Si16aDataset_Run" << run_numbers[i_run] << ".root";
    bool is_active_target_run = true;
    SiTEDepsFromTMETree(filename.str(), out_filename.str(), is_active_target_run);
  }
}
