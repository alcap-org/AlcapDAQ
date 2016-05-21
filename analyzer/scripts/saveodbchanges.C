{
  TFile ifile("odbtrends.root");
  std::ofstream ofile1("odbchanges.txt");
  std::ofstream ofile2("odbchanges_unique.txt");
  TTree* tr = (TTree*)ifile->Get("odbtrend");
  int nbr = tr->GetNbranches();
  int nev = tr->GetEntries();
  TObjArray* branches = tr->GetListOfBranches();
  Int_t oldval[nbr];
  Int_t newval[nbr];
  Int_t run;
  
  for (int ibr = 0; ibr < nbr; ++ibr)
    ((TBranch*)branches->At(ibr))->SetAddress(&newval[ibr]);
  tr->GetEntry(0);
  for (int ibr = 0; ibr < nbr; ++ibr)
    oldval[ibr] = newval[ibr];
  for (int iev = 1; iev < nev; ++iev) {
    std::cout << "Entry: " << iev << std::endl;
    tr->GetEntry(iev);
    bool runchange = false;
    for (int ibr = 1; ibr < nbr; ++ibr) {
      if (oldval[ibr] != newval[ibr]) {
	runchange = true;
	ofile1 << newval[0] << " "
	      << branches->At(ibr)->GetName() << std::endl;
      }
      oldval[ibr] = newval[ibr];
    }
    if (runchange) ofile2 << newval[0] << std::endl;
  }
  ofile1.close();
  ofile2.close();
}
