void merge_energy_histograms(unsigned int group_size, const char fdir[], const char dbname[]) {
  const unsigned int ndatasets = 8;
  const char* datasets[ndatasets] = { "Al50b", "Al100", "Si16P", "SiR2", "SiR21pct", "SiR23pct", "Al50awithNDet2", "Al50awithoutNDet2"};
  unsigned int nfiles[ndatasets];
  char ifname[256], ofname[256], cmd[256], dbcall[256];
  sprintf(dbcall, "sqlite://%s", dbname);
  TSQLiteServer* proddb = new TSQLiteServer(dbcall);
  for (unsigned int i = 0; i < ndatasets; ++i) {
    printf("Dataset: %s\n", datasets[i]);
    TH1 *hAll = NULL, *hPrompt = NULL, *hOOT = NULL, *hEnter = NULL, *hLT = NULL;
    TH1 *hPPAll = NULL, *hPPPrompt = NULL, *hPPOOT = NULL, *hPPEnter = NULL, *hPPLT = NULL;
    TH1 *hTotalAll = NULL, *hTotalPrompt = NULL, *hTotalOOT = NULL, *hTotalEnter = NULL, *hTotalLT = NULL;
    TH1 *hPPTotalAll = NULL, *hPPTotalPrompt = NULL, *hPPTotalOOT = NULL, *hPPTotalEnter = NULL, *hPPTotalLT = NULL;
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
	hTotalEnter->Add(hEnter);
	hTotalLT->Add(hLT);
	AddEnergyHistograms(hPPTotalAll, hPPAll);
	AddEnergyHistograms(hPPTotalPrompt, hPPPrompt);
	AddEnergyHistograms(hPPTotalOOT, hPPOOT);
	hPPTotalEnter->Add(hPPEnter);
	hPPTotalLT->Add(hPPLT);
	hAll->Write(); hPrompt->Write(); hOOT->Write(); hEnter->Write(); hLT->Write();
	hPPAll->Write(); hPPPrompt->Write(); hPPOOT->Write(); hPPEnter->Write(); hPPLT->Write();
	ofile->Close();
	++nhists; mhists = 0;
      }
      if (mhists == 0) {
	sprintf(ofname, "%s/%s_%u.root", fdir, datasets[i], nhists);
	ofile = new TFile(ofname, "RECREATE");
	hAll = (TH1*)ifile->Get("GeSpectrum/hEnergy")->Clone("all");
	hPrompt = ((TH2*)ifile->Get("GeSpectrum/hTimeEnergy"))->ProjectionY("prompt");
	hOOT = (TH1*)ifile->Get("GeSpectrum/hEnergyFarOOT")->Clone("oot");
	hEnter = (TH1*)ifile->Get("GeSpectrum/hNMuons")->Clone("enter");
	hLT = (TH1*)ifile->Get("GeSpectrum/hLivetime")->Clone("livetime");
	hPPAll = (TH1*)ifile->Get("GeSpectrum/hPPEnergy")->Clone("pp_all");
	hPPPrompt = ((TH2*)ifile->Get("GeSpectrum/hPPTimeEnergy"))->ProjectionY("pp_prompt");
	hPPOOT = (TH1*)ifile->Get("GeSpectrum/hPPEnergyFarOOT")->Clone("pp_oot");
	hPPEnter = (TH1*)ifile->Get("GeSpectrum/hPPNMuons")->Clone("pp_enter");
	hPPLT = (TH1*)ifile->Get("GeSpectrum/hPPLivetime")->Clone("pp_livetime");
	if (!hTotalAll) {
	  hTotalAll = (TH1*)hAll->Clone("total_all"); hTotalAll->SetDirectory(0); hTotalAll->Reset();
	  hTotalPrompt = (TH1*)hPrompt->Clone("total_prompt"); hTotalPrompt->SetDirectory(0); hTotalPrompt->Reset();
	  hTotalOOT = (TH1*)hOOT->Clone("total_oot"); hTotalOOT->SetDirectory(0); hTotalOOT->Reset();
	  hTotalEnter = (TH1*)hEnter->Clone("total_enter"); hTotalEnter->SetDirectory(0); hTotalEnter->Reset();
	  hTotalLT = (TH1*)hLT->Clone("total_livetime"); hTotalLT->SetDirectory(0); hTotalLT->Reset();
	  hPPTotalAll = (TH1*)hAll->Clone("pp_total_all"); hPPTotalAll->SetDirectory(0); hPPTotalAll->Reset();
	  hPPTotalPrompt = (TH1*)hPrompt->Clone("pp_total_prompt"); hPPTotalPrompt->SetDirectory(0); hPPTotalPrompt->Reset();
	  hPPTotalOOT = (TH1*)hOOT->Clone("pp_total_oot"); hPPTotalOOT->SetDirectory(0); hPPTotalOOT->Reset();
	  hPPTotalEnter = (TH1*)hEnter->Clone("pp_total_enter"); hPPTotalEnter->SetDirectory(0); hPPTotalEnter->Reset();
	  hPPTotalLT = (TH1*)hLT->Clone("pp_total_livetime"); hPPTotalLT->SetDirectory(0); hPPTotalLT->Reset();
	}
      } else {
	AddEnergyHistograms(hAll, (TH1*)ifile->Get("GeSpectrum/hEnergy"));
	AddEnergyHistograms(hPrompt, ((TH2*)ifile->Get("GeSpectrum/hTimeEnergy"))->ProjectionY());
	AddEnergyHistograms(hOOT, (TH1*)ifile->Get("GeSpectrum/hEnergyFarOOT"));
	hEnter->Add((TH1*)ifile->Get("GeSpectrum/hNMuons"));
	hLT->Add((TH1*)ifile->Get("GeSpectrum/hLivetime"));
	AddEnergyHistograms(hPPAll, (TH1*)ifile->Get("GeSpectrum/hPPEnergy"));
	AddEnergyHistograms(hPPPrompt, ((TH2*)ifile->Get("GeSpectrum/hPPTimeEnergy"))->ProjectionY());
	AddEnergyHistograms(hPPOOT, (TH1*)ifile->Get("GeSpectrum/hPPEnergyFarOOT"));
	hPPEnter->Add((TH1*)ifile->Get("GeSpectrum/hPPNMuons"));
	hPPLT->Add((TH1*)ifile->Get("GeSpectrum/hPPLivetime"));
      }
      delete row;
      ++mhists;
      ifile->Close();
    }
    delete res;
    ofile->cd();
    hAll->Write(); hPrompt->Write(); hOOT->Write(); hEnter->Write(); hLT->Write();
    hPPAll->Write(); hPPPrompt->Write(); hPPOOT->Write(); hPPEnter->Write(); hPPLT->Write();
    ofile->Close();
    sprintf(ofname, "%s.root", datasets[i]);
    ofile = new TFile(ofname, "RECREATE");
    ofile->cd();
    hTotalAll->Write(); hTotalPrompt->Write(); hTotalOOT->Write(); hTotalEnter->Write(); hTotalLT->Write();
    hPPTotalAll->Write(); hPPTotalPrompt->Write(); hPPTotalOOT->Write(); hPPTotalEnter->Write(); hPPTotalLT->Write();
    ofile->Close();
  }
}
