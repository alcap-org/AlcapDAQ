{
  TChain* tr = new TChain("TMETree/TMETree");
  tr->Add("~/R15bTME/Al50/tme0998*.root");
  tr->Add("~/R15bTME/Al50/tme0999*.root");
  TH1* he = new TH1D("he", "Prompt Photon Energy;Energy [keV];Counts/keV", 100,  300, 400);
  TH1* ht = new TH1D("ht", "X-Ray Hit Time;Time [ns];Counts/10 ns",     100, -500, 500);
  tr->Draw("GeHiGain.E>>he",    "anyDoubleCountedPulses==0 && abs(GeHiGain.tTME)<200",           "goff");
  tr->Draw("GeHiGain.tTME>>ht", "anyDoubleCountedPulses==0 && 342<GeHiGain.E && GeHiGain.E<353", "goff");

  TLatex* txtal    = new TLatex(348, 2.4e3, "#muAl_{2p-1s}");
  TLatex* txtpb    = new TLatex(351,   600, "#muPb#rightarrowTl");
  TLatex* txtpbt   = new TLatex(210,    55, "#muPb Capture");
  TLatex* txtalt   = new TLatex(-100,  100, "Al 2p-1s");
  TLine*  ltcut[2] = {
    new TLine(-200, 0, -200, 620), new TLine(200, 0, 200, 620)
  };

  txtal ->SetTextSize(0.1);
  txtpb ->SetTextSize(0.1);
  txtalt->SetTextSize(0.1);
  txtpbt->SetTextSize(0.1);
  // he->GetXaxis()->SetTitleSize(0.06);
  // he->GetYaxis()->SetTitleSize(0.06);
  // ht->GetXaxis()->SetTitleSize(0.06);
  // ht->GetYaxis()->SetTitleSize(0.06);
  // he->GetXaxis()->SetLabelSize(0.04);
  // he->GetYaxis()->SetLabelSize(0.04);
  // ht->GetXaxis()->SetLabelSize(0.04);
  // ht->GetYaxis()->SetLabelSize(0.04);
  // he->GetYaxis()->SetTitleOffset(1.1);
  // ht->GetYaxis()->SetTitleOffset(1.1);
  he    ->SetLineColor(kBlack);
  ht    ->SetLineColor(kBlack);

  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c", "c", 700, 600);
  c->Divide(1, 2);
  c->cd(1);
  he   ->Draw();
  txtal->Draw();
  txtpb->Draw();
  c->cd(2);
  ht      ->Draw();
  ltcut[0]->Draw();
  ltcut[1]->Draw();
  txtpbt  ->Draw();
  txtalt  ->Draw();
  c->SaveAs("img/psi2019/ge.png");
}
