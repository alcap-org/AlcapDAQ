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
  h1->GetXaxis()->SetRangeUser(100., 15000.);
  h2->GetXaxis()->SetRangeUser(100., 15000.);
  h3->GetXaxis()->SetRangeUser(100., 15000.);
  h4->GetXaxis()->SetRangeUser(100., 15000.);
  h1->SetTitle("Ge Si16P");
  h2->SetTitle("Ge Si16P");
  h3->SetTitle("Ge Si16P");
  h4->SetTitle("Ge Si16P");
  h1->GetXaxis()->SetTitle("Energy (ADC)"); h1->GetYaxis()->SetTitle("Counts");
  h2->GetXaxis()->SetTitle("Energy (ADC)"); h2->GetYaxis()->SetTitle("Counts");
  h3->GetXaxis()->SetTitle("Energy (ADC)"); h3->GetYaxis()->SetTitle("Counts");
  h4->GetXaxis()->SetTitle("Energy (ADC)"); h4->GetYaxis()->SetTitle("Counts");

  TLegend *leg = new TLegend(0.7,0.7,0.9,0.9);
  leg->AddEntry(h1, "All");
  leg->AddEntry(h2, "Out of time (> +/- 5 us)");
  leg->AddEntry(h3, "Out of time (> +/-500 ns)");
  leg->AddEntry(h4, "In Time (< +/-500 ns)");

  h1->SetLineColor(kBlack);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kRed);
  h4->SetLineColor(kGreen);

  new TCanvas;
  h1->DrawClone();
  h2->DrawClone("SAME");
  h3->DrawClone("SAME");
  h4->DrawClone("SAME");
  leg->DrawClone("SAME");
  (new TCanvas)->SetLogy();
  h1->GetYaxis()->SetRangeUser(1., 10000.);
  h2->GetYaxis()->SetRangeUser(1., 10000.);
  h3->GetYaxis()->SetRangeUser(1., 10000.);
  h4->GetYaxis()->SetRangeUser(1., 10000.);
  h1->Draw();
  h2->Draw("SAME");
  h3->Draw("SAME");
  h4->Draw("SAME");
  leg->Draw("SAME");
}
