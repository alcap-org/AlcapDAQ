{
  // Al50 p=1.02
  // const char* DATASET   = "Al100 p=1.02";
  // int RUNS[2]           = { 9890, 10080};
  // const char* IFNAMEFMT = "~/R15bTME/Al50/tme%05d.root";
  // Al50 p=1.03
  const char* DATASET   = "Al50 p=1.03";
  int RUNS[2]           = { 10081, 10127};
  const char* IFNAMEFMT = "~/R15bTME/Al50/tme%05d.root";
  // Al100:       9410-9682
  // const char* DATASET   = "Al100";
  // int RUNS[2]           = { 9410, 9682};
  // const char* IFNAMEFMT = "~/R15bTME/Al100/out%05d.root";

  int nb      = 0;
  int ntme    = 0;
  int ntme_pp = 0;
  for (int i = RUNS[0]; i <= RUNS[1]; ++i) {
    char ifname[64];
    sprintf(ifname, IFNAMEFMT, i);
    TFile* f = new TFile(ifname, "READ");
    if (f->IsZombie())
      continue;
    TTree* tr = (TTree*)f->Get("TMETree/TMETree");
    Int_t b;
    int ret = tr->SetBranchAddress("blockId", &b);
    ret = tr->GetEvent(tr->GetEntries()-1);
    nb      += b+1; // Add one for block 0;
    ntme    += tr->GetEntries();
    ntme_pp += tr->GetEntries("timeToPrevTME > 10e3 && timeToNextTME > 10e3");
    f->Close();
    delete f;
  }
  printf("           %s\n", DATASET);
  printf("Blocks:    %d\n", nb);
  printf("Livetime:  %g hours\n", nb*(96.e-3-10e-6)/3600.);
  printf("TMEs:      %d\n", ntme);
  printf("TMEs (PP): %d\n", ntme_pp);
}
