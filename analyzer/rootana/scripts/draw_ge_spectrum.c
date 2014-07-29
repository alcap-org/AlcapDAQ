{
  TFile* f = new TFile("out.root");
  TH1* h1 = (TH1*)f->Get("GeSpectrum/hEnergy");
  TH1* h2 = (TH1*)f->Get("GeSpectrum/hEnergyFarOOT");
  TH1* h3 = (TH1*)f->Get("GeSpectrum/hEnergyOOT");
  TH1* h4 = ((TH2*)f->Get("GeSpectrum/hTimeEnergy"))->ProjectionY();
  h1->SetStats(0);
  h2->SetStats(0);
  h3->SetStats(0);
  h4->SetStats(0);
  TLegend *leg = new TLegend(0.7,0.7,0.9,0.9);
  leg->AddEntry(h1, "All");
  leg->AddEntry(h2, "Out of time (> +/- 2 us)");
  leg->AddEntry(h3, "Out of time (> +/-200 ns)");
  leg->AddEntry(h4, "In Time (< +/-200 ns)");
  h1->SetLineColor(kBlack);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kRed);
  h4->SetLineColor(kGreen);
  new TCanvas;
  h1->Draw(); h2->Draw("SAME"); h3->Draw("SAME"); h4->Draw("SAME"); leg->Draw("SAME");
}
