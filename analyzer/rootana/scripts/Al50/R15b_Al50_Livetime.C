{
  int nb   = 0;
  int ntme = 0;
  for (int i = /*9890*/10081; i < 10128; ++i) {
    char ifname[64];
    sprintf(ifname, "~/R15bTME/Al50/tme%05d.root", i);
    TFile* f = new TFile(ifname, "READ");
    if (f->IsZombie())
      continue;
    TTree* tr = (TTree*)f->Get("TMETree/TMETree");
    Int_t b;
    int ret = tr->SetBranchAddress("blockId", &b);
    ret = tr->GetEvent(tr->GetEntries()-1);
    nb   += b+1; // Add one for block 0;
    ntme += tr->GetEntries();
    f->Close();
    delete f;
  }
  printf("Blocks: %d\n", nb);
  printf("Livetime: %g hours\n", nb*(96.e-3-10e-6)/3600.);
  printf("TMEs: %d\n", ntme);
}
