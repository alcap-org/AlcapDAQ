{
  // MC SiT volID 0-3 is upper right, then clockwise
  // RL 1-4 starts in upper-left and goes counter-clockwise
	TFile* fsim = new TFile("~/data/R15b/mc/R15bAl50MuPreSit_1.root");
  TFile* fdat = new TFile("~/R15bTME/Al50/tme09890.root");
  TTree* trsim = (TTree*)fsim->Get("tree");
  TTree* trdat = (TTree*)fdat->Get("TMETree/TMETree");
  long long nsim[4] = {
    trsim->GetEntries("M_volName==\"SiT\" && M_particleName==\"mu-\" && M_volID==0"),
    trsim->GetEntries("M_volName==\"SiT\" && M_particleName==\"mu-\" && M_volID==1"),
    trsim->GetEntries("M_volName==\"SiT\" && M_particleName==\"mu-\" && M_volID==2"),
    trsim->GetEntries("M_volName==\"SiT\" && M_particleName==\"mu-\" && M_volID==3")
  };
  long long ndat[4] = {
    trdat->GetEntries("@SiT_4.size()==1"),
    trdat->GetEntries("@SiT_3.size()==1"),
    trdat->GetEntries("@SiT_2.size()==1"),
    trdat->GetEntries("@SiT_1.size()==1")
  };
  TH1* hsim = new TH1D("hsim", "SiT Hit Distribution;Quadrant;Hit/#mu", 4, 0.5, 4.5);
  TH1* hdat = new TH1D("hdat", "SiT Hit Distribution;Quadrant;Hit/#mu", 4, 0.5, 4.5);
  hsim->SetBinContent(1, nsim[0]); hsim->SetBinError(1, std::sqrt(nsim[0]));
  hsim->SetBinContent(2, nsim[1]); hsim->SetBinError(2, std::sqrt(nsim[1]));
  hsim->SetBinContent(3, nsim[2]); hsim->SetBinError(3, std::sqrt(nsim[2]));
  hsim->SetBinContent(4, nsim[3]); hsim->SetBinError(4, std::sqrt(nsim[3]));
  hdat->SetBinContent(1, ndat[0]); hdat->SetBinError(1, std::sqrt(ndat[0]));
  hdat->SetBinContent(2, ndat[1]); hdat->SetBinError(2, std::sqrt(ndat[1]));
  hdat->SetBinContent(3, ndat[2]); hdat->SetBinError(3, std::sqrt(ndat[2]));
  hdat->SetBinContent(4, ndat[3]); hdat->SetBinError(4, std::sqrt(ndat[3]));
  hsim->Scale(1./(nsim[0]+nsim[1]+nsim[2]+nsim[3]));
  hdat->Scale(1./(ndat[0]+ndat[1]+ndat[2]+ndat[3]));

  hsim->SetLineColor(2);
  hdat->SetLineColor(1);
  hsim->SetMinimum(0);
  hdat->SetMinimum(0);

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hdat, "Data");
  l->AddEntry(hsim, "MC");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 700, 500);
  hdat->Draw("SAME");
  hsim->Draw("SAME");
  l->Draw();
  c->SaveAs("img/sit_pos_mc_data.png")
}
