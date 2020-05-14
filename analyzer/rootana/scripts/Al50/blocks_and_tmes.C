{
  gROOT->ProcessLine(".L src/plotters/SimplePulse.h+g");
  const int NAL100GRPS = 8;
  const int NAL50GRPS  = 3;
  const int NTI50GRPS  = 2;
  int AL100RUNS[NAL100GRPS][2] = {
    {9410, 9412}, {9494, 9505}, {9506, 9515}, {9516, 9518},
    {9519, 9555}, {9559, 9585}, {9594, 9641}, {9655, 9682}
  };
  int AL50RUNS[NAL50GRPS][2] = {
    {9889, 9996}, {9997, 10080}, {10087, 10129}
  };
  int TI50RUNS[NTI50GRPS][2] = {
    {10157, 10206}, {10208, 10254}
  };

  int nTMEAl100[NAL100GRPS+1] = {0}, nPPTMEAl100[NAL100GRPS+1] = {0};
  int nBlAl100[NAL100GRPS+1] = {0};
  for (int g = 0; g < NAL100GRPS; ++g) {
    int nb = 0, ntme = 0, npptme = 0;
    for (int r = AL100RUNS[g][0]; r <= AL100RUNS[g][1]; ++r) {
      char fname[128];
      sprintf(fname, "~/R15bTME/Al100/out%05d.root", r);
      TFile* f = new TFile(fname);
      if (!f->IsZombie()) {
        int b = 0;
        double t[2] = {0};
        TTree* tr = (TTree*)f->Get("TMETree/TMETree");
        tr->SetBranchStatus("*",             false);
        tr->SetBranchStatus("blockId",       true);
        tr->SetBranchStatus("timeToPrevTME", true);
        tr->SetBranchStatus("timeToNextTME", true);
        tr->SetBranchAddress("blockId", &b);
        tr->GetEntry(tr->GetEntries()-1);
        nb     += b;
        ntme   += tr->GetEntries();
        npptme += tr->GetEntries("timeToPrevTME > 10e3 && timeToNextTME > 10e3");
        delete tr;
      }
      delete f;
    }
    nTMEAl100[g]            =  ntme;
    nTMEAl100[NAL100GRPS]   += ntme;
    nPPTMEAl100[g]          =  npptme;
    nPPTMEAl100[NAL100GRPS] += npptme;
    nBlAl100[g]             =  nb;
    nBlAl100[NAL100GRPS]    += nb;
    printf("%d-%d:\t%d\t%d\t%d\n",
           AL100RUNS[g][0], AL100RUNS[g][1], nb, ntme, npptme);
  }
  printf("Total Al100:\t%d\t%d\t%d\n",
         nBlAl100[NAL100GRPS], nTMEAl100[NAL100GRPS], nPPTMEAl100[NAL100GRPS]);

  int nTMEAl50[NAL50GRPS+1] = {0}, nPPTMEAl50[NAL50GRPS+1] = {0};
  int nBlAl50[NAL50GRPS+1] = {0};
  for (int g = 0; g < NAL50GRPS; ++g) {
    int nb = 0, ntme = 0, npptme = 0;
    for (int r = AL50RUNS[g][0]; r <= AL50RUNS[g][1]; ++r) {
      char fname[128];
      sprintf(fname, "~/R15bTME/Al50/tme%05d.root", r);
      TFile* f = new TFile(fname);
      if (!f->IsZombie()) {
        int b = 0;
        TTree* tr = (TTree*)f->Get("TMETree/TMETree");
        tr->SetBranchStatus("*",             false);
        tr->SetBranchStatus("blockId",       true);
        tr->SetBranchStatus("timeToPrevTME", true);
        tr->SetBranchStatus("timeToNextTME", true);
        tr->SetBranchAddress("blockId", &b);
        tr->GetEntry(tr->GetEntries()-1);
        nb     += b;
        ntme   += tr->GetEntries();
        npptme += tr->GetEntries("timeToPrevTME > 10e3 && timeToNextTME > 10e3");
        delete tr;
      }
      delete f;
    }
    nTMEAl50[g]           =  ntme;
    nTMEAl50[NAL50GRPS]   += ntme;
    nPPTMEAl50[g]         =  npptme;
    nPPTMEAl50[NAL50GRPS] += npptme;
    nBlAl50[g]            =  nb;
    nBlAl50[NAL50GRPS]    += nb;
    printf("%d-%d:\t%d\t%d\t%d\n",
           AL50RUNS[g][0], AL50RUNS[g][1], nb, ntme, npptme);
  }
  printf("Total Al50:\t%d\t%d\t%d\n",
         nBlAl50[NAL50GRPS], nTMEAl50[NAL50GRPS], nPPTMEAl50[NAL50GRPS]);


  int nTMETi50[NTI50GRPS+1] = {0}, nPPTMETi50[NTI50GRPS+1] = {0};
  int nBlTi50[NTI50GRPS+1] = {0};
  for (int g = 0; g < NTI50GRPS; ++g) {
    int nb = 0, ntme = 0, npptme = 0;
    for (int r = TI50RUNS[g][0]; r <= TI50RUNS[g][1]; ++r) {
      char fname[128];
      sprintf(fname, "~/R15bTME/Ti50/out%05d.root", r);
      TFile* f = new TFile(fname);
      if (!f->IsZombie()) {
        int b = 0;
        TTree* tr = (TTree*)f->Get("TMETree/TMETree");
        tr->SetBranchStatus("*",             false);
        tr->SetBranchStatus("blockId",       true);
        tr->SetBranchStatus("timeToPrevTME", true);
        tr->SetBranchStatus("timeToNextTME", true);
        tr->SetBranchAddress("blockId", &b);
        tr->GetEntry(tr->GetEntries()-1);
        nb     += b;
        ntme   += tr->GetEntries();
        npptme += tr->GetEntries("timeToPrevTME > 10e3 && timeToNextTME > 10e3");
        delete tr;
      }
      delete f;
    }
    nTMETi50[g]           =  ntme;
    nTMETi50[NTI50GRPS]   += ntme;
    nPPTMETi50[g]         =  npptme;
    nPPTMETi50[NTI50GRPS] += npptme;
    nBlTi50[g]            =  nb;
    nBlTi50[NTI50GRPS]    += nb;
    printf("%d-%d:\t%d\t%d\t%d\n",
           TI50RUNS[g][0], TI50RUNS[g][1], nb, ntme, npptme);
  }
  printf("Total Ti50:\t%d\t%d\t%d\n",
         nBlTi50[NTI50GRPS], nTMETi50[NTI50GRPS], nPPTMETi50[NTI50GRPS]);

}
