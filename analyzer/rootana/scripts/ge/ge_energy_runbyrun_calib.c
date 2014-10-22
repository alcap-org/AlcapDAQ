#include "TH1.h"
#include "TF1.h"
#include "TAxis.h"
#include "TSQLiteServer.h"

void FitParametersFirstGuess(TH1* h, TF1* f, const Double_t sigma) {
  const unsigned int npar = 5;
  Double_t par[npar];

  TAxis* x = h->GetXaxis();
  const Double_t n = x->GetLast() - x->GetFirst() + 1.;

  par[4] = 0.;
  par[2] = sigma;
  par[1] = (Double_t)h->GetMaximumBin();
  h->GetXaxis()->SetRangeUser(par[1] - n/2., par[1] + n/2.);
  par[3] = h->Integral()/n;
  par[0] = h->GetMaximum() - par[3];

  f->SetParameters(par);
  f->SetParLimits(0, 0., h->GetMaximum());
  f->SetParLimits(1, x->GetFirst(), x->GetLast());
  f->SetParLimits(2, 1., 3.*sigma);
  return;
}



ge_energy_runbyrun_calib(const char* mergedb_name) {
  const unsigned int nsets = 7;
  const unsigned int npeaks = 8;
  const char* sets[nsets] = { "Al100", "Al50awithNDet2", "Al50awithoutNDet2", "Al50b", "Si16P", "SiR21pct", "SiR23pct"};
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
  char dbname[256];
  sprintf(dbname, "sqlite://%s", mergedb_name);
  TSQLiteServer* mergedb = new TSQLiteServer(dbname);
  unsigned int nfiles[nsets] = { 0, 0, 0, 0, 0, 0, 0 };
  unsigned int nfiles_tmp = 0;
  for (unsigned int i = 0; i < nsets; ++i) {
    char cmd[128];
    sprintf(cmd, "SELECT COUNT(file) FROM Merge WHERE file LIKE '%s%%'", sets[i]);
    TSQLiteResult* res = (TSQLiteResult*)mergedb->Query(cmd);
    TSQLiteRow* row = (TSQLiteRow*)res->Next();
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
    sprintf(cmd, "SELECT time FROM Merge WHERE file LIKE '%s%%' ORDER BY time ASC", sets[i]);
    TSQLiteResult* res = (TSQLiteResult*)mergedb->Query(cmd);
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      TSQLiteRow* row = (TSQLiteRow*)res->Next();
      time[i][j] = atof(row->GetField(0));
      delete row;
    }
    delete res;
  }

  char fname[64];
  for (unsigned int i = 0; i < nsets; ++i) {
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      sprintf(fname, "%s_%u.root", sets[i], j+1);
      TFile* f = new TFile(fname, "READ");
      TH1* h = (TH1*)f->Get("GeSpectrum/hEnergyFarOOT");
      // Get first guess at shift
      h->GetXaxis()->SetRangeUser(3500., 4500.);
      Double_t shift_511 = (bounds[1][0] + bounds[1][1])/2. - (Double_t)h->GetMaximumBin();
      for (unsigned int k = 0; k < npeaks; ++k) {
	Double_t shift = shift_511*(1.+((bounds[k][0]+bounds[k][1])/2.-4100.)/6000.);
	TF1 fit("fitfunc", "gaus(0)+pol1(3)");
	h->GetXaxis()->SetRangeUser(bounds[k][0] - shift, bounds[k][1] - shift);
	FitParametersFirstGuess(h, &fit, sigma);
	TFitResultPtr fitres = h->Fit(&fit, "SEQM");
	if (!fitres->IsValid()) {
	  printf("Peak: %s(%u)\tStatus: %d\n", fname, k, (Int_t)fitres);
	  return;
	}
	peaks[i][j][k] = fitres->Parameter(1); peaks_err[i][j][k] = fitres->ParError(1);
	sigmas[i][j][k] = fitres->Parameter(2); sigmas_err[i][j][k] = fitres->ParError(2);
	status[i][j][k] = (Int_t)fitres; chi2[i][j][k] = fitres->Chi2(); ndf[i][j][k] = (Double_t) fitres->Ndf();
      }
    }
  }
  TMultiGraph* mg[nsets];
  char grname[64], grtitle[64];
  for (unsigned int i = 0; i < nsets; ++i) {
    sprintf(grname, "mg%s", sets[i]);
    sprintf(grtitle, "Gain (%s)", sets[i]);
    mg[i] = new TMultiGraph(grname, grtitle);
  }
  Double_t gain[nsets][nfiles_max], offset[nsets][nfiles_max], gain_err[nsets][nfiles_max], offset_err[nsets][nfiles_max], chi2_cal[nsets][nfiles_max], ndf_cal[nsets][nfiles_max];
  for (unsigned int i = 0; i < nsets; ++i) {
    printf("--------------------------------------------------------------------------------\n");
    printf("%s\nFile\tGain\tPedestal\tChi2\tNDF\Chi2/NDF\tGain Pct Err\n", sets[i]);
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      TGraphErrors* gr = new TGraphErrors(npeaks, peaks[i][j], en, peaks_err[i][j], en_err);
      TFitResultPtr fitres = gr->Fit("pol1", "SQ");
      printf("%u\t%g\t%g\t%g\t%g\t%g\t%g\n", j, fitres->Value(1), fitres->Value(0), fitres->Chi2(), (Double_t)fitres->Ndf(), fitres->Chi2()/(Double_t)fitres->Ndf(), fitres->ParError(1)/fitres->Value(1));
      mg[i]->Add(gr); gr->GetFunction("pol1")->SetLineColor(j+1); gr->SetMarkerColor(j+1); gr->SetMarkerStyle(1);
      gain[i][j] = fitres->Parameter(1); gain_err[i][j] = fitres->ParError(1);
      offset[i][j] = fitres->Parameter(0); offset_err[i][j] = fitres->ParError(0);
      chi2_cal[i][j] = fitres->Chi2(); ndf_cal[i][j] = (Double_t)fitres->Ndf();
    }
  }
  TMultiGraph* mg_gain = new TMultiGraph("mg_gain", "Gain Drift");
  TLegend* leg_gain = new TLegend(0.7,0.7,0.9,0.9);
  for (unsigned int i = 0; i < nsets; ++i) {
    new TCanvas();
    TGraphErrors* gr = new TGraphErrors(nfiles[i], time[i], gain[i], 0x0, gain_err[i]);
    mg_gain->Add(gr); gr->SetMarkerColor(i+1); gr->SetMarkerStyle(1);
    mg[i]->Draw("A*");
    leg_gain->AddEntry(gr, sets[i]);
  }
  new TCanvas();
  mg_gain->Draw("A*");
  mg_gain->GetXaxis()->SetTitle("Unix Time (s)");
  mg_gain->GetYaxis()->SetTitle("Gain (E=Gain*ADC+Offset)");
  leg_gain->Draw("SAME");

  for (unsigned int i = 0; i < nsets; ++i) {
    for (unsigned int j = 0; j < nfiles[i]; ++j) {
      char cmd[128];
      sprintf(cmd, "SELECT runs FROM Merge WHERE file=='%s_%u'", sets[i], j+1);
      TSQLiteResult* res = (TSQLiteResult*)mergedb->Query(cmd);
      TSQLiteRow* row = (TSQLiteRow*)res->Next();
      std::stringstream ss(row->GetField(0));
      delete res; delete row;
      while (ss.good()) {
	char ofname[128];
	unsigned int run;
	ss >> run;
	sprintf(ofname, "calib.run%05u.Energy.csv", run);
	std::ofstream ofile(ofname);
	ofile << "run,channel,gain,gain_err,offset,offset_err,chi2,ndf" << std::endl;
	ofile << run << ",Ge-S," << gain[i][j] << ',' << gain_err[i][j] << ','
	      << offset[i][j] << ',' << offset_err[i][j] << ','
	      << chi2_cal[i][j] << ',' << ndf_cal[i][j] << std::endl;
	ofile.close();
      }
    }
  }
}
