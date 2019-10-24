{
  TFile* fmw = new TFile("~m-wong/data/R15b/al50.root");
  TTree* trmw = (TTree*)fmw->Get("tree");
  std::set<int>* runs[2] = { new std::set<int>(), new std::set<int>() };
  int run;
  trmw->SetBranchStatus("*",     0);
  trmw->SetBranchStatus("runId", 1);
  trmw->SetBranchAddress("runId", &run);
  for (int i = 0; i < trmw->GetEntries(); ++i) {
    trmw->GetEntry(i);
    if (i%100000 == 0)
      std::cout << "MW: " << i << "/" << trmw->GetEntries() << std::endl;
    runs[0]->insert(run);
  }
  for (int i = 9889; i <= 10128; ++i) {
    char fname[128];
    sprintf(fname, "~/R15bTME/Al50/tme%05d.root", i);
    TFile* f = new TFile(fname);
    if (!f->IsZombie())
      runs[1]->insert(i);
    f->Close();
    delete f;
  }
  if (*runs[0] == *runs[1]) {
    std::cout << "Mark and John use the same run list for Al50!" << std::endl;
  } else {
    for (std::set<int>::const_iterator i = runs[0]->begin(); i != runs[0]->end(); ++i)
      if (runs[1]->count(*i) == 0)
        std::cout << "John missing run " << *i << std::endl;
      for (std::set<int>::const_iterator i = runs[1]->begin(); i != runs[1]->end(); ++i)
        if (runs[0]->count(*i) == 0)
          std::cout << "Mark missing run " << *i << std::endl;
  }
}
