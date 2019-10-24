{
  // MC SiT volID 0-3 is upper right, then clockwise
  // RL 1-4 starts in upper-left and goes counter-clockwise
	TFile* fsim = new TFile("~/data/R15b/mc/R15bSi16bMuPreSiT_1.root");
  TFile* fdat = new TFile("~/R15bTME/Si16b/out10390.root");
  TTree* trsim = (TTree*)fsim->Get("tree");
  TTree* trdat = (TTree*)fdat->Get("TMETree/TMETree");
  long long nsim[14] = {
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==1"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==2"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==3"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==4"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==5"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==6"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==7"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==8"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==9"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==10"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==11"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==12"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==13"),
    trsim->GetEntries("M_volName==\"Target\" && M_particleName==\"mu-\" && M_volID==14")
  };
  long long ndat[14] = {
    trdat->GetEntries("@SiL1_2.size()==1"),
    trdat->GetEntries("@SiL1_3.size()==1"),
    trdat->GetEntries("@SiL1_4.size()==1"),
    trdat->GetEntries("@SiL1_5.size()==1"),
    trdat->GetEntries("@SiL1_6.size()==1"),
    trdat->GetEntries("@SiL1_7.size()==1"),
    trdat->GetEntries("@SiL1_8.size()==1"),
    trdat->GetEntries("@SiL1_9.size()==1"),
    trdat->GetEntries("@SiL1_10.size()==1"),
    trdat->GetEntries("@SiL1_11.size()==1"),
    trdat->GetEntries("@SiL1_12.size()==1"),
    trdat->GetEntries("@SiL1_13.size()==1"),
    trdat->GetEntries("@SiL1_14.size()==1"),
    trdat->GetEntries("@SiL1_15.size()==1"),
  };
  TH1* hsim = new TH1D("hsim", "Si16b Target Hit Distribution;Segment;Hit/#mu stop", 16, 0.5, 16.5);
  TH1* hdat = new TH1D("hdat", "Si16b Target Hit Distribution;Segment;Hit/#mu stop", 16, 0.5, 16.5);
  for (int i = 0; i < 14; ++i) {
    hsim->SetBinContent(i+2, nsim[i]);
    hsim->SetBinError  (i+2, std::sqrt(nsim[i]));
    hdat->SetBinContent(i+2, ndat[i]);
    hdat->SetBinError  (i+2, std::sqrt(ndat[i]));
  }
  hsim->Scale(1./std::accumulate(nsim, nsim+14, 0.));
  hdat->Scale(1./std::accumulate(ndat, ndat+14, 0.));

  hsim->SetLineColor(2);
  hdat->SetLineColor(1);
  hsim->SetMinimum(0);
  hdat->SetMinimum(0);

  TLegend* l = new TLegend(0.4, 0.2, 0.6, 0.4);
  l->AddEntry(hdat, "Data");
  l->AddEntry(hsim, "MC");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 700, 500);
  hsim->Draw("SAME");
  hdat->Draw("SAME");
  l->Draw();
  c->SaveAs("img/targ_pos_mc_data.png");
}
