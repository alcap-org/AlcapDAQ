{
  TFile* fmw = new TFile("data/Al50/from_Mark/unfolded.transfer.proton.localhisto.7m.multiplebins.root");
  TFile* fjq = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.elo2e3keV.ehi17e3keV.root");

  TH1* hmw[2] = { (TH1*)fmw->Get("hRecoLeft"),     (TH1*)fmw->Get("hRecoRight")    };
  TH1* hjq[2] = { (TH1*)fjq->Get("hl_e_reco_bay"), (TH1*)fjq->Get("hr_e_reco_bay") };

  hmw[0]->SetLineColor(kBlue);
  hmw[1]->SetLineColor(kRed);
  hjq[0]->SetLineColor(kBlue);
  hjq[1]->SetLineColor(kRed);
  hjq[0]->SetLineStyle(2);
  hjq[1]->SetLineStyle(2);

  hmw[0]->SetTitle("SiL Reconstructed Proton Spectrum;E [keV];counts [/500keV]");
  hmw[1]->SetTitle("SiR Reconstructed Proton Spectrum;E [keV];counts [/500keV]");
  hjq[0]->SetTitle("SiL Reconstructed Proton Spectrum;E [keV];counts [/500keV]");
  hjq[1]->SetTitle("SiR Reconstructed Proton Spectrum;E [keV];counts [/500keV]");

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hmw[0], "Mark");
  l->AddEntry(hjq[0], "John");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 700);
  c->Divide(2);
  c->cd(1);
  hjq[0]->Draw("SAME");
  hmw[0]->Draw("SAME");
  l->Draw();
  c->cd(2);
  hjq[1]->Draw("SAME");
  hmw[1]->Draw("SAME");

  double nmw_all[2]   = {0, 0};
  double enmw_all[2]  = {0, 0};
  double nmw_roi1[2]  = {0, 0};
  double enmw_roi1[2] = {0, 0};
  double nmw_roi2[2]  = {0, 0};
  double enmw_roi2[2] = {0, 0};
  double njq_all[2]   = {0, 0};
  double enjq_all[2]  = {0, 0};
  double njq_roi1[2]  = {0, 0};
  double enjq_roi1[2] = {0, 0};
  double njq_roi2[2]  = {0, 0};
  double enjq_roi2[2] = {0, 0};

  nmw_all[0]  = hmw[0]->IntegralAndError(1,                         hmw[0]->GetNbinsX(),      enmw_all[0]);
  nmw_all[1]  = hmw[1]->IntegralAndError(1,                         hmw[1]->GetNbinsX(),      enmw_all[1]);
  nmw_roi1[0] = hmw[0]->IntegralAndError(hmw[0]->FindFixBin(3.5e3), hmw[0]->FindFixBin(10e3), enmw_roi1[0]);
  nmw_roi1[1] = hmw[1]->IntegralAndError(hmw[1]->FindFixBin(3.5e3), hmw[1]->FindFixBin(10e3), enmw_roi1[1]);
  nmw_roi2[0] = hmw[0]->IntegralAndError(hmw[0]->FindFixBin(4e3),   hmw[0]->FindFixBin(8e3),  enmw_roi2[0]);
  nmw_roi2[1] = hmw[1]->IntegralAndError(hmw[1]->FindFixBin(4e3),   hmw[1]->FindFixBin(8e3),  enmw_roi2[1]);
  njq_all[0]  = hjq[0]->IntegralAndError(1,                         hjq[0]->GetNbinsX(),      enjq_all[0]);
  njq_all[1]  = hjq[1]->IntegralAndError(1,                         hjq[1]->GetNbinsX(),      enjq_all[1]);
  njq_roi1[0] = hjq[0]->IntegralAndError(hjq[0]->FindFixBin(3.5e3), hjq[0]->FindFixBin(10e3), enjq_roi1[0]);
  njq_roi1[1] = hjq[1]->IntegralAndError(hjq[1]->FindFixBin(3.5e3), hjq[1]->FindFixBin(10e3), enjq_roi1[1]);
  njq_roi2[0] = hjq[0]->IntegralAndError(hjq[0]->FindFixBin(4e3),   hjq[0]->FindFixBin(8e3),  enjq_roi2[0]);
  njq_roi2[1] = hjq[1]->IntegralAndError(hjq[1]->FindFixBin(4e3),   hjq[1]->FindFixBin(8e3),  enjq_roi2[1]);

  printf("Mark--SiL--SiR---\n");
  printf("All: %g +/- %g \t %g +/- %g\n",  nmw_all[0],  enmw_all[0],  nmw_all[1],  enmw_all[1]);
  printf("ROI1: %g +/- %g \t %g +/- %g\n", nmw_roi1[0], enmw_roi1[0], nmw_roi1[1], enmw_roi1[1]);
  printf("ROI2: %g +/- %g \t %g +/- %g\n", nmw_roi2[0], enmw_roi2[0], nmw_roi2[1], enmw_roi2[1]);
  printf("John--SiL--SiR---\n");
  printf("All: %g +/- %g \t %g +/- %g\n",  njq_all[0],  enjq_all[0],  njq_all[1],  enjq_all[1]);
  printf("ROI1: %g +/- %g \t %g +/- %g\n", njq_roi1[0], enjq_roi1[0], njq_roi1[1], enjq_roi1[1]);
  printf("ROI2: %g +/- %g \t %g +/- %g\n", njq_roi2[0], enjq_roi2[0], njq_roi2[1], enjq_roi2[1]);

}
