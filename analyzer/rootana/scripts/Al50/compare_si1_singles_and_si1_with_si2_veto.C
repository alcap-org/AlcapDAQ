{
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add("~/R15bTME/Al50/tme09890.root");
  // ch->Add("~/R15bTME/Al50/tme*.root");
  ch->Draw("SiR1_1.E>>hr_veto(70,0,7e3)",  "@SiR2.size()==0 && @SiR1_1.size()==1", "goff");
  ch->Draw("SiR1_2.E>>+hr_veto",           "@SiR2.size()==0 && @SiR1_2.size()==1", "goff");
  ch->Draw("SiR1_3.E>>+hr_veto",           "@SiR2.size()==0 && @SiR1_3.size()==1", "goff");
  ch->Draw("SiR1_4.E>>+hr_veto",           "@SiR2.size()==0 && @SiR1_4.size()==1", "goff");
  ch->Draw("SiR1_1.E>>hr(70,0,7e3)",       "@SiR1_1.size()==1", "goff");
  ch->Draw("SiR1_2.E>>+hr",                "@SiR1_2.size()==1", "goff");
  ch->Draw("SiR1_3.E>>+hr",                "@SiR1_3.size()==1", "goff");
  ch->Draw("SiR1_4.E>>+hr",                "@SiR1_4.size()==1", "goff");
  ch->Draw("SiL1_2.E>>+hl_veto(70,0,7e3)", "@SiL3.size()==0 && @SiL1_2.size()==1", "goff");
  ch->Draw("SiL1_3.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_3.size()==1", "goff");
  ch->Draw("SiL1_4.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_4.size()==1", "goff");
  ch->Draw("SiL1_5.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_5.size()==1", "goff");
  ch->Draw("SiL1_6.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_6.size()==1", "goff");
  ch->Draw("SiL1_7.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_7.size()==1", "goff");
  ch->Draw("SiL1_8.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_8.size()==1", "goff");
  ch->Draw("SiL1_9.E>>+hl_veto",           "@SiL3.size()==0 && @SiL1_9.size()==1", "goff");
  ch->Draw("SiL1_10.E>>+hl_veto",          "@SiL3.size()==0 && @SiL1_10.size()==1", "goff");
  ch->Draw("SiL1_11.E>>+hl_veto",          "@SiL3.size()==0 && @SiL1_11.size()==1", "goff");
  ch->Draw("SiL1_12.E>>+hl_veto",          "@SiL3.size()==0 && @SiL1_12.size()==1", "goff");
  ch->Draw("SiL1_13.E>>+hl_veto",          "@SiL3.size()==0 && @SiL1_13.size()==1", "goff");
  ch->Draw("SiL1_14.E>>+hl_veto",          "@SiL3.size()==0 && @SiL1_14.size()==1", "goff");
  ch->Draw("SiL1_15.E>>+hl_veto",          "@SiL3.size()==0 && @SiL1_15.size()==1", "goff");
  ch->Draw("SiL1_2.E>>hl(70,0,7e3)",       "@SiL1_2.size()==1", "goff");
  ch->Draw("SiL1_3.E>>+hl",                "@SiL1_3.size()==1", "goff");
  ch->Draw("SiL1_4.E>>+hl",                "@SiL1_4.size()==1", "goff");
  ch->Draw("SiL1_5.E>>+hl",                "@SiL1_5.size()==1", "goff");
  ch->Draw("SiL1_6.E>>+hl",                "@SiL1_6.size()==1", "goff");
  ch->Draw("SiL1_7.E>>+hl",                "@SiL1_7.size()==1", "goff");
  ch->Draw("SiL1_8.E>>+hl",                "@SiL1_8.size()==1", "goff");
  ch->Draw("SiL1_9.E>>+hl",                "@SiL1_9.size()==1", "goff");
  ch->Draw("SiL1_10.E>>+hl",               "@SiL1_10.size()==1", "goff");
  ch->Draw("SiL1_11.E>>+hl",               "@SiL1_11.size()==1", "goff");
  ch->Draw("SiL1_12.E>>+hl",               "@SiL1_12.size()==1", "goff");
  ch->Draw("SiL1_13.E>>+hl",               "@SiL1_13.size()==1", "goff");
  ch->Draw("SiL1_14.E>>+hl",               "@SiL1_14.size()==1", "goff");
  ch->Draw("SiL1_15.E>>+hl",               "@SiL1_15.size()==1", "goff");


  hr->SetTitle("SiR1 Energy;E [keV];/100 keV");
  hl->SetTitle("SiL1 Energy;E [keV];/100 keV");
  hr_veto->SetLineColor(2);
  hr     ->SetLineColor(1);
  hl_veto->SetLineColor(2);
  hl     ->SetLineColor(1);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hl,      "Singles");
  leg->AddEntry(hl_veto, "#bar{Si2}");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1)->SetLogy();
  leg->Draw();
  hl     ->Draw();
  hl_veto->Draw("SAME");
  c->cd(2)->SetLogy();
  hr     ->Draw();
  hr_veto->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/compare_si1_singles_and_si1_with_si2_veto.png");
}
