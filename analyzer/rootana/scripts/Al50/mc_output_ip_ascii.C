{
  TFile* f =  new TFile("data/mc/R15bAl50ProtTargFromMuDist_1.root");
  Double_t px, py, pz;
  tree->SetBranchAddress("i_px", &px);
  tree->SetBranchAddress("i_py", &py);
  tree->SetBranchAddress("i_pz", &pz);
  std::ofstream ofile("out/mc_al50_p0.csv");
  for (int i = 0; i < 100e3; ++i) {
    tree->GetEvent(i);
    ofile << px << " " << py << " " << pz << std::endl;
  }
  ofile.close();

}
