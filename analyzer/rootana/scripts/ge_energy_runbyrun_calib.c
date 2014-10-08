void FitParametersFirstGuess(const TH1* h, const Double_t nbins, const Double_t sigma, Double_t par[5]) {
  par[4] = 0.;
  par[3] = h->Integral()/nbins;
  par[2] = sigma;
  par[1] = (Double_t)h->GetMaximumBin();
  par[0] = h->GetMaximum() - par[3];
}

ge_energy_runbyrun_calib() {
  const unsigned int nsets = 1;//7;
  const unsigned int npeaks = 8;
  const char* sets[nsets] = { "Al100" }; // , "Al50awithNDet2", "Al50awithoutNDet2", "Al50b", "Si16P", "SiR21pct", "SiR23pct" };
  const Double_t en[npeaks] =     { 351.932, 510.998928, 583.191, 609.312, 911.204, 1173.237, 1332.501, 1460.83 };
  const Double_t en_err[npeaks] = { 0.002,   0.000011,   0.002,   0.007,   0.004,   0.004,    0.005,    0.01 };
  const Double_t sigma = 8.;
  const Double_t nbins = 100.;
  const Double_t bounds[npeaks][2] = { { 2800.,  2800. + nbins },
				       { 4100.,  4100. + nbins },
				       { 4700.,  4700. + nbins },
				       { 4900.,  4900. + nbins },
				       { 7360.,  7360. + nbins },
				       { 9500.,  9500. + nbins },
				       { 10800., 10800.+ nbins },
				       { 11850., 11850. + nbins } };
  TSQLiteServer* db = new TSQLiteServer("sqlite://merge.db");
  unsigned int nfiles[nsets] = { 0 };//, 0, 0, 0, 0, 0, 0 };
  unsigned int nfiles_tmp = 0;
  for (unsigned int i = 0; i < nsets; ++i) {
    char cmd[128];
    sprintf(cmd, "SELECT COUNT(file) FROM Merge WHERE file LIKE '%s%%'", sets[i]);
    TSQLiteResult* res = (TSQLiteResult*)db->Query(cmd);
    TSQLiteRow* row = res->Next();
    nfiles[i] = (unsigned int)atoi(row->GetField(0));
    if (nfiles[i] > nfiles_tmp) nfiles_tmp = nfiles[i];
    delete row;
    delete res;
  }
  const unsigned int nfiles_max = nfiles_tmp;
  Double_t time[nsets][nfiles_max];
  Double_t peaks[nsets][nfiles_max][npeaks], sigmas[nsets][nfiles_max][npeaks], peaks_err[nsets][nfiles_max][npeaks], sigmas_err[nsets][nfiles_max][npeaks];
  Double_t chi2[nsets][nfiles_max][npeaks], ndf[nsets][nfiles_max][npeaks];
  Int_t status[nsets][nfiles_max][npeaks];
  for (unsigned int i = 0; i < nsets; ++i) {
    char cmd[128];
    sprintf(cmd, "SELECT time FROM Merge WHERE file LIKE '%s%%'", sets[i]);
    TSQLiteResult* res = (TSQLiteResult*)db->Query(cmd);
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      TSQLiteRow* row = (TSQLiteRow*)res->Next();
      time[i][j] = atof(row->GetField(0));
      delete row;
    }
    delete res;
  }

  //  TF1* fit = new TF1("fitfunc", "gaus(0)+pol1(3)");
  char fname[64];
  for (unsigned int i = 0; i < nsets; ++i) {
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      sprintf(fname, "%s_%u.root", sets[i], j+1);
      printf("%s\n", fname);
      TFile* f = new TFile(fname, "READ");
      TH1* h = (TH1*)f->Get("GeSpectrum/hEnergyFarOOT");
      for (unsigned int k = 0; k < npeaks; ++k) {
	TF1 fit("fitfunc", "gaus(0)+pol1(3)");
	h->GetXaxis()->SetRangeUser(bounds[k][0], bounds[k][1]);
	Double_t par[5];
	FitParametersFirstGuess(h, nbins, sigma, par);
	fit.SetParameters(par);//fit->SetParameters(par);
	fit.SetParLimits(0, 0., par[0]+par[1]);
	fit.SetParLimits(1, bounds[k][0], bounds[k][1]);
	fit.SetParLimits(2, 0., 20.);
	printf("%g %g %g %g %g\n", par[0], par[1], par[2], par[3], par[4]);
	TFitResultPtr fitres = h->Fit(&fit, "SE");//TFitResultPtr fitres = h->Fit(fit, "SE");
	printf("Fit result status:\t%d\t%d\n", (Int_t)fitres, fitres->Status());
	if ((Int_t)fitres) {
	  return;
	}
	peaks[i][j][k] = fitres->Parameter(1); peaks_err[i][j][k] = fitres->ParError(1);
	sigmas[i][j][k] = fitres->Parameter(2); sigmas_err[i][j][k] = fitres->ParError(2);
	status[i][j][k] = (Int_t)fitres; chi2[i][j][k] = fitres->Chi2(); ndf[i][j][k] = fitres->Ndf();
      }
    }
  }
  TMultiGraph* mg[nsets];
  char grname[64], grtitle[64];
  for (unsigned int i = 0; i < nsets; ++i) {
    sprintf(grname, "mg%s", sets[i]);
    sprintf(grtitle, "Gain (%s)", sets[i]);
    mg[i] = new TMultiGtaph(grname, grtitle);
  }
  Double_t gain[nsets][nfiles_max], offset[nsets][nfiles_max], gain_err[nsets][nfiles_max], offset_err[nsets][nfiles_max];
  for (unsigned int i = 0; i < nsets; ++i) {
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      TGraphErrors* gr = new TGraphErrors(npeaks, peaks[i][j], en, peaks_err[i][j], en_err);
      TFitResultPtr fitres = gr->Fit("pol1", "SQ0");
      mg[i]->Add(gr);
      gain[i][j] = fitres->Parameter(1); gain_err[i][j] = fitres->ParError(1);
      offset[i][j] = fitres->Parameter(0); offset_err[i][j] = fitres->ParError(0);
    }
  }
  
  for (unsigned int i = 0; i < nsets; ++i) {
    new TCanvas();
    mg[i]->Draw("A*");
  }
  /* TGraphErrors* gr = new TGraphErrors(nfiles, time, gain, 0, gain_err); */
  /* new TCanvas(); */
  /* gr->Fit("pol1"); */
  /* gr->Draw("A*"); */
}
