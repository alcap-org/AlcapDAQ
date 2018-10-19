void merge_energy_histograms(unsigned int group_size, const char fdir[], const char dbname[]) {
  const unsigned int ndatasets = 1;
  const char* datasets[ndatasets] = { "Al100" }; //, "Al50b", "Si16P", "SiR2", "Al50awithoutNDet2"};
  unsigned int nfiles[ndatasets];
  char ifname[256], ofname[256], cmd[256], dbcall[256];
  sprintf(dbcall, "sqlite://%s", dbname);
  TSQLiteServer* proddb = new TSQLiteServer(dbcall);
  for (unsigned int i = 0; i < ndatasets; ++i) {
    printf("Dataset: %s\n", datasets[i]);
    TH1 *hAll = NULL, *hPrompt = NULL, *hOOT = NULL, *hFarOOT = NULL, *hTWindow = NULL;
    TH1 *hTotalAll = NULL, *hTotalPrompt = NULL, *hTotalOOT = NULL, *hTotalFarOOT = NULL, *hTotalTWindow = NULL;
    sprintf(cmd, "SELECT run FROM datasets WHERE dataset=='%s'", datasets[i]);
    TSQLiteResult* res = (TSQLiteResult*)proddb->Query(cmd);
    TSQLiteRow* row;
    unsigned int nhists = 0, mhists = 0;
    while ((row = (TSQLiteRow*)res->Next())) {
      if (atoi(row->GetField(0)) == 3257) continue;
      printf("    Histogram %u of histogram %u\n", mhists, nhists);
      sprintf(ifname, "%s/out%05d.root", fdir, atoi(row->GetField(0)));
      TFile* ifile = new TFile(ifname, "READ");
      if (mhists >= group_size) {
       ofile->cd();
       AddEnergyHistograms(hTotalAll, hAll);
       AddEnergyHistograms(hTotalPrompt, hPrompt);
       AddEnergyHistograms(hTotalOOT, hOOT);
       AddEnergyHistograms(hTotalFarOOT, hFarOOT);
       AddEnergyHistograms(hTotalTWindow, hTWindow);
       hAll->Write(); hPrompt->Write(); hOOT->Write(); hFarOOT->Write(); hTWindow->Write();
       ofile->Close();
       ++nhists; mhists = 0;
     }
     if (mhists == 0) {
       sprintf(ofname, "%s/%s_%u.root", fdir, datasets[i], nhists);
       ofile    = new TFile(ofname, "RECREATE");
       hAll     =  (TH1*)ifile->Get("GeSpectrum/hEnergy")       ->Clone("all");
       hPrompt  = ((TH2*)ifile->Get("GeSpectrum/hTimeEnergy"))  ->ProjectionY("prompt");
       hOOT     =  (TH1*)ifile->Get("GeSpectrum/hEnergyOOT")    ->Clone("oot");
       hFarOOT  =  (TH1*)ifile->Get("GeSpectrum/hEnergyFarOOT") ->Clone("faroot");
       hTWindow =  (TH1*)ifile->Get("GeSpectrum/hEnergyTWindow")->Clone("twindow");
       if (!hTotalAll) {
         hTotalAll     = (TH1*)hAll    ->Clone("total_all");     hTotalAll    ->SetDirectory(0); hTotalAll    ->Reset();
         hTotalPrompt  = (TH1*)hPrompt ->Clone("total_prompt");  hTotalPrompt ->SetDirectory(0); hTotalPrompt ->Reset();
         hTotalOOT     = (TH1*)hOOT    ->Clone("total_oot");     hTotalOOT    ->SetDirectory(0); hTotalOOT    ->Reset();
         hTotalFarOOT  = (TH1*)hFarOOT ->Clone("total_faroot");  hTotalFarOOT ->SetDirectory(0); hTotalFarOOT ->Reset();
         hTotalTWindow = (TH1*)hTWindow->Clone("total_twindow"); hTotalTWindow->SetDirectory(0); hTotalTWindow->Reset();
       }
     } else {
       AddEnergyHistograms(hAll,     (TH1*)ifile->Get("GeSpectrum/hEnergy"));
       AddEnergyHistograms(hPrompt, ((TH2*)ifile->Get("GeSpectrum/hTimeEnergy"))->ProjectionY());
       AddEnergyHistograms(hOOT,     (TH1*)ifile->Get("GeSpectrum/hEnergyOOT"));
       AddEnergyHistograms(hFarOOT,  (TH1*)ifile->Get("GeSpectrum/hEnergyFarOOT"));
       AddEnergyHistograms(hTWindow, (TH1*)ifile->Get("GeSpectrum/hEnergyTWindow"));
     }
     delete row;
     ++mhists;
     ifile->Close();
   }
   delete res;
   ofile->cd();
   AddEnergyHistograms(hTotalAll,     hAll);
   AddEnergyHistograms(hTotalPrompt,  hPrompt);
   AddEnergyHistograms(hTotalOOT,     hOOT);
   AddEnergyHistograms(hTotalFarOOT,  hFarOOT);
   AddEnergyHistograms(hTotalTWindow, hTWindow);
   hAll->Write(); hPrompt->Write(); hOOT->Write(); hFarOOT->Write(); hTWindow->Write();
   ofile->Close();
   sprintf(ofname, "%s.root", datasets[i]);
   ofile = new TFile(ofname, "RECREATE");
   ofile->cd();
   hTotalAll->Write(); hTotalPrompt->Write(); hTotalOOT->Write(); hTotalFarOOT->Write(); hTotalTWindow->Write();
   ofile->Close();
 }
}
