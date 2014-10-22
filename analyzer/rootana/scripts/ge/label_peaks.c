{
  const unsigned int npeaks = 6;
  const Double_t peak_search[npeaks] = { 1453., 1325., 967., 933., 606., 508.};
  const TFile* all = new TFile("out/v37/out.root", "READ");
  TH1* h = (TH1*)all->Get("GeSpectrum/hEnergy");
  h->SetStats(0);
  h->SetTitle("Peaks Watched For Drift");
  h->GetXaxis()->SetTitle("Energy (keV");
  h->GetYaxis()->SetTitle("Counts (All Al50b Data)");
  h->SetLineColor(kBlack);

  TCanvas* c = new TCanvas();
  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  h->Draw();
  h->GetXaxis()->SetRangeUser(400., 1600.);
  h->GetYaxis()->SetRangeUser(0., 20000.);
  for (unsigned int i = 0; i < npeaks; ++i) {
    char lab[256];
    const Double_t hgt = 4000.;
    const Double_t x1 = peak_search[i];
    const Double_t x2 = peak_search[i];
    const Double_t y2 = h->GetBinContent(h->FindBin(peak_search[i]));
    const Double_t y1 = y2 + hgt;
    TArrow* arr = new TArrow(x1, y1, x2, y2, 0.02);
    arr->SetLineColor(i+1);
    sprintf(lab, "%u keV", (unsigned int)peak_search[i]);
    leg->AddEntry(arr, lab, "l");
    arr->Draw();
  }
  leg->Draw("SAME");

}
