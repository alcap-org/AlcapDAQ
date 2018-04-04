
void SiTEDepsFromTMETree_Si16bDataset() {

  gROOT->Load("scripts/MuonBeamSimulation/SiTEDepsFromTMETree.C");
  std::string filename = "~/data/out/v10/Si16b.root";
  std::string out_filename = "scripts/MuonBeamSimulation/SiTEDepsFromTMETree_Si16bDataset.root";
  bool is_active_target_run = true;
  SiTEDepsFromTMETree(filename, out_filename, is_active_target_run);
}
