{
  // MC SiT volID 0-3 is upper right, then clockwise
  // RL 1-4 starts in upper-left and goes counter-clockwise
	TFile* fsim = new TFile("~/data/R15b/mc/R15bSi16bMuPreSiT_1.root");
  TFile* fdat = new TFile("~/R15bTME/Si16b/out10400.root");
  TTree* trsim = (TTree*)fsim->Get("tree");
  TTree* trdat = (TTree*)fdat->Get("TMETree/TMETree");
  TH1* hsim = new TH1D("hsim", "Si16b Target Energy;E [keV];", 100, 0., 2.e3);
  TH1* hdat = new TH1D("hdat", "Si16b Target Energy;E [keV];", 100, 0., 2.e3);
  trsim->Draw("1e6*M_edep>>+hsim", "M_volName==\"Target\" && M_particleName==\"mu-\"", "goff");
  trdat->Draw("SiL1_2.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_3.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_4.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_5.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_6.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_7.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_8.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_9.E>>+hdat",  "", "goff");
  trdat->Draw("SiL1_10.E>>+hdat", "", "goff");
  trdat->Draw("SiL1_11.E>>+hdat", "", "goff");
  trdat->Draw("SiL1_12.E>>+hdat", "", "goff");
  trdat->Draw("SiL1_13.E>>+hdat", "", "goff");
  trdat->Draw("SiL1_14.E>>+hdat", "", "goff");
  trdat->Draw("SiL1_15.E>>+hdat", "", "goff");
  hsim->Scale(1./hsim->GetEntries());
  hdat->Scale(1./hdat->GetEntries());

  hsim->SetLineColor(2);
  hdat->SetLineColor(1);
  hsim->SetMinimum(0);
  hdat->SetMinimum(0);

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hdat, "Data");
  l->AddEntry(hsim, "MC");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 700, 500);
  hsim->Draw("SAME");
  hdat->Draw("SAME");
  l->Draw();
  c->SaveAs("img/sit_en_mc_data.png");
}
