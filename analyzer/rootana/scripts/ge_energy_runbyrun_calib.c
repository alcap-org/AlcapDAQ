{
  const unsigned int nsets = 6;
  const unsigned int npeaks = 8;
  const char* sets[nsets] = { "Al100", "Al50awithNDet2", "Al50awithoutNDet2", "Al50b", "Si16P", "SiR21pct", "SiR23pct" };
  const Double_t en[npeaks] =     { 351.932, 510.998928, 583.191, 609.312, 911.204, 1173.237, 1332.501, 1460.83 };
  const Double_t en_err[npeaks] = { 0.002,   0.000011,   0.002,   0.007,   0.004,   0.004,    0.005,    0.01 };
  const Double_t par[npeaks][5] = { { 35.,  2860.,  8.,  45., 0. },
				    { 430., 4150.,  10., 20., 0. },
				    { 20.,  4740.,  8.,  10., 0. },
				    { 30.,  4955.,  8.,  10., 0. },
				    { 15.,  7415.,  8.,  5.,  0. },
				    { 12.,  9550.,  8.,  4.,  0.},
				    { 12.,  10850., 8.,  2.,  0.},
				    { 33.,  11895., 8.,  2.,  0.} };
  const Double_t bounds[npeaks][2] = { { 2800.,  2900. },
				       { 4100.,  4200. },
				       { 4700.,  4800. },
				       { 4900.,  5000. },
				       { 7360.,  7460. },
				       { 9500.,  9600. },
				       { 10800., 10900.},
				       { 11850., 11950.} };
  const Double_t time[nfiles] = { 1387604653.92, 1387607518.98, 1387610414.26, 1387613304.14, 1387616138.98,
				  1387618927.99,  1387622098.87, 1387625126.46, 1387627879.8, 1387631140.66,
				  1387634404.95, 1387637159.86, 1387639919.46, 1387642858.41, 1387645790.1,
				  1387648568.37, 1387651559.9, 1387664920.0, 1387667710.98, 1387670485.14,
				  1387673258.58, 1387676026.6, 1387678825.48, 1387681660.52, 1387684482.86,
				  1387687285.58, 1387690100.48, 1387692815.51, 1387697785.09, 1387700882.8,
				  1387704520.53 };
  TSQLiteServer* db = new TSQLiteServer("sqlite://merge.db");
  char cmd[128];
  unsigned int nfiles[nsets] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  for (unsigned int i = 0; i < nsets; ++i) {
    sprintf(cmd, "SELECT COUNT(file) WHERE file LIKE '%s%%'", sets[i]);
    TSQLiteResult* res = con->Query(cmd);
    TSQLiteRow* row = res->Next();
    nfiles[i] = (unsigned int)atoi(row->GetField(0));
    delete row, res;
  }

  Double_t peaks[nsets][nfiles][npeaks], sigmas[nsets][nfiles][npeaks], peaks_err[nsets][nfiles][npeaks], sigmas_err[nsets][nfiles][npeaks];
  Double_t chi2[nsets][nfiles][npeaks], ndf[nsets][nfiles][npeaks];
  Int_t status[nsets][nfiles][npeaks];

  TF1* fit = new TF1("fitfunc", "gaus(0)+pol1(3)");
  char fname[64];
  for (unsigned int i = 0; i < nsets; ++i) {
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      sprintf(fname, "%s_%u.root", sets[i], j+1);
      printf("%s\n", fname);
      TFile* f = new TFile(fname, "READ");
      TH1* h = (TH1*)f->Get("GeSpectrum/hEnergyFarOOT");
      for (unsigned int k = 0; k < npeaks; ++k) {
	h->GetXaxis()->SetRangeUser(bounds[k][0], bounds[k][1]);
	fit->SetParameters(par[j]);
	TFitResultPtr res = h->Fit(fit, "QSN");
	peaks[i][j] = res->Parameter(1); peaks_err[i][j] = res->ParError(1);
	sigmas[i][j] = res->Parameter(2); sigmas_err[i][j] = res->ParError(2);
	status[i][j] = (Int_t)res; chi2[i][j] = res->Chi2(); ndf[i][j] = res->Ndf();
      }
    }
  }
  TMultiGraph* mg = new TMultiGraph("lines", "Energy Calibration");
  char grname[256];
  Double_t gain[nfiles], offset[nfiles], gain_err[nfiles], offset_err[nfiles];
  for (unsigned int i = 0; i < nfiles; ++i) {
    sprintf(grname, "gr%u", i+1);
    TGraphErrors* gr = new TGraphErrors(npeaks, peaks[i], en, peaks_err[i], en_err);
    TFitResultPtr res = gr->Fit("pol1", "SQ");
    TF1* fnc = gr->GetFunction("pol1");
    mg->Add(gr);
    gain[i] = res->Parameter(1); gain_err[i] = res->ParError(1);
  }
  mg->Draw("A*");
  TGraphErrors* gr = new TGraphErrors(nfiles, time, gain, 0, gain_err);
  new TCanvas();
  gr->Fit("pol1");
  gr->Draw("A*");
}
