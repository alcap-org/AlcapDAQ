{
  TFile* ftm   = new TFile("data/tmal50.root");
  TFile* fdata = new TFile("data/pselal50.root");
  RooUnfoldResponse* TM[2][10] = {
    {
      (RooUnfoldResponse*)ftm->Get("SiL_TM_0"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_9"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_8"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_7"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_6"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_5"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_4"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_3"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_2"),
      (RooUnfoldResponse*)ftm->Get("SiL_TM_1")
    },
    {
      (RooUnfoldResponse*)ftm->Get("SiR_TM_0"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_9"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_8"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_7"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_6"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_5"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_4"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_3"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_2"),
      (RooUnfoldResponse*)ftm->Get("SiR_TM_1")
    }
  };
  TH2* DM[2][10] = {
    {
      (TH2*)ftm->Get("SiL_DM_0"), (TH2*)ftm->Get("SiL_DM_9"),
      (TH2*)ftm->Get("SiL_DM_8"), (TH2*)ftm->Get("SiL_DM_7"),
      (TH2*)ftm->Get("SiL_DM_6"), (TH2*)ftm->Get("SiL_DM_5"),
      (TH2*)ftm->Get("SiL_DM_4"), (TH2*)ftm->Get("SiL_DM_3"),
      (TH2*)ftm->Get("SiL_DM_2"), (TH2*)ftm->Get("SiL_DM_1")
    },
    {
      (TH2*)ftm->Get("SiR_DM_0"), (TH2*)ftm->Get("SiR_DM_9"),
      (TH2*)ftm->Get("SiR_DM_8"), (TH2*)ftm->Get("SiR_DM_7"),
      (TH2*)ftm->Get("SiR_DM_6"), (TH2*)ftm->Get("SiR_DM_5"),
      (TH2*)ftm->Get("SiR_DM_4"), (TH2*)ftm->Get("SiR_DM_3"),
      (TH2*)ftm->Get("SiR_DM_2"), (TH2*)ftm->Get("SiR_DM_1")
    }
  };
  TH1* TMp[2][10];
  TProfile* DMp[2][10];
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 10; ++j) {
      TMp[i][j] = TM[i][j]->Hresponse()->ProjectionY();
      TM[i][j] ->SetTitle("Response;E_{meas} [keV];E_{0} [keV]");
      TMp[i][j]->SetTitle(";E_{0} [keV]");
      TMp[i][j]->SetLineColor(j);
      TMp[i][j]->SetMaximum(2e3);
      DMp[i][j] = DM[i][j]->ProfileX();
      DMp[i][j]->SetLineColor(j);
      DMp[i][j]->SetStats(false);
      DM[i][j] ->SetTitle("MC Measured Differences;E_{0} [keV];E_{0}-E_{meas}");
      DMp[i][j]->SetTitle("MC Measured Differences;E_{0} [keV];E_{0}-E_{meas}");
      DM[i][j]->GetYaxis()->SetRangeUser(0, 3e3);
    }
  }
  TPave* box[10];
  box[1] = new TPave(0.6, 0.8, 0.7, 0.9, 0, "NB NDC");
  box[2] = new TPave(0.7, 0.8, 0.8, 0.9, 0, "NB NDC");
  box[3] = new TPave(0.8, 0.8, 0.9, 0.9, 0, "NB NDC");
  box[4] = new TPave(0.6, 0.7, 0.7, 0.8, 0, "NB NDC");
  box[5] = new TPave(0.7, 0.7, 0.8, 0.8, 0, "NB NDC");
  box[6] = new TPave(0.8, 0.7, 0.9, 0.8, 0, "NB NDC");
  box[7] = new TPave(0.6, 0.6, 0.7, 0.7, 0, "NB NDC");
  box[8] = new TPave(0.7, 0.6, 0.8, 0.7, 0, "NB NDC");
  box[9] = new TPave(0.8, 0.6, 0.9, 0.7, 0, "NB NDC");
  for (int i = 1; i <= 9; ++i)
    box[i]->SetFillColor(i);

  TH3* hevdevt[2] = { (TH3*)fdata->Get("evde_l0_proton"),
                      (TH3*)fdata->Get("evde_r0_proton") };
  RooUnfoldBayes* unfold[2][10];
  TH1 *heraw[2], *he[2][10];
  for (int i = 0; i < 2; ++i) {
    hevdevt[i]->GetZaxis()->SetRangeUser(400, 100e3);
    hevdevt[i]->RebinX(4);
    hevdevt[i]->GetXaxis()->SetRangeUser(0, 15e3);
    heraw[i] = hevdevt[i]->Project3D("ex");
    for (int j = 0; j < 10; ++j) {
      unfold[i][j] = new RooUnfoldBayes(TM[i][j], heraw[i], 4);
      he    [i][j] = unfold[i][j]->Hreco();
      he[i][j]->SetLineColor(j);
      he[i][j]->SetStats(false);
      char htitle[256];
      sprintf(htitle,
              "Si%c unfolding as function of proton emission position;E [keV]",
              i == 0 ? 'L' : 'R');
      he[i][j]->SetTitle(htitle);
    }
  }

  gStyle->SetOptStat(11);

  TCanvas* cltm = new TCanvas("cltm", "SiL", 1400, 1000);
  cltm->Divide(3, 3);
  for (int i = 1; i <= 9; ++i) {
    cltm->cd(i);
    TM[0][i]->Hresponse()->Draw("COL");
  }
  cltm->SaveAs("img/sil_tm_targsec.png");
  TCanvas* crtm = new TCanvas("crtm", "SiR", 1400, 1000);
  crtm->Divide(3, 3);
  for (int i = 1; i <= 9; ++i) {
    crtm->cd(i);
    TM[1][i]->Hresponse()->Draw("COL");
  }
  crtm->SaveAs("img/sir_tm_targsec.png");

  TCanvas* cltmp = new TCanvas("cltmp", "SiL");
  for (int i = 1; i <= 9; ++i) {
    TMp[0][i]->Draw("SAME");
    box[i]->Draw();
  }
  cltmp->SaveAs("img/sil_tmp_targsec.png");
  TCanvas* crtmp = new TCanvas("crtmp", "SiR");
  for (int i = 1; i <= 9; ++i) {
    TMp[1][i]->Draw("SAME");
    box[i]->Draw();
  }
  crtmp->SaveAs("img/sir_tmp_targsec.png");

  TCanvas* cldm = new TCanvas("cldp", "SiL", 1400, 1000);
  cldm->Divide(3, 3);
  for (int i = 1; i <= 9; ++i) {
    cldm->cd(i);
    DM[0][i]->Draw("COL");
  }
  cldm->SaveAs("img/sil_dm_targsec.png");
  TCanvas* crdm = new TCanvas("crdp", "SiR", 1400, 1000);
  crdm->Divide(3, 3);
  for (int i = 1; i <= 9; ++i) {
    crdm->cd(i);
    DM[1][i]->Draw("COL");
  }
  crdm->SaveAs("img/sir_dm_targsec.png");

  TCanvas* cldp = new TCanvas("cldmp", "SiL");
  for (int i = 1; i<= 9; ++i) {
    DMp[0][i]->Draw("SAME");
    box[i]->Draw();
  }
  cldp->SaveAs("img/sil_dp_targsec.png");
  TCanvas* crdp = new TCanvas("crdmp", "SiR");
  for (int i = 1; i<= 9; ++i) {
    DMp[1][i]->Draw("SAME");
    box[i]->Draw();
  }
  crdp->SaveAs("img/sir_dp_targsec.png");

  TCanvas* cle = new TCanvas("cle", "SiL");
  for (int i = 1; i <= 9; ++i) {
    he[0][i]->Draw("SAME");
    box[i]->Draw();
  }
  cle->SaveAs("img/sil_e_targsec.png");
  TCanvas* cre = new TCanvas("cre", "SiR");
  for (int i = 1; i <= 9; ++i) {
    he[1][i]->Draw("SAME");
    box[i]->Draw();
  }
  cre->SaveAs("img/sir_e_targsec.png");
}
