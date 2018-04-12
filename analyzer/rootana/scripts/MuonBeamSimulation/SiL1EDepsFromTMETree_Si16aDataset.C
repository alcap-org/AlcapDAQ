
void SiL1EDepsFromTMETree_Si16aDataset() {

  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/SiL1EDepsFromTMETree.C++");

  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};

  std::stringstream filename, out_filename;
  for (int i_run = 0;i_run < n_runs; ++i_run) {
    filename.str("");
    filename << "~/data/out/v10/out0" << run_numbers[i_run] << ".root";

    out_filename.str("");
    out_filename << "scripts/MuonBeamSimulation/hist_files/SiL1EDepsFromTMETree_Si16aDataset_Run" << run_numbers[i_run] << ".root";
    bool is_Si16a_dataset = true;
    SiL1EDepsFromTMETree(filename.str(), out_filename.str(), is_Si16a_dataset);
  }
}
