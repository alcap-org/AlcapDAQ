{
	TChain* trmu = new TChain("TMETree/TMETree");
  trmu->Add("~/R15bTME/Al50/tme09990.root");
  TH1* he = new TH1D("he", "Muon Beam Counter;Energy [keV];Counts/10 keV", 150, 0., 1.5e3);
  TH1* ht = new TH1D("ht", "Time Between Entering Muons;Time [#mus];Counts/#mus", 194, 6, 200);
  trmu->Draw("SiT_1.E>>+he", "anyDoubleCountedPulses==0", "goff");
  trmu->Draw("SiT_2.E>>+he", "anyDoubleCountedPulses==0", "goff");
  trmu->Draw("SiT_3.E>>+he", "anyDoubleCountedPulses==0", "goff");
  trmu->Draw("SiT_4.E>>+he", "anyDoubleCountedPulses==0", "goff");
  trmu->Draw("timeToNextTME/1e3>>+ht", "", "goff");
  double ecut = 190;
  double tcut = 10;

  TLine* lecut  = new TLine(ecut, 0.9, ecut, 2e5);
  TLine* ltcut  = new TLine(tcut, 3e3, tcut, 11400);
  TArrow* arrmu = new TArrow(ecut, 100, 500, 100);
  TArrow* arrpp = new TArrow(tcut, 5.5e3, 60, 5.5e3);
  TLatex* txtmu = new TLatex(300, 200, "#mu");
  TLatex* txte  = new TLatex(100, 200, "e");
  TLatex* txtpp = new TLatex(15, 6e3, "Pileup Protected");
  txtmu->SetTextSize(0.1);
  txte ->SetTextSize(0.08);
  he   ->SetLineColor(kBlack);
  ht   ->SetLineColor(kBlack);
  he   ->SetMaximum(2e5);
  he   ->SetMinimum(0.9);
  ht   ->SetMinimum(3e3);
  ht   ->SetMaximum(11.4e3);
  ht->GetYaxis()->SetTitleOffset(1.3);

  gStyle->SetOptStat(0);

  TCanvas* ce = new TCanvas("ce", "ce", 700, 500);
  ce->SetLogy();
  he   ->Draw();
  lecut->Draw();
  arrmu->Draw();
  txtmu->Draw();
  txte ->Draw();
  ce->SaveAs("img/psi2019/sit_e.png");

  TCanvas* ct = new TCanvas("ct", "ct", 700, 500);
  ht   ->Draw();
  ltcut->Draw();
  arrpp->Draw();
  txtpp->Draw();
  ct->SaveAs("img/psi2019/sit_t.png");

  printf("PP rejected %.2f%%\n", 100*trmu->GetEntries("timeToNextTME<10e3")/double(trmu->GetEntries()));
}
