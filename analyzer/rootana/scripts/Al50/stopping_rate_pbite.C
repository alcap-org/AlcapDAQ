{
  const int N = 16;
  TTree* tr[N];
  for (int i = 0; i < N; ++i) {
    char ifname[128];
    std::sprintf(ifname, "~/data/R15b/mc/beamwidth_studies/R15bAl50MuPreSiT_pbite%.2f.root", 0.50+i*0.1);
    TFile* f = new TFile(ifname);
    tr[i] = (TTree*)f->Get("tree");
  }
  int ndet[N] = {}, nstop[N] = {};
  for (int i = 0; i < N; ++i) {
    std::vector<std::string> *vols = nullptr, *particles = nullptr;
    std::vector<int> *stops = nullptr;
    tr[i]->SetBranchStatus ("*", 0);
    tr[i]->SetBranchStatus ("M_volName",      1);
    tr[i]->SetBranchStatus ("M_particleName", 1);
    tr[i]->SetBranchStatus ("M_stopped",      1);
    tr[i]->SetBranchAddress("M_volName",      &vols);
    tr[i]->SetBranchAddress("M_particleName", &particles);
    tr[i]->SetBranchAddress("M_stopped",      &stops);
    std::cout << "Tree " << i << "..." << std::endl;
    for (int j = 0; j < tr[i]->GetEntries(); ++j) {
      tr[i]->GetEntry(j);
      bool detected = false;
      for (int k = 0; k < vols->size(); ++k) {
        if (particles->at(k) != "mu-")
          continue;
        if (!detected && vols->at(k) == "SiT") {
          detected = true;
          ++ndet[i];
        }
        if (detected && vols->at(k) == "Target" && stops->at(k) == 1)
          ++nstop[i];
      }
    }
  }
}
