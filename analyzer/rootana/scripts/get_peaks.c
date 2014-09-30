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
  Double_t sig[nfiles][npeaks];
  Double_t peaks_err[nfiles][npeaks];
  Double_t sig_err[nfiles][npeaks];
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
      peaks_err[i][j] = func->GetParError(1);
      sig[i][j] = func->GetParameter(2);
      sig_err[i][j] = func->GetParError(2);
    }
  }

  // Save peak positions
  FILE* ofile = fopen("peaks.csv", "w");
  fprintf(ofile, "Time    Peak    +/-Peak    Sigma    +/-Sigma");
  for (unsigned int i = 0; i < nfiles; ++i) {
    fprintf(ofile, "%u ", t[i]);
    for (unsigned int j = 0; j < npeaks; ++j)
      fprintf(ofile, " %5g %5.5g %5g %5.5g", peaks[i][j], peaks_err[i][j], sig[i][j], sig_err[i][j]);
    fprintf(ofile, "\n");
  }
  fclose(ofile);

  // Do fits
  TLegend* leg = new TLegend(0.2, 0.53, 0.9, 0.755, "Gain Drifts");
  TLegend* leg_norm = new TLegend(0.6, 0.1, 0.8, 0.4, "Gain Drifts");
  TLegend* leg_shift = new TLegend(0.6, 0.1, 0.8, 0.4, "Gain Drifts");
  TLegend* leg_sig = new TLegend(0.5, 0.1, 0.7, 0.4, "Sigma Drifts");
  TLegend* leg_res = new TLegend(0.5, 0.1, 0.7, 0.4, "Resolution Drifts");
  TMultiGraph* mg = new TMultiGraph("peakpos", "Germanium Peaks");
  TMultiGraph* mg_norm = new TMultiGraph("peakpos_norm", "Germanium Peaks (Normalized)");
  TMultiGraph* mg_shift = new TMultiGraph("peakpos_shift", "Germanium Peaks (Shifted)");
  TMultiGraph* mg_sig = new TMultiGraph("peaksigma", "Germanium Peak Sigma");
  TMultiGraph* mg_res = new TMultiGraph("peakresolution", "Germanium Peak Resolution");
  for (unsigned int i = 0; i < npeaks; ++i) {
    char fmt_pk[256]; memset(fmt_pk, '\0', 256); strcat(fmt_pk, "%lg");
    char fmt_sig[256]; memset(fmt_sig, '\0', 256); strcat(fmt_sig, "%lg");
    char lab[256]; memset(lab, '\0', 256);
    for (unsigned int j = 0; j < npeaks; ++j) {
      if (j==i) {
	strcat(fmt_pk, " %lg %lg %*lg %*lg");
	strcat(fmt_sig, " %*lg %*lg %lg %lg");
      } else {
	strcat(fmt_pk, " %*lg %*lg %*lg %*lg");
	strcat(fmt_sig, " %*lg %*lg %*lg %*lg");
      }
    }
    TGraphErrors* gr = new TGraphErrors("peaks.csv", fmt_pk);
    TGraphErrors* gr_norm = (TGraphErrors*)gr->Clone();
    TGraphErrors* gr_shift = (TGraphErrors*)gr->Clone();
    TGraphErrors* gr_sig = new TGraphErrors("peaks.csv", fmt_sig);
    TGraphErrors* gr_res = (TGraphErrors*)gr_sig->Clone();
    Double_t norm = gr_norm->GetY()[0];
    for (unsigned int j = 0; j < gr_norm->GetN(); ++j) {
      gr_norm->GetY()[j] = gr_norm->GetY()[j] / norm;
      gr_norm->GetEY()[j] = gr_norm->GetEY()[j] / norm;
      gr_shift->GetY()[j] = gr_shift->GetY()[j] - norm;
      gr_res->GetY()[j] = gr_res->GetY()[j] / peak_search[i];
      gr_res->GetEY()[j] = gr_res->GetEY()[j] / peak_search[i];
    }

    TFitResultPtr fit = gr->Fit("pol1", "SQ", "AL");
    gr_norm->Fit("pol1", "SQ", "AL");
    gr_shift->Fit("pol1", "SQ", "AL");
    sprintf(lab, "%u keV: %2f +/- %2f eV/s (%3f eV Total)", (unsigned int)peak_search[i], fit->Value(1)*1000., fit->ParError(1)*1000., total_livetime*fit->Value(1)*1000.);
    leg->AddEntry(gr, lab);
    sprintf(lab, "%u keV", (unsigned int)peak_search[i]);
    leg_norm->AddEntry(gr_norm, lab);
    leg_shift->AddEntry(gr_shift, lab);
    fit = gr_sig->Fit("pol1", "SQ");
    gr_res->Fit("pol1", "SQ");
    sprintf(lab, "%u keV: %2f +/- %2f eV/s", (unsigned int)peak_search[i], fit->Value(1)*1000., fit->ParError(1)*1000.);
    leg_sig->AddEntry(gr_sig, lab);
    sprintf(lab, "%u keV", (unsigned int)peak_search[i]);
    leg_res->AddEntry(gr_res, lab);

    gr->SetMarkerColor(i+1);                    gr_norm->SetMarkerColor(i+1);                    gr_shift->SetMarkerColor(i+1);
    gr->SetMarkerStyle(21);                     gr_norm->SetMarkerStyle(21);                     gr_shift->SetMarkerStyle(21);
    gr->SetFillStyle(0);                        gr_norm->SetFillStyle(0);                        gr_shift->SetFillStyle(0);
    gr->GetFunction("pol1")->SetLineColor(i+1); gr_norm->GetFunction("pol1")->SetLineColor(i+1); gr_shift->GetFunction("pol1")->SetLineColor(i+1);
    gr_sig->SetMarkerColor(i+1);                    gr_res->SetMarkerColor(i+1);
    gr_sig->SetMarkerStyle(21);                     gr_res->SetMarkerStyle(21);
    gr_sig->SetFillStyle(0);                        gr_res->SetFillStyle(0);
    gr_sig->GetFunction("pol1")->SetLineColor(i+1); gr_res->GetFunction("pol1")->SetLineColor(i+1);
    mg->Add(gr);
    mg_norm->Add(gr_norm);
    mg_shift->Add(gr_shift);
    mg_sig->Add(gr_sig);
    mg_res->Add(gr_res);
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
  mg_norm->GetYaxis()->SetTitle("Normalized Peak Position (keV)");
  leg_norm->Draw("SAME");
  new TCanvas();
  leg_shift->SetTextSize(0.03);
  mg_shift->Draw("AP");
  mg_shift->GetXaxis()->SetTitle("Time since start of first run (s)");
  mg_shift->GetYaxis()->SetTitle("Shifted Peak Position (keV)");
  leg_shift->Draw("SAME");
  new TCanvas();
  leg_sig->SetTextSize(0.03);
  mg_sig->Draw("AP");
  mg_sig->GetXaxis()->SetTitle("Time since start of first run (s)");
  mg_sig->GetYaxis()->SetTitle("#sigma_{E}");
  leg_sig->Draw("SAME");
  new TCanvas();
  leg_res->SetTextSize(0.03);
  mg_res->Draw("AP");
  mg_res->GetXaxis()->SetTitle("Time since start of first run (s)");
  mg_res->GetYaxis()->SetTitle("#sigma_{E}/E");
  leg_res->Draw("SAME");
}
