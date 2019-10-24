#include "./scripts/Al50/util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

#include <iostream>

void compare_mark_john_unfold() {

  TFile* fjq   = new TFile("data/Al50/tm.p.2layer.root");
  TFile* fmw   = new TFile("data/m-wong/transfer.proton.localhisto.7m.Al50.root");
  TFile* fdata = new TFile("data/Al50/psel.3layer.pidpow2.root");

  RooUnfoldResponse* tmjq[2] = { (RooUnfoldResponse*)fjq->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fjq->Get("SiR_TM") };
  RooUnfoldResponse* tmmw[2] = { (RooUnfoldResponse*)fmw->Get("SiL500_TM"),
                                 (RooUnfoldResponse*)fmw->Get("SiR500_TM") };
  TTree* data[2] = { (TTree*)fdata->Get("PID_LP"),
                     (TTree*)fdata->Get("PID_RP") };
  data[0]->Draw("e>>hlraw(26,2e3,15e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  data[1]->Draw("e>>hrraw(26,2e3,15e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  TH1* hraw[2] = { (TH1*)gDirectory->Get("hlraw"),
                   (TH1*)gDirectory->Get("hrraw") };
  TMUtils::Rebin(tmjq[0], 5);
  TMUtils::Rebin(tmjq[1], 5);
  TMUtils::SetRange(tmjq[0], 2e3, 15e3);
  TMUtils::SetRange(tmjq[1], 2e3, 15e3);
  TMUtils::SetRange(tmmw[0], 2e3, 15e3);
  TMUtils::SetRange(tmmw[1], 2e3, 15e3);
  TH2* htmjq[2]   = { tmjq[0]->Hresponse(), tmjq[1]->Hresponse() };
  TH2* htmmw[2]   = { tmmw[0]->Hresponse(), tmmw[1]->Hresponse() };
  TH1* hjqtrue[2] = { tmjq[0]->Htruth(),    tmjq[1]->Htruth() };
  TH1* hmwtrue[2] = { tmmw[0]->Htruth(),    tmmw[1]->Htruth() };
  TH1* hjqmeas[2] = { tmjq[0]->Hmeasured(), tmjq[1]->Hmeasured() };
  TH1* hmwmeas[2] = { tmmw[0]->Hmeasured(), tmmw[1]->Hmeasured() };
  TH1* hjqratio[2] = { (TH1*)hjqmeas[0]->Clone("hjqlratio"), (TH1*)hjqmeas[1]->Clone("hjqrratio")};
  TH1* hmwratio[2] = { (TH1*)hmwmeas[0]->Clone("hmwlratio"), (TH1*)hmwmeas[1]->Clone("hmwrratio")};
  hjqratio[0]->Divide(hjqtrue[0]);
  hjqratio[1]->Divide(hjqtrue[1]);
  hmwratio[0]->Divide(hmwtrue[0]);
  hmwratio[1]->Divide(hmwtrue[1]);
  RooUnfoldBayes* unfjq[2] = { new RooUnfoldBayes(tmjq[0], hraw[0], 4),
                               new RooUnfoldBayes(tmjq[1], hraw[1], 4) };
  RooUnfoldBayes* unfmw[2] = { new RooUnfoldBayes(tmmw[0], hraw[0], 4),
                               new RooUnfoldBayes(tmmw[1], hraw[1], 4) };
  TH1* ejq[2] = { unfjq[0]->Hreco(), unfjq[1]->Hreco() };
  TH1* emw[2] = { unfmw[0]->Hreco(), unfmw[1]->Hreco() };

  hjqtrue[0] ->SetLineColor(kBlue);
  hjqtrue[1] ->SetLineColor(kBlue);
  hmwtrue[0] ->SetLineColor(kBlack);
  hmwtrue[1] ->SetLineColor(kBlack);
  hjqmeas[0] ->SetLineColor(kBlue);
  hjqmeas[1] ->SetLineColor(kBlue);
  hmwmeas[0] ->SetLineColor(kBlack);
  hmwmeas[1] ->SetLineColor(kBlack);
  hjqratio[0]->SetLineColor(kBlue);
  hjqratio[1]->SetLineColor(kBlue);
  hmwratio[0]->SetLineColor(kBlack);
  hmwratio[1]->SetLineColor(kBlack);
  hraw[0]    ->SetLineColor(kBlue);
  hraw[1]    ->SetLineColor(kRed);
  ejq[0]     ->SetLineColor(kBlue);
  ejq[1]     ->SetLineColor(kBlue);
  emw[0]     ->SetLineColor(kBlack);
  emw[1]     ->SetLineColor(kBlack);


  htmjq[0]->Scale(1./hjqtrue[0]->Integral());
  htmjq[1]->Scale(1./hjqtrue[1]->Integral());
  htmmw[0]->Scale(1./hmwtrue[0]->Integral());
  htmmw[1]->Scale(1./hmwtrue[1]->Integral());
  hjqmeas[0]->Scale(1./hjqtrue[0]->Integral());
  hmwmeas[0]->Scale(1./hmwtrue[0]->Integral());
  hjqmeas[1]->Scale(1./hjqtrue[1]->Integral());
  hmwmeas[1]->Scale(1./hmwtrue[1]->Integral());
  hjqtrue[0]->Scale(1./hjqtrue[0]->Integral());
  hmwtrue[0]->Scale(1./hmwtrue[0]->Integral());
  htmjq[0]->SetMaximum(0.36e-3);
  htmjq[1]->SetMaximum(0.36e-3);
  htmmw[0]->SetMaximum(0.36e-3);
  htmmw[1]->SetMaximum(0.36e-3);
  htmjq[0]->SetTitle("SiL John TM (Norm);E_{meas} [keV];E_{true} [keV];/500keV/500keV");
  htmjq[1]->SetTitle("SiR John TM (Norm);E_{meas} [keV];E_{true} [keV];/500keV/500keV");
  htmmw[0]->SetTitle("SiL Mark TM (Norm);E_{meas} [keV];E_{true} [keV];/500keV/500keV");
  htmmw[1]->SetTitle("SiR Mark TM (Norm);E_{meas} [keV];E_{true} [keV];/500keV/500keV");
  hjqtrue[0]->SetTitle("MC Truth (Norm);Proton E [keV];Counts/500 keV");
  hjqtrue[1]->SetTitle("MC Truth (Norm);Proton E [keV];Counts/500 keV");
  hmwtrue[0]->SetTitle("MC Truth (Norm);Proton E [keV];Counts/500 keV");
  hmwtrue[1]->SetTitle("MC Truth (Norm);Proton E [keV];Counts/500 keV");
  hjqmeas[0]->SetTitle("MC Measured (SiL Norm);Proton E [keV];Counts/500 keV");
  hjqmeas[1]->SetTitle("MC Measured (SiR Norm);Proton E [keV];Counts/500 keV");
  hmwmeas[0]->SetTitle("MC Measured (SiL Norm);Proton E [keV];Counts/500 keV");
  hmwmeas[1]->SetTitle("MC Measured (SiR Norm);Proton E [keV];Counts/500 keV");
  hjqratio[0]->SetTitle("MC Meas/True (SiL);Proton E [keV];/500 keV");
  hjqratio[1]->SetTitle("MC Meas/True (SiR);Proton E [keV];/500 keV");
  hmwratio[0]->SetTitle("MC Meas/True (SiL);Proton E [keV];/500 keV");
  hmwratio[1]->SetTitle("MC Meas/True (SiR);Proton E [keV];/500 keV");
  hraw[0]->SetTitle("Raw Proton Energies;E [keV];Counts/500 keV");
  hraw[1]->SetTitle("Raw Proton Energies;E [keV];Counts/500 keV");
  ejq[0]->SetTitle("SiL Proton Spectrum;E [keV];Unfolded counts [p/500 keV]");
  ejq[1]->SetTitle("SiR Proton Spectrum;E [keV];Unfolded counts [p/500 keV]");
  emw[0]->SetTitle("SiL Proton Spectrum;E [keV];Unfolded counts [p/500 keV]");
  emw[1]->SetTitle("SiR Proton Spectrum;E [keV];Unfolded counts [p/500 keV]");

  TLegend* legraw = new TLegend(0.7, 0.7, 0.9, 0.9);
  legraw->AddEntry(hraw[0], "SiL");
  legraw->AddEntry(hraw[1], "SiR");
  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(ejq[0], "John");
  leg->AddEntry(emw[0], "Mark");

  gStyle->SetOptStat(0);

  TCanvas* ctm = new TCanvas("ctm", "ctm", 1400, 1000);
  ctm->Divide(2,2);
  ctm->cd(1); htmjq[0]->Draw("COLZ");
  ctm->cd(2); htmjq[1]->Draw("COLZ");
  ctm->cd(3); htmmw[0]->Draw("COLZ");
  ctm->cd(4); htmmw[1]->Draw("COLZ");
  ctm->SaveAs("img/mark_john_unfold_tm.png");

  TCanvas* ctrue = new TCanvas("ctrue", "ctrue", 700, 500);
  hmwtrue[0]->Draw("SAME");
  hjqtrue[0]->Draw("SAME");
  ctrue->SaveAs("img/mark_john_unfold_true.png");

  TCanvas* cmeas = new TCanvas("cmeas", "cmeas", 1400, 500);
  hjqmeas[0]->SetMinimum(0);
  hjqmeas[1]->SetMinimum(0);
  hmwmeas[0]->SetMinimum(0);
  hmwmeas[1]->SetMinimum(0);
  cmeas->Divide(2);
  cmeas->cd(1);
  hmwmeas[0]->Draw("SAME");
  hjqmeas[0]->Draw("SAME");
  cmeas->cd(2);
  hjqmeas[1]->Draw("SAME");
  hmwmeas[1]->Draw("SAME");
  cmeas->SaveAs("img/mark_john_unfold_meas.png");

  TCanvas* cratio = new TCanvas("cratio", "cratio", 1400, 500);
  hjqratio[0]->SetMinimum(0);
  hjqratio[1]->SetMinimum(0);
  hmwratio[0]->SetMinimum(0);
  hmwratio[1]->SetMinimum(0);
  cratio->Divide(2);
  cratio->cd(1);
  hmwratio[0]->Draw("SAME");
  hjqratio[0]->Draw("SAME");
  cratio->cd(2);
  hjqratio[1]->Draw("SAME");
  hmwratio[1]->Draw("SAME");
  cratio->SaveAs("img/mark_john_unfold_ratio.png");

  TCanvas* craw = new TCanvas("craw", "craw", 700, 500);
  hraw[1]->Draw("SAME");
  hraw[0]->Draw("SAME");
  legraw ->Draw("SAME");
  craw->SaveAs("img/mark_john_unfold_raw.png");

  TCanvas* ce = new TCanvas("ce", "ce", 1400, 500);
  ce->Divide(2);
  ce->cd(1);
  ejq[0]->Draw("SAME");
  emw[0]->Draw("SAME");
  leg   ->Draw("SAME");
  ce->cd(2);
  ejq[1]->Draw("SAME");
  emw[1]->Draw("SAME");
  leg   ->Draw("SAME");
  ce->SaveAs("img/mark_john_unfold_compare.png");

  double njq[2][2] = {
    { HistUtils::IntegralByUserLimits(ejq[0], 3.5e3, 10e3).first,
      HistUtils::IntegralByUserLimits(ejq[1], 3.5e3, 10e3).first },
    { HistUtils::IntegralByUserLimits(ejq[0], 4e3,   8e3).first,
      HistUtils::IntegralByUserLimits(ejq[1], 4e3,   8e3).first }
  };
  double nmw[2][2] = {
    { HistUtils::IntegralByUserLimits(emw[0], 3.5e3, 10e3).first,
      HistUtils::IntegralByUserLimits(emw[1], 3.5e3, 10e3).first },
    { HistUtils::IntegralByUserLimits(emw[0], 4e3,   8e3).first,
      HistUtils::IntegralByUserLimits(emw[1], 4e3,   8e3).first }
  };

  double norm = 0.63058788*1.6e8*0.61;
  njq[0][0] /= norm; nmw[0][0] /= norm;
  njq[0][1] /= norm; nmw[0][1] /= norm;
  njq[1][0] /= norm; nmw[1][0] /= norm;
  njq[1][1] /= norm; nmw[1][1] /= norm;
  std::cout << njq[0][0] << " " << njq[0][1] << std::endl;
  std::cout << nmw[0][0] << " " << nmw[0][1] << std::endl;
  std::cout << njq[1][0] << " " << njq[1][1] << std::endl;
  std::cout << nmw[1][0] << " " << nmw[1][1] << std::endl;
}
