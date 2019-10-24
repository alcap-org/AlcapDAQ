{
  TFile* f = new TFile("out/michel_electrons.root");
  TH2* hlet[4] = { (TH2*)f->Get("hletall"),     (TH2*)f->Get("hlet0si1"),
                   nullptr,                     nullptr };
  TH2* hret[4] = { (TH2*)f->Get("hretall"),     (TH2*)f->Get("hret0si1"),
                   (TH2*)f->Get("hret0si1si3"), nullptr };
  hlet[3] = (TH2*)hlet[0]->Clone("hletsi1"); hlet[3]->Add(hlet[1], -1);
  hret[3] = (TH2*)hret[0]->Clone("hretsi1"); hret[3]->Add(hret[1], -1);
  TH1* hle[4] = {
    hlet[0]->ProjectionY("hleall",     hlet[0]->GetXaxis()->FindFixBin(0.), hlet[0]->GetXaxis()->FindFixBin(10e3)-1),
    hlet[1]->ProjectionY("hle0si1",    hlet[1]->GetXaxis()->FindFixBin(0.), hlet[1]->GetXaxis()->FindFixBin(10e3)-1),
    nullptr,
    hlet[3]->ProjectionY("hlesi1",     hlet[3]->GetXaxis()->FindFixBin(0.), hlet[3]->GetXaxis()->FindFixBin(10e3)-1)
  };
  TH1* hre[4] = {
    hret[0]->ProjectionY("hreall",     hret[0]->GetXaxis()->FindFixBin(0.), hret[0]->GetXaxis()->FindFixBin(10e3)-1),
    hret[1]->ProjectionY("hre0si1",    hret[1]->GetXaxis()->FindFixBin(0.), hret[1]->GetXaxis()->FindFixBin(10e3)-1),
    hret[2]->ProjectionY("hre0si1si3", hret[2]->GetXaxis()->FindFixBin(0.), hret[2]->GetXaxis()->FindFixBin(10e3)-1),
    hret[3]->ProjectionY("hresi1",     hret[3]->GetXaxis()->FindFixBin(0.), hret[3]->GetXaxis()->FindFixBin(10e3)-1)
  };
  TH1* hlt[4] = {
    hlet[0]->ProjectionX("hltall"),
    hlet[1]->ProjectionX("hlt0si1"),
    nullptr,
    hlet[3]->ProjectionX("hltsi1")
  };
  TH1* hrt[4] = {
    hret[0]->ProjectionX("hrtall"),
    hret[1]->ProjectionX("hrt0si1"),
    hret[2]->ProjectionX("hrt0si1si3"),
    hret[3]->ProjectionX("hrtsi1")
  };

  TF1* fitlt[4] = {
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"),
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"),
    nullptr,
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])")
  };
  TF1* fitrt[4] = {
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"),
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"),
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"),
    new TF1("", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])")
  };

  fitlt[0]->SetParameters(2e3, 3e3, 200, 1e3, 800);
  fitlt[1]->SetParameters(2e3, 3e3, 200, 1e3, 800);
  fitlt[3]->SetParameters(8,   8e2, 200, 2e2, 800);
  fitrt[0]->SetParameters(2e3, 5e4, 200, 1e4, 800);
  fitrt[1]->SetParameters(2e3, 5e4, 200, 1e4, 800);
  fitrt[2]->SetParameters(1e3, 2e4, 200, 5e3, 800);
  fitrt[3]->SetParameters(3e1, 1e3, 200, 5e2, 800);

  fitlt[0]->SetLineColor(kBlack);
  fitlt[1]->SetLineColor(kBlue);
  fitlt[3]->SetLineColor(kRed);
  fitrt[0]->SetLineColor(kBlack);
  fitrt[1]->SetLineColor(kBlue);
  fitrt[2]->SetLineColor(kGreen);
  fitrt[3]->SetLineColor(kRed);
  hle[0]->SetLineColor(kBlack); hlt[0]->SetLineColor(kBlack);
  hle[1]->SetLineColor(kBlue);  hlt[1]->SetLineColor(kBlue);
  hle[3]->SetLineColor(kRed);   hlt[3]->SetLineColor(kRed);
  hre[0]->SetLineColor(kBlack); hrt[0]->SetLineColor(kBlack);
  hre[1]->SetLineColor(kBlue);  hrt[1]->SetLineColor(kBlue);
  hre[2]->SetLineColor(kGreen); hrt[2]->SetLineColor(kGreen);
  hre[3]->SetLineColor(kRed);   hrt[3]->SetLineColor(kRed);

  hlt[0]->SetMinimum(3);
  hrt[0]->SetMinimum(20);

  // fitlt[0]->FixParameter(1, 0); fitlt[0]->FixParameter(2, 1);
  // fitlt[1]->FixParameter(1, 0); fitlt[1]->FixParameter(2, 1);
  // fitlt[3]->FixParameter(1, 0); fitlt[3]->FixParameter(2, 1);
  // fitrt[0]->FixParameter(1, 0); fitrt[0]->FixParameter(2, 1);
  // fitrt[1]->FixParameter(1, 0); fitrt[1]->FixParameter(2, 1);
  // fitrt[2]->FixParameter(1, 0); fitrt[2]->FixParameter(2, 1);
  // fitrt[3]->FixParameter(1, 0); fitrt[3]->FixParameter(2, 1);
  hlt[0]->SetTitle("SiL Hits");
  hrt[0]->SetTitle("SiR Hits"); hre[0]->SetTitle("SiR Hits");
  // hlt[0]->GetXaxis()->SetRangeUser(500, 5e3);
  // hlt[1]->GetXaxis()->SetRangeUser(500, 5e3);
  // hlt[3]->GetXaxis()->SetRangeUser(500, 5e3);
  // hrt[0]->GetXaxis()->SetRangeUser(500, 5e3); hre[0]->GetXaxis()->SetRangeUser(0, 2e3);
  // hrt[1]->GetXaxis()->SetRangeUser(500, 5e3); hre[1]->GetXaxis()->SetRangeUser(0, 2e3);
  // hrt[2]->GetXaxis()->SetRangeUser(500, 5e3); hre[2]->GetXaxis()->SetRangeUser(0, 2e3);
  // hrt[3]->GetXaxis()->SetRangeUser(500, 5e3); hre[3]->GetXaxis()->SetRangeUser(0, 2e3);
  hlt[0]->GetXaxis()->SetRangeUser(0, 5e3);
  hlt[1]->GetXaxis()->SetRangeUser(0, 5e3);
  hlt[3]->GetXaxis()->SetRangeUser(0, 5e3);
  hrt[0]->GetXaxis()->SetRangeUser(0, 5e3); hre[0]->GetXaxis()->SetRangeUser(0, 2e3);
  hrt[1]->GetXaxis()->SetRangeUser(0, 5e3); hre[1]->GetXaxis()->SetRangeUser(0, 2e3);
  hrt[2]->GetXaxis()->SetRangeUser(0, 5e3); hre[2]->GetXaxis()->SetRangeUser(0, 2e3);
  hrt[3]->GetXaxis()->SetRangeUser(0, 5e3); hre[3]->GetXaxis()->SetRangeUser(0, 2e3);

  TFitResultPtr fitresl[4] = {
    hlt[0]->Fit(fitlt[0], "MS"), hlt[1]->Fit(fitlt[1], "MS"),
    nullptr,                     hlt[3]->Fit(fitlt[3], "MS")
  };
  TFitResultPtr fitresr[4] = {
    hrt[0]->Fit(fitrt[0], "MS"), hrt[1]->Fit(fitrt[1], "MS"),
    hrt[2]->Fit(fitrt[2], "MS"), hrt[3]->Fit(fitrt[3], "MS")
  };

  char txtl[4][128], txtr[4][128];
  std::sprintf(txtl[0], "All (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)",          fitresl[0]->Parameter(2), fitresl[0]->Error(2), fitresl[0]->Parameter(4), fitresl[0]->Error(4));
  std::sprintf(txtl[1], "!Si1+Si2 (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)",     fitresl[1]->Parameter(2), fitresl[1]->Error(2), fitresl[1]->Parameter(4), fitresl[1]->Error(4));
  std::sprintf(txtl[3], "Si1+Si2 (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)",      fitresl[3]->Parameter(2), fitresl[3]->Error(2), fitresl[3]->Parameter(4), fitresl[3]->Error(4));
  std::sprintf(txtr[0], "All (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)",          fitresr[0]->Parameter(2), fitresr[0]->Error(2), fitresr[0]->Parameter(4), fitresr[0]->Error(4));
  std::sprintf(txtr[1], "!Si1+Si2 (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)",     fitresr[1]->Parameter(2), fitresr[1]->Error(2), fitresr[1]->Parameter(4), fitresr[1]->Error(4));
  std::sprintf(txtr[2], "!Si1+Si2+Si3 (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)", fitresr[2]->Parameter(2), fitresr[2]->Error(2), fitresr[2]->Parameter(4), fitresr[2]->Error(4));
  std::sprintf(txtr[3], "Si1+Si2 (#tau_{1}=%.1f#pm%.1fns,#tau_{2}=%.1f#pm%.1fns)",      fitresr[3]->Parameter(2), fitresr[3]->Error(2), fitresr[3]->Parameter(4), fitresr[3]->Error(4));
  // std::sprintf(txtl[0], "All (#tau=%.1f#pm%.1fns)",          fitresl[0]->Parameter(4), fitresl[0]->Error(4));
  // std::sprintf(txtl[1], "!Si1+Si2 (#tau=%.1f#pm%.1fns)",     fitresl[1]->Parameter(4), fitresl[1]->Error(4));
  // std::sprintf(txtl[3], "Si1+Si2 (#tau=%.1f#pm%.1fns)",      fitresl[3]->Parameter(4), fitresl[3]->Error(4));
  // std::sprintf(txtr[0], "All (#tau=%.1f#pm%.1fns)",          fitresr[0]->Parameter(4), fitresr[0]->Error(4));
  // std::sprintf(txtr[1], "!Si1+Si2 (#tau=%.1f#pm%.1fns)",     fitresr[1]->Parameter(4), fitresr[1]->Error(4));
  // std::sprintf(txtr[2], "!Si1+Si2+Si3 (#tau=%.1f#pm%.1fns)", fitresr[2]->Parameter(4), fitresr[2]->Error(4));
  // std::sprintf(txtr[3], "Si1+Si2 (#tau=%.1f#pm%.1fns)",      fitresr[3]->Parameter(4), fitresr[3]->Error(4));
  TLegend* leg[2] = { new TLegend(0.5, 0.72, 0.9, 0.9),
                      new TLegend(0.5, 0.7, 0.9, 0.9) };
  leg[0]->AddEntry(hle[0], txtl[0]);
  leg[0]->AddEntry(hle[1], txtl[1]);
  leg[0]->AddEntry(hle[3], txtl[3]);
  leg[1]->AddEntry(hre[0], txtr[0]);
  leg[1]->AddEntry(hre[1], txtr[1]);
  leg[1]->AddEntry(hre[2], txtr[2]);
  leg[1]->AddEntry(hre[3], txtr[3]);

  gStyle->SetOptStat(0);
  TCanvas* ct = new TCanvas("ct", "ct", 1400, 500);
  ct->Divide(2);
  ct->cd(1)->SetLogy();
  hlt[0]->Draw("SAME");
  hlt[1]->Draw("SAME");
  hlt[3]->Draw("SAME");
  leg[0]->Draw();
  ct->cd(2)->SetLogy();
  hrt[0]->Draw("SAME");
  hrt[1]->Draw("SAME");
  hrt[2]->Draw("SAME");
  hrt[3]->Draw("SAME");
  leg[1]->Draw("SAME");
  ct->SaveAs("img/michel_lifetime_500ns-5us.png");



  //////////////////////////////////////////////////////////////////////////////
  TCanvas* ce = new TCanvas("ce", "ce", 700, 500);
  hre[0]->Draw("SAME");
  hre[1]->Draw("SAME");
  hre[2]->Draw("SAME");
  hre[3]->Draw("SAME");
  leg[1]->Draw("SAME");
  ce->SaveAs("img/michel_energies.png");
}
