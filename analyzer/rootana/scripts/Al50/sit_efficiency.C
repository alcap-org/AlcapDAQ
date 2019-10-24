#include "TCanvas.h"
#include "TChain.h"
#include "TCut.h"
#include "TF1.h"
#include "TH1.h"
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"

#include <cstdio>

TH1* CreateSiL1MuonStopTimeDistribution(TTree* tr) {
  TCut cut("@SiT_1.size()+@SiT_2.size()+@SiT_3.size()+@SiT_4.size() == 1");
  TH1* h = new TH1D("hsil1t_stops", "Si16b SiL1 Stops Time;t [ns];cnts/10ns",
                    1000, 0, 10e3);
  tr->Draw("SiL1_2.tTME>>+hsil1t_stops",  cut && "500 < SiL1_2.E  && SiL1_2.E  < 1500");
  tr->Draw("SiL1_3.tTME>>+hsil1t_stops",  cut && "500 < SiL1_3.E  && SiL1_3.E  < 1500");
  tr->Draw("SiL1_4.tTME>>+hsil1t_stops",  cut && "500 < SiL1_4.E  && SiL1_4.E  < 1500");
  tr->Draw("SiL1_5.tTME>>+hsil1t_stops",  cut && "500 < SiL1_5.E  && SiL1_5.E  < 1500");
  tr->Draw("SiL1_6.tTME>>+hsil1t_stops",  cut && "500 < SiL1_6.E  && SiL1_6.E  < 1500");
  tr->Draw("SiL1_7.tTME>>+hsil1t_stops",  cut && "500 < SiL1_7.E  && SiL1_7.E  < 1500");
  tr->Draw("SiL1_8.tTME>>+hsil1t_stops",  cut && "500 < SiL1_8.E  && SiL1_8.E  < 1500");
  tr->Draw("SiL1_9.tTME>>+hsil1t_stops",  cut && "500 < SiL1_9.E  && SiL1_9.E  < 1500");
  tr->Draw("SiL1_10.tTME>>+hsil1t_stops", cut && "500 < SiL1_10.E && SiL1_10.E < 1500");
  tr->Draw("SiL1_11.tTME>>+hsil1t_stops", cut && "500 < SiL1_11.E && SiL1_11.E < 1500");
  tr->Draw("SiL1_12.tTME>>+hsil1t_stops", cut && "500 < SiL1_12.E && SiL1_12.E < 1500");
  tr->Draw("SiL1_13.tTME>>+hsil1t_stops", cut && "500 < SiL1_13.E && SiL1_13.E < 1500");
  tr->Draw("SiL1_14.tTME>>+hsil1t_stops", cut && "500 < SiL1_14.E && SiL1_14.E < 1500");
  tr->Draw("SiL1_15.tTME>>+hsil1t_stops", cut && "500 < SiL1_15.E && SiL1_15.E < 1500");
  return h;
}

TH1* CreateAndFitSiTEnergyHistogram(TTree* tr) {
  TH1* h = new TH1D("hsite",
                    "Si16b Central Muon (SiT) Energy;E [keV];#mu/10 keV",
                    200, 0, 2e3);
  tr->Draw("centralMuonEnergy>>+hsite", "", "goff");
  h->GetXaxis()->SetRangeUser(250, 400);
  TF1* fit = new TF1("fit", "gaus(0)");
  h->Fit(fit, "ME");
  h->GetXaxis()->UnZoom();
  return h;
}

TH1* CreateSiL1TMEEnergyHistogram(TTree* tr) {
  TCut cut("@SiT_1.size()+@SiT_2.size()+@SiT_3.size()+@SiT_4.size() == 1");
  TH1* h = new TH1D("hsil1e_tme", "Si16b SiL1 Energy;E [keV];cts/10 keV",
                    500, 0, 5e3);
  tr->Draw("SiL1_2.E>>+hsil1e_tme",  cut && "abs(SiL1_2.tTME) <= 200", "goff");
  tr->Draw("SiL1_3.E>>+hsil1e_tme",  cut && "abs(SiL1_3.tTME) <= 200", "goff");
  tr->Draw("SiL1_4.E>>+hsil1e_tme",  cut && "abs(SiL1_4.tTME) <= 200", "goff");
  tr->Draw("SiL1_5.E>>+hsil1e_tme",  cut && "abs(SiL1_5.tTME) <= 200", "goff");
  tr->Draw("SiL1_6.E>>+hsil1e_tme",  cut && "abs(SiL1_6.tTME) <= 200", "goff");
  tr->Draw("SiL1_7.E>>+hsil1e_tme",  cut && "abs(SiL1_7.tTME) <= 200", "goff");
  tr->Draw("SiL1_8.E>>+hsil1e_tme",  cut && "abs(SiL1_8.tTME) <= 200", "goff");
  tr->Draw("SiL1_9.E>>+hsil1e_tme",  cut && "abs(SiL1_9.tTME) <= 200", "goff");
  tr->Draw("SiL1_10.E>>+hsil1e_tme", cut && "abs(SiL1_10.tTME) <= 200", "goff");
  tr->Draw("SiL1_11.E>>+hsil1e_tme", cut && "abs(SiL1_11.tTME) <= 200", "goff");
  tr->Draw("SiL1_12.E>>+hsil1e_tme", cut && "abs(SiL1_12.tTME) <= 200", "goff");
  tr->Draw("SiL1_13.E>>+hsil1e_tme", cut && "abs(SiL1_13.tTME) <= 200", "goff");
  tr->Draw("SiL1_14.E>>+hsil1e_tme", cut && "abs(SiL1_14.tTME) <= 200", "goff");
  tr->Draw("SiL1_15.E>>+hsil1e_tme", cut && "abs(SiL1_15.tTME) <= 200", "goff");
  return h;
}

TH1* CreateSiL1TMEOOTEnergyHistogram(TTree* tr) {
  TCut cut("@SiT_1.size()+@SiT_2.size()+@SiT_3.size()+@SiT_4.size() == 1");
  TH1* h = new TH1D("hsil1e_tme_oot", "SiL1 Energy;E [keV];cts/10 keV",
                    500, 0, 5e3);
  tr->Draw("SiL1_2.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_2.tTME) > 200",  "goff");
  tr->Draw("SiL1_3.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_3.tTME) > 200",  "goff");
  tr->Draw("SiL1_4.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_4.tTME) > 200",  "goff");
  tr->Draw("SiL1_5.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_5.tTME) > 200",  "goff");
  tr->Draw("SiL1_6.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_6.tTME) > 200",  "goff");
  tr->Draw("SiL1_7.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_7.tTME) > 200",  "goff");
  tr->Draw("SiL1_8.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_8.tTME) > 200",  "goff");
  tr->Draw("SiL1_9.E>>+hsil1e_tme_oot",  cut && "abs(SiL1_9.tTME) > 200",  "goff");
  tr->Draw("SiL1_10.E>>+hsil1e_tme_oot", cut && "abs(SiL1_10.tTME) > 200", "goff");
  tr->Draw("SiL1_11.E>>+hsil1e_tme_oot", cut && "abs(SiL1_11.tTME) > 200", "goff");
  tr->Draw("SiL1_12.E>>+hsil1e_tme_oot", cut && "abs(SiL1_12.tTME) > 200", "goff");
  tr->Draw("SiL1_13.E>>+hsil1e_tme_oot", cut && "abs(SiL1_13.tTME) > 200", "goff");
  tr->Draw("SiL1_14.E>>+hsil1e_tme_oot", cut && "abs(SiL1_14.tTME) > 200", "goff");
  tr->Draw("SiL1_15.E>>+hsil1e_tme_oot", cut && "abs(SiL1_15.tTME) > 200", "goff");
  return h;
}

TH1* CreateSiL1NonTMEEnergyHistogram(TTree* tr) {
  TH1* h = new TH1D("hsil1e_nontme", "SiL1 Energy;E [keV];cts/10 keV",
                    500, 0, 5e3);
  tr->Draw("SiL1_2.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_3.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_4.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_5.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_6.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_7.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_8.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_9.E>>+hsile_nontme",   "", "goff");
  tr->Draw("SiL1_10.E>>+hsil1e_nontme", "", "goff");
  tr->Draw("SiL1_11.E>>+hsil1e_nontme", "", "goff");
  tr->Draw("SiL1_12.E>>+hsil1e_nontme", "", "goff");
  tr->Draw("SiL1_13.E>>+hsil1e_nontme", "", "goff");
  tr->Draw("SiL1_14.E>>+hsil1e_nontme", "", "goff");
  tr->Draw("SiL1_15.E>>+hsil1e_nontme", "", "goff");
  return h;
}

void PrintSiTEnergyHistogramFitInfo(const TH1* h) {
  TF1* f = h->GetFunction("fit");
  std::printf("Integral: %g %g %g\n",
              f->Integral(-1e6, 200), f->Integral(-1e6, 1e6),
              f->Integral(-1e6, 200)/f->Integral(-1e6, 1e6));
}

void PrintSiL1EnergyHistogramInfo(const TH1* htme, const TH1* hoot,
                                  const TH1* hnon) {
  int bs[2] = { htme->FindFixBin(500), htme->FindFixBin(1500) }; // Si16b
  double ntme = htme->Integral(bs[0], bs[1]);
  double noot = hoot->Integral(bs[0], bs[1]);
  double nnon = hnon->Integral(bs[0], bs[1]);
  std::printf("tme oottme nontme: %g %g %g\n", ntme, noot, nnon);
}

void sit_efficiency() {
  TChain* tr[2] = { new TChain("TMETree/TMETree"),
                    new TChain("TMETree/NonTMETree") };
  // tral50[0]->Add("~/R15bTME/Al50/tme*.root");
  // tral50[1]->Add("~/R15bTME/Al50/tme*.root");
  // tral50[0]->Add("~/R15bTME/Al50/tme09890.root");
  // tral50[1]->Add("~/R15bTME/Al50/tme09890.root");
  // trsi16[0]->Add("~/R15bTME/Si16b/out*.root");
  // trsi16[1]->Add("~/R15bTME/Si16b/out*.root");
  tr[0]->Add("~/R15bTME/Si16b/out10362.root");
  tr[1]->Add("~/R15bTME/Si16b/out10362.root");
  // tr[0]->Add("~/R15bTME/Si16b/out*.root");
  // tr[1]->Add("~/R15bTME/Si16b/out*.root");

  TH1* hsite          = CreateAndFitSiTEnergyHistogram(tr[0]);
  TH1* hsil1e_tme     = CreateSiL1TMEEnergyHistogram(tr[0]);
  TH1* hsil1e_tme_oot = CreateSiL1TMEOOTEnergyHistogram(tr[0]);
  TH1* hsil1e_nontme  = CreateSiL1NonTMEEnergyHistogram(tr[1]);
  TH1* hsil1t_stops   = CreateSiL1MuonStopTimeDistribution(tr[0]);

  hsil1e_tme    ->SetLineColor(kGreen);
  hsil1e_tme_oot->SetLineColor(kBlue);
  hsil1e_nontme ->SetLineColor(kRed);

  TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
  leg->AddEntry(hsil1e_tme,     "TMETree (|t|<200ns && OneHit(SiT))");
  leg->AddEntry(hsil1e_tme_oot, "TMETree (|t|>200ns && OneHit(SiT))");
  leg->AddEntry(hsil1e_nontme,  "NonTMETree");

  THStack* hsil1es = new THStack("hsil1es",
                                 "Si16b SiL1 Energy;E [keV];cts/10 keV");
  hsil1es->Add(hsil1e_tme);
  hsil1es->Add(hsil1e_tme_oot);
  hsil1es->Add(hsil1e_nontme);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  TCanvas* csit = new TCanvas;
  hsite->Draw();
  csit->SaveAs("img/sit_efficiency_site.png");
  TCanvas* csil1 = new TCanvas("csil1", "csil1", 1400, 500);
  csil1->Divide(2);
  csil1->cd(1);
  hsil1es->Draw("NOSTACK");
  leg->Draw();
  csil1->cd(2);
  hsil1t_stops->Draw();
  csil1->SaveAs("img/sit_efficiency_sil1e.png");

  PrintSiTEnergyHistogramFitInfo(hsite);
  PrintSiL1EnergyHistogramInfo(hsil1e_tme, hsil1e_tme_oot, hsil1e_nontme);
}
