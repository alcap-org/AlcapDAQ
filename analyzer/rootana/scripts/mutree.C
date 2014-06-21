{
  gROOT->ProcessLine(".L /home/nam/work/AlcapDAQ/analyzer/rootana/libAnalysis.so");
  gStyle->SetPalette(55);

  TString data_path = "/home/nam/work/RunPSI2013/data/root/dq3_rootanahist/";
  int firstRun = 2091;
  int lastRun = 2095;
  int runNo = firstRun;

  TChain *chain = new TChain("MuEvt/mutree");
  for (runNo = 0; runNo <= lastRun; ++runNo)
  {
    TString data_file = data_path + Form("ranahist%.5d.root", runNo);
    if (gSystem->AccessPathName(data_file))
      continue;

    chain->Add(data_file);
  }

  //chain->Draw("E_SiR2_S:(t_SiR2_S - t_muSc)>>h2(1024, -2000, 12000, 512, 0, 2500)",
     //"", "colz");
  //chain->Draw("E_SiR2_F:(t_SiR2_F - t_muSc)>>h2(1024, -2000, 12000, 512, 0, 2500)",
     //"", "colz");

  //double E_muSc, t_muSc;
  //std::vector<double> *E_SiR2_S, *t_SiR2_S, *E_SiR2_F, *t_SiR2_F;
  //chain->SetBranchAddress("E_muSc", &E_muSc);
  //chain->SetBranchAddress("t_muSc", &t_muSc);
  //chain->SetBranchAddress("E_SiR2_S", &E_SiR2_S);
  //chain->SetBranchAddress("t_SiR2_S", &t_SiR2_S);
  //chain->SetBranchAddress("E_SiR2_F", &E_SiR2_F);
  //chain->SetBranchAddress("t_SiR2_F", &t_SiR2_F);

  //TH2F * hSiR2S_muSc = new TH2F("hSiR2S_muSc", "hSiR2S_muSc", 1024, -4e3, 13e3,
      //512, 0, 2500);
  //TH2F * hSiR2F_muSc = new TH2F("hSiR2F_muSc", "hSiR2F_muSc", 1024, -4e3, 13e3,
      //512, 0, 2500);

  //long int nentries = chain->GetEntries();
  //nentries = 1;
  //for (int i = 0; i < nentries; ++i)
  //{
    //if (i%(int)(1e7) == 1)
      //std::cout<<i<<" events out of "<<nentries<<std::endl;

    //chain->GetEntry(i);
    //for (unsigned int j = 0; j < E_SiR2_S->size(); ++j)
      //hSiR2S_muSc->Fill(t_SiR2_S->at(j) - t_muSc, E_SiR2_S->at(j));

    //for (unsigned int j = 0; j < E_SiR2_F->size(); ++j)
      //hSiR2F_muSc->Fill(t_SiR2_F->at(j) - t_muSc, E_SiR2_F->at(j));
  //}
  //hSiR2S_muSc->Draw("colz");
}
