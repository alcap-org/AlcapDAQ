{
  TFile* fdat = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TFile* fsim = new TFile("data/Al50/tm.p.2layer.mudist.root");

  TTree* trdat[2] = { (TTree*)fdat->Get("PID_LP"),
                      (TTree*)fdat->Get("PID_RP") };
  TTree* trsim[2] = { (TTree*)fsim->Get("SiL_TransTree"),
                      (TTree*)fsim->Get("SiR_TransTree") };
  TH1* hdat[2][20];
  TH1* hsim[2][20];
  TH2* HSIM[2] = {
    new TH2D("hlsim", "dEvE L", 20, 2e3, 12e3, 300, 0., 3e3),
    new TH2D("hrsim", "dEvE R", 20, 2e3, 12e3, 300, 0., 3e3)
  };
  trsim[0]->SetBranchStatus("*",      false);
  trsim[1]->SetBranchStatus("*",      false);
  trsim[0]->SetBranchStatus("demeas", true);
  trsim[0]->SetBranchStatus("emeas",  true);
  trsim[1]->SetBranchStatus("demeas", true);
  trsim[1]->SetBranchStatus("emeas",  true);
  trsim[0]->Draw("demeas:emeas>>+hlsim", "", "goff");
  trsim[1]->Draw("demeas:emeas>>+hrsim", "", "goff");
  for (int i = 0; i < 20; ++i) {
    char hldatname[128], hrdatname[128], hldattitle[128], hrdattitle[128];
    char hlsimname[128], hrsimname[128], hlsimtitle[128], hrsimtitle[128];
    sprintf(hldatname, "hldat%d", 2000+i*500);
    sprintf(hrdatname, "hrdat%d", 2000+i*500);
    sprintf(hlsimname,  "hlsim%d",  2000+i*500);
    sprintf(hrsimname,  "hrsim%d",  2000+i*500);
    sprintf(hldattitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Norm=1", 2000+i*500, 2500+i*500);
    sprintf(hrdattitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Norm=1", 2000+i*500, 2500+i*500);
    sprintf(hlsimtitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Norm=1", 2000+i*500, 2500+i*500);
    sprintf(hrsimtitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Norm=1", 2000+i*500, 2500+i*500);
    hdat[0][i] = new TH1D(hldatname, hldattitle, 300, 0, 3000);
    hdat[1][i] = new TH1D(hrdatname, hrdattitle, 300, 0, 3000);
    hsim[0][i] = HSIM[0]->ProjectionY(hlsimname, i+1, i+1);
    hsim[1][i] = HSIM[1]->ProjectionY(hrsimname, i+1, i+1);
    hsim[0][i]->SetTitle(hlsimtitle);
    hsim[1][i]->SetTitle(hrsimtitle);
    hdat[0][i]->SetLineColor(kBlue);
    hdat[1][i]->SetLineColor(kRed);
    hsim[0][i]->SetLineColor(kBlue);
    hsim[1][i]->SetLineColor(kRed);
    hdat[0][i]->SetLineStyle(1);
    hdat[1][i]->SetLineStyle(1);
    hsim[0][i]->SetLineStyle(2);
    hsim[1][i]->SetLineStyle(2);
    hsim[0][i]->GetXaxis()->SetRangeUser(hsim[0][i]->GetMean()-500, hsim[0][i]->GetMean()+500);

    char cmd[64], cut[64];
    sprintf(cut, "%d<e && e<%d && e3==0", 2000+500*i, 2500+500*i);
    sprintf(cmd, "de>>+%s", hldatname);
    trdat[0]->Draw(cmd, cut, "goff");
    sprintf(cmd, "de>>+%s", hrdatname);
    trdat[1]->Draw(cmd, cut, "goff");
  }

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hdat[0][0], "Data SiL");
  leg->AddEntry(hdat[1][0], "Data SiR");
  leg->AddEntry(hsim[0][0], "MC SiL");
  leg->AddEntry(hsim[1][0], "MC SiR");

  gStyle->SetOptStat(0);

  TCanvas* c1 = new TCanvas("c1", "c1", 2100, 1500);
  c1->Divide(3, 3);
  c1->cd(1);
  hsim[0][0]->DrawNormalized("SAME"); hsim[1][0]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][0]->DrawNormalized("SAME"); hdat[1][0]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(2);
  hsim[0][1]->DrawNormalized("SAME"); hsim[1][1]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][1]->DrawNormalized("SAME"); hdat[1][1]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(3);
  hsim[0][2]->DrawNormalized("SAME"); hsim[1][2]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][2]->DrawNormalized("SAME"); hdat[1][2]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(4);
  hsim[0][3]->DrawNormalized("SAME"); hsim[1][3]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][3]->DrawNormalized("SAME"); hdat[1][3]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(5);
  hsim[0][4]->DrawNormalized("SAME"); hsim[1][4]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][4]->DrawNormalized("SAME"); hdat[1][4]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(6);
  hsim[0][5]->DrawNormalized("SAME"); hsim[1][5]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][5]->DrawNormalized("SAME"); hdat[1][5]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(7);
  hsim[0][6]->DrawNormalized("SAME"); hsim[1][6]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][6]->DrawNormalized("SAME"); hdat[1][6]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(8);
  hsim[0][7]->DrawNormalized("SAME"); hsim[1][7]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][7]->DrawNormalized("SAME"); hdat[1][7]->DrawNormalized("SAME"); leg->Draw();
  c1->cd(9);
  hsim[0][8]->DrawNormalized("SAME"); hsim[1][8]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][8]->DrawNormalized("SAME"); hdat[1][8]->DrawNormalized("SAME"); leg->Draw();
  c1->SaveAs("img/deve_data_sim_eslices_1.png");
  TCanvas* c2 = new TCanvas("c2", "c2", 2100, 1500);
  c2->Divide(3, 3);
  c2->cd(1);
  hsim[0][9] ->DrawNormalized("SAME"); hsim[1][9] ->DrawNormalized("SAME"); leg->Draw();
  hdat[0][9] ->DrawNormalized("SAME"); hdat[1][9] ->DrawNormalized("SAME"); leg->Draw();
  c2->cd(2);
  hsim[0][10]->DrawNormalized("SAME"); hsim[1][10]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][10]->DrawNormalized("SAME"); hdat[1][10]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(3);
  hsim[0][11]->DrawNormalized("SAME"); hsim[1][11]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][11]->DrawNormalized("SAME"); hdat[1][11]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(4);
  hsim[0][12]->DrawNormalized("SAME"); hsim[1][12]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][12]->DrawNormalized("SAME"); hdat[1][12]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(5);
  hsim[0][13]->DrawNormalized("SAME"); hsim[1][13]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][13]->DrawNormalized("SAME"); hdat[1][13]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(6);
  hsim[0][14]->DrawNormalized("SAME"); hsim[1][14]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][14]->DrawNormalized("SAME"); hdat[1][14]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(7);
  hsim[0][15]->DrawNormalized("SAME"); hsim[1][15]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][15]->DrawNormalized("SAME"); hdat[1][15]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(8);
  hsim[0][16]->DrawNormalized("SAME"); hsim[1][16]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][16]->DrawNormalized("SAME"); hdat[1][16]->DrawNormalized("SAME"); leg->Draw();
  c2->cd(9);
  hsim[0][17]->DrawNormalized("SAME"); hsim[1][17]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][17]->DrawNormalized("SAME"); hdat[1][17]->DrawNormalized("SAME"); leg->Draw();
  c2->SaveAs("img/deve_data_sim_eslices_2.png");
  TCanvas* c3 = new TCanvas("c3", "c3", 1400, 500);
  c3->Divide(2);
  c3->cd(1);
  hsim[0][18]->DrawNormalized("SAME"); hsim[1][18]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][18]->DrawNormalized("SAME"); hdat[1][18]->DrawNormalized("SAME"); leg->Draw();
  c3->cd(2);
  hsim[0][19]->DrawNormalized("SAME"); hsim[1][19]->DrawNormalized("SAME"); leg->Draw();
  hdat[0][19]->DrawNormalized("SAME"); hdat[1][19]->DrawNormalized("SAME"); leg->Draw();
  c3->SaveAs("img/deve_data_sim_eslices_3.png");
}
