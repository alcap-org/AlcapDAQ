{
  TFile* f = new TFile("out/michel_electrons.root");
  TH2* hlet[4] = { (TH2*)f->Get("hletall"),    (TH2*)f->Get("hlet0si1"),
                   nullptr,                    nullptr };
  TH2* hret[4] = { (TH2*)f->Get("hretall"),     (TH2*)f->Get("hret0si1"),
                  (TH2*)f->Get("hret0si1si3"), nullptr };
  hlet[3] = (TH2*)hlet[0]->Clone("hletsi1"); hlet[3]->Add(hlet[1], -1);
  hret[3] = (TH2*)hret[0]->Clone("hretsi1"); hret[3]->Add(hret[1], -1);
  TH1* hle[4] = {
    hlet[1]->ProjectionY("hle2000ns", hlet[1]->GetXaxis()->FindFixBin(2.0e3), hlet[1]->GetXaxis()->FindFixBin(2.5e3)-1),
    hlet[1]->ProjectionY("hle2500ns", hlet[1]->GetXaxis()->FindFixBin(2.5e3), hlet[1]->GetXaxis()->FindFixBin(3.0e3)-1),
    hlet[1]->ProjectionY("hle3000ns", hlet[1]->GetXaxis()->FindFixBin(3.0e3), hlet[1]->GetXaxis()->FindFixBin(3.5e3)-1),
    hlet[1]->ProjectionY("hle3500ns", hlet[1]->GetXaxis()->FindFixBin(3.5e3), hlet[1]->GetXaxis()->FindFixBin(4.0e3)-1)
  };
  TH1* hre[4] = {
    hret[2]->ProjectionY("hre2000ns", hret[2]->GetXaxis()->FindFixBin(2.0e3), hret[2]->GetXaxis()->FindFixBin(2.5e3)-1),
    hret[2]->ProjectionY("hre2500ns", hret[2]->GetXaxis()->FindFixBin(2.5e3), hret[2]->GetXaxis()->FindFixBin(3.0e3)-1),
    hret[2]->ProjectionY("hre3000ns", hret[2]->GetXaxis()->FindFixBin(3.0e3), hret[2]->GetXaxis()->FindFixBin(3.5e3)-1),
    hret[2]->ProjectionY("hre3500ns", hret[2]->GetXaxis()->FindFixBin(3.5e3), hret[2]->GetXaxis()->FindFixBin(4.0e3)-1)
  };

  hle[0]->SetLineColor(4); hre[0]->SetLineColor(4);
  hle[1]->SetLineColor(2); hre[1]->SetLineColor(2);
  hle[2]->SetLineColor(1); hre[2]->SetLineColor(1);
  hle[3]->SetLineColor(6); hre[3]->SetLineColor(6);

  hle[0]->SetTitle("Al50 SiL3 Hits #tau=864ns (!SiL1+SiL3);Energy [keV];Counts/Expected Counts/100 keV");
  hle[1]->SetTitle("Al50 SiL3 Hits #tau=864ns (!SiL1+SiL3);Energy [keV];Counts/Expected Counts/100 keV");
  hle[2]->SetTitle("Al50 SiL3 Hits #tau=864ns (!SiL1+SiL3);Energy [keV];Counts/Expected Counts/100 keV");
  hle[3]->SetTitle("Al50 SiL3 Hits #tau=864ns (!SiL1+SiL3);Energy [keV];Counts/Expected Counts/100 keV");
  hre[0]->SetTitle("Al50 SiR2 Hits #tau=864ns (!SiR1+SiR2+SiR3);Energy [keV];Counts/Expected Counts/100 keV");
  hre[1]->SetTitle("Al50 SiR2 Hits #tau=864ns (!SiR1+SiR2+SiR3);Energy [keV];Counts/Expected Counts/100 keV");
  hre[2]->SetTitle("Al50 SiR2 Hits #tau=864ns (!SiR1+SiR2+SiR3);Energy [keV];Counts/Expected Counts/100 keV");
  hre[3]->SetTitle("Al50 SiR2 Hits #tau=864ns (!SiR1+SiR2+SiR3);Energy [keV];Counts/Expected Counts/100 keV");

  TF1* fnc[2] = { new TF1("", "[0]+[1]*exp(-x/[2])"),
                  new TF1("", "[0]+[1]*exp(-x/[2])") };
  fnc[0]->SetParameters(1240.20, 8107.64, 864.);
  fnc[1]->SetParameters(680.040, 5239.64, 864.);
  double norm[2][4] = {
    { fnc[0]->Integral(2000, 2500), fnc[0]->Integral(2500, 3000),
      fnc[0]->Integral(3000, 3500), fnc[0]->Integral(3500, 4000) },
    { fnc[1]->Integral(2000, 2500), fnc[1]->Integral(2500, 3000),
      fnc[1]->Integral(3000, 3500), fnc[1]->Integral(3500, 4000) }
  };
  for (int i = 0; i < 4; ++ i) {
    hle[i]->Scale(1/norm[0][i]);
    hre[i]->Scale(1/norm[1][i]);
  }

  TLegend* legt = new TLegend(0.7, 0.7, 0.9, 0.9);
  legt->AddEntry(hle[0], "2#mus<t<2.5#mus");
  legt->AddEntry(hle[1], "2.5#mus<t<3#mus");
  legt->AddEntry(hle[2], "3#mus<t<3.5#mus");
  legt->AddEntry(hle[3], "3.5#mus<t<4#mus");

  gStyle->SetOptStat(0);

  TCanvas* cet = new TCanvas("cet", "cet", 1400, 500);
  cet->Divide(2);
  cet->cd(1);
  hlet[1]->Draw("COLZ");
  cet->cd(2);
  hret[2]->Draw("COLZ");
  cet->SaveAs("img/michel_eletrons_et.png");

  TCanvas* ce = new TCanvas("ce", "ce", 1400, 500);
  ce->Divide(2);
  ce->cd(1)->SetLogy();
  hle[3]->Draw("SAME");
  hle[2]->Draw("SAME");
  hle[1]->Draw("SAME");
  hle[0]->Draw("SAME");
  legt  ->Draw("SAME");
  ce->cd(2)->SetLogy();
  hre[3]->Draw("SAME");
  hre[2]->Draw("SAME");
  hre[1]->Draw("SAME");
  hre[0]->Draw("SAME");
  legt  ->Draw("SAME");
  ce->SaveAs("img/michel_electrons_tslice.png");
}
