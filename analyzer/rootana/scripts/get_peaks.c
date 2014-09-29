{
  const char* files[] = { "add1.root",  "add2.root",  "add3.root",  "add4.root",  "add5.root",
			  "add6.root",  "add7.root",  "add8.root",  "add9.root",  "add10.root",
			  "add11.root", "add12.root", "add13.root", "add14.root", "add15.root",
			  "add16.root", "add17.root", "add18.root", "add19.root", "add20.root",
			  "add21.root", "add22.root", "add23.root", "add24.root", "add25.root",
			  "add26.root", "add27.root", "add28.root", "add29.root", "add30.root" };
  const unsigned int nfiles = 30;
  const unsigned int npeaks = 6;
  const Double_t peak_search[npeaks] = { 1453., 1325., 967., 933., 606., 508.};
  const Double_t search_window = 20.;
  const Double_t sigma = 1.;
  Double_t peaks[nfiles][npeaks];
  Double_t peaks_err[nfiles][npeaks];
  unsigned int t[nfiles] = { 1387604374, 1387606953, 1387609839, 1387612731, 1387615575,
			     1387618372, 1387621403, 1387624574, 1387627327, 1387630073,
			     1387633340, 1387636613, 1387639373, 1387642307, 1387645236,
			     1387648020, 1387650798, 1387662449, 1387667155, 1387669933,
			     1387672708, 1387675469, 1387678264, 1387681099, 1387683923,
			     1387686724, 1387689536, 1387692364, 1387697564, 1387700883 }; // Seconds since Unix epoch
  const unsigned int first_run_start = 1387603224;
  const double total_livetime = 104090.;
  for (unsigned int i = 0; i < nfiles; ++i)
    t[i] -= first_run_start;

  // Get Peaks
  TF1* func = new TF1("pkfunc", "gaus(0)+[3]*x+[4]", 400., 1600.);
  for (unsigned int i = 0; i < nfiles; ++i) {
    printf("File %d\n", i+1);
    char path[256];
    TFile f(strcat(strcpy(path, "/home/jrquirk/tmp/ge/out/v37/"), files[i]), "READ");
    TH1* h = (TH1*)f.Get("GeSpectrum/hEnergy");
    for (unsigned int j = 0; j < npeaks; ++j) {
      h->GetXaxis()->SetRangeUser(peak_search[j] - search_window, peak_search[j] + search_window);
      func->SetParameters(h->GetBinContent(h->FindBin(peak_search[j])),
			  peak_search[j],
			  sigma,
			  0.,
			  (h->GetBinContent(h->FindBin(peak_search[j] - 20.)) + h->GetBinContent(h->FindBin(peak_search[j] + 20.)))/2.);
      TFitResultPtr res = h->Fit(func, "SNQ");
      peaks[i][j] = func->GetParameter(1);
      peaks_err[i][j] = res->ParError(1);
    }
  }

  // Save peak positions
  FILE* ofile = fopen("peaks.csv", "w");
  for (unsigned int i = 0; i < nfiles; ++i) {
    fprintf(ofile, "%u ", t[i]);
    for (unsigned int j = 0; j < npeaks; ++j)
      fprintf(ofile, " %g %g", peaks[i][j], peaks_err[i][j]);
    fprintf(ofile, "\n");
  }
  fclose(ofile);

  // Do fits
  TLegend* leg = new TLegend(0.2, 0.53, 0.9, 0.755, "Gain Drifts");
  TLegend* leg_norm = new TLegend(0.2, 0.53, 0.9, 0.755, "Gain Drifts");
  TMultiGraph* mg = new TMultiGraph("peakpos", "Germanium Peaks");
  TMultiGraph* mg_norm = new TMultiGraph("peakpos_norm", "Germanium Peaks (Normalized)");
  for (unsigned int i = 0; i < npeaks; ++i) {
    char fmt[256]; memset(fmt, '\0', 256); strcat(fmt, "%lg\0");
    char lab[256]; memset(lab, '\0', 256);
    for (unsigned int j = 0; j < npeaks; ++j) {
      if (j==i)
	strcat(fmt, " %lg %lg");
      else
	strcat(fmt, " %*lg %*lg");
    }
    TGraphErrors* gr = new TGraphErrors("peaks.csv", fmt);
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(21);
    gr->SetFillStyle(0);
    TGraphErrors* gr_norm = (TGraphErrors*)gr->Clone();
    double norm = gr_norm->GetY()[0];
    /* for (unsigned int j = 0; j < (unsigned int)gr_norm->GetN(); ++j) { */
    /*   gr_norm->GetY()[j] /= norm; */
    /*   gr_norm->GetEY()[j] /= norm; */
    /* } */
    TFitResultPtr fit = gr->Fit("pol1", "S", "AL");
    sprintf(lab, "%u keV: %2f +/- %2f eV/s (%3f eV Total)", (unsigned int)peak_search[i], fit->Value(1)*1000., fit->ParError(1)*1000., total_livetime*fit->Value(1)*1000.);
    leg->AddEntry(gr, lab);
    fit = gr_norm->Fit("pol1", "S", "AL");
    sprintf(lab, "%u keV", (unsigned int)peak_search[i]);
    leg_norm->AddEntry(gr_norm, lab);
    mg->Add(gr);
    mg_norm->Add(gr_norm);
  }

  // Draw
  new TCanvas();
  leg->SetTextSize(0.03);
  mg->Draw("AP");
  mg->GetXaxis()->SetTitle("Time since start of first run (s)");
  mg->GetYaxis()->SetTitle("Peak Position (keV)");
  leg->Draw("SAME");
  new TCanvas();
  leg_norm->SetTextSize(0.03);
  mg_norm->Draw("AP");
  mg_norm->GetXaxis()->SetTitle("Time since start of first run (s)");
  mg->GetYaxis()->SetTitle("Normalized Peak Position (keV)");
}
