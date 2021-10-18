
void SiL3EDepsFromTMETree_SiL3RangeScanDataset() {

  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/SiL3EDepsFromTMETree.C++");

  const int n_runs = 3;
  int run_numbers[n_runs] = {8994, 8995, 8996};

  std::stringstream filename, out_filename;
  for (int i_run = 0;i_run < n_runs; ++i_run) {
    filename.str("");
    filename << "~/data/out/local/out0" << run_numbers[i_run] << "_tme-tree_no-templates.root";

    out_filename.str("");
    out_filename << "scripts/MuonBeamSimulation/hist_files/SiL3EDepsFromTMETree_SiL3RangeScanDataset_Run" << run_numbers[i_run] << ".root";
    SiL3EDepsFromTMETree(filename.str(), out_filename.str());
  }
}
