#include "util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TRatioPlot.h"
#include "TStyle.h"
#include "TTree.h"

void check_effect_of_p_bg_from_si16b() {
  static const int NTME = 421027119; // pp=10us

  TFile* fbg  = new TFile("data/Si16bBackground/forJohn_timecut400ns.root");
  TFile* fraw = new TFile("data/Al50/psel.3layer.root");
  TFile* ftm  = new TFile("data/Al50/tm.p.2layer.root");

  TH1* hbg = (TH1*)fbg->Get("hEvdE_TwoLayer_12not3_px"); // per TME
  TTree* trraw[2] = { (TTree*)fraw->Get("PID_LP"),
                      (TTree*)fraw->Get("PID_RP") };
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)ftm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)ftm->Get("SiR_TM") };

  hbg->Rebin(5);
  HistUtils::SetRange(hbg, 2e3, 15e3);
  trraw[0]->Draw("e1+e2>>hlraw(26,2e3,15e3)", "t>400 && abs(dt)<200 && e3==0.", "goff");
  trraw[1]->Draw("e1+e2>>hrraw(26,2e3,15e3)", "t>400 && abs(dt)<200 && e3==0.", "goff");
  TH1* hraw[2] = { (TH1*)gDirectory->Get("hlraw"),
                   (TH1*)gDirectory->Get("hrraw") };
  TMUtils::Rebin   (resp[0], 5);
  TMUtils::Rebin   (resp[1], 5);
  TMUtils::SetRange(resp[0], 2e3, 15e3);
  TMUtils::SetRange(resp[1], 2e3, 15e3);
  TH1* hrawbg[2] = { (TH1*)hraw[0]->Clone("hlrawbg"),
                     (TH1*)hraw[1]->Clone("hrrawbg") };
  hrawbg[0]->Add(hbg, -NTME); // BG subtract, scale BG to number of TMEs
  hrawbg[1]->Add(hbg, -NTME); // BG subtract, scale BG to number of TMEs

  RooUnfoldBayes* unf[2]   = { new RooUnfoldBayes(resp[0], hraw[0],   4),
                               new RooUnfoldBayes(resp[1], hraw[1],   4) };
  RooUnfoldBayes* unfbg[2] = { new RooUnfoldBayes(resp[0], hrawbg[0], 4),
                               new RooUnfoldBayes(resp[1], hrawbg[1], 4) };
  TH1* he[2]   = { unf[0]->Hreco(),   unf[1]->Hreco() };
  TH1* hebg[2] = { unfbg[0]->Hreco(), unfbg[1]->Hreco() };
  he  [0]->Scale(1./Normalization::Al50(400));
  he  [1]->Scale(1./Normalization::Al50(400));
  hebg[0]->Scale(1./Normalization::Al50(400));
  hebg[1]->Scale(1./Normalization::Al50(400));

  he  [0]->SetLineColor(kBlue);
  he  [1]->SetLineColor(kRed);
  hebg[0]->SetLineColor(kBlue);
  hebg[1]->SetLineColor(kRed);
  hebg[0]->SetLineStyle(7);
  hebg[1]->SetLineStyle(7);
  he  [0]->SetTitle("SiL Proton Spectrum;Energy [keV];Rate [p/#mu]");
  he  [1]->SetTitle("SiR Proton Spectrum;Energy [keV];Rate [p/#mu]");

  // TRatioPlot* rp[2];
  // TCanvas* c = new TCanvas("c", "c", 1400, 500);
  // c->Divide(2);
  // c->cd(1); rp[0] = new TRatioPlot(he[0], hebg[0], "diffsig pois"); rp[0]->Draw();
  // c->cd(2); rp[1] = new TRatioPlot(he[1], hebg[1], "diffsig pois"); rp[1]->Draw();

  TLegend* leg[2] = { new TLegend(0.7, 0.7, 0.9, 0.9),
                      new TLegend(0.7, 0.7, 0.9, 0.9) };
  leg[0]->AddEntry(he  [0], "canonical");
  leg[0]->AddEntry(hebg[0], "bg sub.");
  leg[1]->AddEntry(he  [1], "canonical");
  leg[1]->AddEntry(hebg[1], "bg sub.");

  gStyle->SetOptStat(false);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1); he[0]->Draw("SAME"); hebg[0]->Draw("SAME"); leg[0]->Draw();
  c->cd(2); he[1]->Draw("SAME"); hebg[1]->Draw("SAME"); leg[1]->Draw();
  c->SaveAs("img/check_effect_of_p_bg_from_si16b.png");

  std::pair<double,double> roi1[2], roi1bg[2], roi2[2], roi2bg[2];
  roi1  [0] = HistUtils::IntegralByUserLimits(he[0],   3.5e3, 10e3);
  roi1  [1] = HistUtils::IntegralByUserLimits(he[1],   3.5e3, 10e3);
  roi2  [0] = HistUtils::IntegralByUserLimits(he[0],   4e3,   8e3);
  roi2  [1] = HistUtils::IntegralByUserLimits(he[1],   4e3,   8e3);
  roi1bg[0] = HistUtils::IntegralByUserLimits(hebg[0], 3.5e3, 10e3);
  roi1bg[1] = HistUtils::IntegralByUserLimits(hebg[1], 3.5e3, 10e3);
  roi2bg[0] = HistUtils::IntegralByUserLimits(hebg[0], 4e3,   8e3);
  roi2bg[1] = HistUtils::IntegralByUserLimits(hebg[1], 4e3,   8e3);
  printf("        SiL                    SiR\n");
  printf("ROI1:   %f +/- %f, %f +/- %f\n", roi1  [0].first, roi1  [0].second, roi1  [1].first, roi1  [1].second);
  printf("ROI1BG: %f +/- %f, %f +/- %f\n", roi1bg[0].first, roi1bg[0].second, roi1bg[1].first, roi1bg[1].second);
  printf("ROI2:   %f +/- %f, %f +/- %f\n", roi2  [0].first, roi2  [0].second, roi2  [1].first, roi2  [1].second);
  printf("ROI2BG: %f +/- %f, %f +/- %f\n", roi2bg[0].first, roi2bg[0].second, roi2bg[1].first, roi2bg[1].second);
}
