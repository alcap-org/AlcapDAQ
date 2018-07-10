{
  TFile* ftm   = new TFile("data/tmal50p.root");
  TFile* fdata = new TFile("data/pselal50.root");
  TList* leg = new TList();
  leg->Add(new TPave(0.6, 0.8, 0.7, 0.9, 0, "NB NDC"));
  leg->Add(new TPave(0.7, 0.8, 0.8, 0.9, 0, "NB NDC"));
  leg->Add(new TPave(0.8, 0.8, 0.9, 0.9, 0, "NB NDC"));
  leg->Add(new TPave(0.6, 0.7, 0.7, 0.8, 0, "NB NDC"));
  leg->Add(new TPave(0.7, 0.7, 0.8, 0.8, 0, "NB NDC"));
  leg->Add(new TPave(0.8, 0.7, 0.9, 0.8, 0, "NB NDC"));
  leg->Add(new TPave(0.6, 0.6, 0.7, 0.7, 0, "NB NDC"));
  leg->Add(new TPave(0.7, 0.6, 0.8, 0.7, 0, "NB NDC"));
  leg->Add(new TPave(0.8, 0.6, 0.9, 0.7, 0, "NB NDC"));
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
  THStack* TMp[2] = {
    new THStack("TMprojsl", "Left Relative Efficiency;E_{0} [keV];~Norm"),
    new THStack("TMprojsr", "Right Relative Efficiency;E_{0} [keV];~Norm")
  };
  THStack* DMp[2] = {
    new THStack("DMprofsl",
                "MC Left Measured Differences;E_{0} [keV];E_{0}-E_{meas}"),
    new THStack("DMprofsr",
                "MC Right Measured Differences;E_{0} [keV];E_{0}-E_{meas}")
  };
  THStack* E[2] = {
    new THStack("hel", "SiL unfolding as function of proton emission;E [keV]"),
    new THStack("her", "SiR unfolding as function of proton emission;E [keV]")
  };
  TH3* hevdevt[2] = {
    (TH3*)fdata->Get("evde_l0_proton"),
    (TH3*)fdata->Get("evde_r0_proton")
  };
  RooUnfoldBayes* unfold[2][10];
  TH1* heraw[2];
  for (int i = 0; i < 2; ++i) {
    hevdevt[i]->GetXaxis()->SetRangeUser(0, 15e3);
    hevdevt[i]->GetZaxis()->SetRangeUser(400, 100e3);
    hevdevt[i]->RebinX(4);
    heraw[i] = hevdevt[i]->Project3D("ex");
    for (int j = 1; j < 10; ++j) {
      unfold[i][j] = new RooUnfoldBayes(TM[i][j], heraw[i], 4);
      TH1* htmp    = TM[i][j]->Hresponse()->ProjectionY();
      TH1* hdmp    = DMp[i][j] = DM[i][j]->ProfileX();
      TH1* he      = unfold[i][j]->Hreco();
      TMp[i]  ->Add(htmp);
      DMp[i]  ->Add(hdmp);
      E[i]    ->Add(he);
      htmp    ->SetLineColor(j);
      hdmp    ->SetLineColor(j);
      he      ->SetLineColor(j);
      box[j]  ->SetFillColor(j);
      TM[i][j]->SetTitle("Response;E_{meas} [keV];E_{0} [keV]");
      DM[i][j]->SetTitle("MC Measured Differences;E_{0} [keV];E_{0}-E_{meas} [keV]");
      DM[i][j]->GetYaxis()->SetRangeUser(0, 3e3);
    }
  }

  gStyle->SetOptStat(11);

  TCanvas* ce = new TCanvas("ce", "Si", 1400, 500);
  ce->Divide(2);
  ce->cd(1); E[0]->Draw("nostack"); leg->Draw();
  ce->cd(2); E[1]->Draw("nostack"); leg->Draw();
  ce->SaveAs("img/tm_lateral.pdf(");

  TCanvas* ctm[2] = {
    new TCanvas("cltm", "SiL", 1400, 1000),
    new TCanvas("crtm", "SiR", 1400, 1000)
  };
  for (int i = 0; i < 2; ++i) {
    ctm[i]->Divide(3, 3);
    for (int j = 1; j <= 9; ++j) {
      ctm[i]->cd(j);
      TM[i][j]->Hresponse()->Draw("COL");
    }
    ctm[i]->SaveAs("img/tm_lateral.pdf");
  }

  TCanvas* ctmp = new TCanvas("ctmp", "Si", 1400, 500);
  ctmp->Divide(2);
  ctmp->cd(1); TMp[0]->Draw("nostack"); leg->Draw();
  ctmp->cd(2); TMp[1]->Draw("nostack"); leg->Draw();
  ctmp->SaveAs("img/tm_lateral.pdf");

  TCanvas* cdm[2] = {
    new TCanvas("cldm", "SiL", 1400, 1000),
    new TCanvas("crdm", "SiR", 1400, 1000)
  };
  for (int i = 0; i < 2; ++i) {
    cdm[i]->Divide(3, 3);
    for (int j = 1; j <= 9; ++j) {
      cdm[i]->cd(j);
      DM[i][j]->Draw("COL");
    }
    cdm[i]->SaveAs("img/tm_later.pdf");
  }

  TCanvas* cdp = new TCanvas("cdmp", "Si", 1400, 500);
  cdp->Divide(2);
  cdp->cd(1); DMp[0]->Draw("nostack"); leg[0]->Draw();
  cdp->cd(2); DMp[1]->Draw("nostack"); leg[1]->Draw();
  cdp->SaveAs("img/tm_lateral).pdf)");
}
