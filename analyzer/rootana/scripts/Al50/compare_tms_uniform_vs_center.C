{
	TFile* ftm_uni = new TFile("~/data/R15b/al50ptm_1.root");
  TFile* ftm_pnt = new TFile("~/data/R15b/al50ptmpnt_1.root");
  TFile* fdata   = new TFile("~/data/R15b/pselal50.root");
  RooUnfoldResponse* TM[2][2] =
  { { (RooUnfoldResponse*)ftm_uni->Get("SiL_TM"), (RooUnfoldResponse*)ftm_uni->Get("SiR_TM") },
    { (RooUnfoldResponse*)ftm_pnt->Get("SiL_TM"), (RooUnfoldResponse*)ftm_pnt->Get("SiR_TM") } };
  TH2* htm[2][2] =
  { { TM[0][0]->Hresponse(), TM[0][1]->Hresponse() },
    { TM[1][0]->Hresponse(), TM[1][1]->Hresponse() } };
  TH3* hevdevt[2] = { (TH3*)fdata->Get("evde_l0_proton"),
                      (TH3*)fdata->Get("evde_r0_proton") };
  hevdevt[0]->GetZaxis()->SetRangeUser(400, 100e3); // Needed for t-cut in
  hevdevt[1]->GetZaxis()->SetRangeUser(400, 100e3); // projection.
  hevdevt[0]->RebinX(4);
  hevdevt[1]->RebinX(4);
  hevdevt[0]->GetXaxis()->SetRangeUser(0, 15e3);
  hevdevt[1]->GetXaxis()->SetRangeUser(0, 15e3);
  TH1* heraw[2]            = { hevdevt[0]->Project3D("ex"),
                               hevdevt[1]->Project3D("ex") };

  RooUnfoldBayes* resp[2][2] =
  { { new RooUnfoldBayes(TM[0][0], heraw[0], 4), new RooUnfoldBayes(TM[0][1], heraw[1], 4) },
    { new RooUnfoldBayes(TM[1][0], heraw[0], 4), new RooUnfoldBayes(TM[1][1], heraw[1], 4) } };
  TH1* he[2][2] = { { resp[0][0]->Hreco(), resp[0][1]->Hreco() },
                    { resp[1][0]->Hreco(), resp[1][1]->Hreco() } };

  htm[0][0]->SetTitle("Uniform-in-Target Proton Generation (Left);E_{meas} [keV];E_{true} [keV]");
  htm[0][1]->SetTitle("Uniform-in-Target Proton Generation (Right);E_{meas} [keV];E_{true} [keV]");
  htm[1][0]->SetTitle("Center-of-Target Proton Generation (Left);E_{meas} [keV];E_{true} [keV]");
  htm[1][1]->SetTitle("Center-of-Target Proton Generation (Right);E_{meas} [keV];E_{true} [keV]");
  he[0][0]->SetTitle("Reconstructed (Left);E [keV]");
  he[0][1]->SetTitle("Reconstructed (Right);E [keV]");
  he[0][0]->GetXaxis()->SetRangeUser(2e3, 15e3);
  he[0][1]->GetXaxis()->SetRangeUser(2e3, 15e3);
  he[0][0]->SetLineColor(kBlue);
  he[0][1]->SetLineColor(kBlue);
  he[1][0]->SetLineColor(kRed);
  he[1][1]->SetLineColor(kRed);
  he[0][0]->SetStats(false);
  he[0][1]->SetStats(false);
  he[1][0]->SetStats(false);
  he[1][1]->SetStats(false);

  TCanvas* ctm = new TCanvas("ctm", "Transfer Matrices", 1400, 1000);
  ctm->Divide(2, 2);
  ctm->cd(1);
  htm[0][0]->Draw("COL");
  ctm->cd(2);
  htm[0][1]->Draw("COL");
  ctm->cd(3);
  htm[1][0]->Draw("COL");
  ctm->cd(4);
  htm[1][1]->Draw("COL");
  ctm->SaveAs("img/pointVuniform_tm.png");
  TCanvas* ce = new TCanvas("ce", "Unfolded Spectrum", 1400, 500);
  TLegend* leg = new TLegend(0.55, 0.7, 0.9, 0.9);
  leg->AddEntry(he[0][0], "Uniform in target");
  leg->AddEntry(he[1][0], "Center point in target");
  ce->Divide(2);
  ce->cd(1);
  he[0][0]->Draw();
  he[1][0]->Draw("SAME");
  leg->Draw();
  ce->cd(2);
  he[0][1]->Draw();
  he[1][1]->Draw("SAME");
  leg->Draw();
  ce->SaveAs("img/pointVuniform_e.png");

  std::cout << "Left:\t"  << he[0][0]->KolmogorovTest(he[1][0]) << std::endl;
  std::cout << "Right:\t" << he[0][1]->KolmogorovTest(he[1][1]) << std::endl;
}
