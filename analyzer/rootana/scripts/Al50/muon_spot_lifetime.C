#include "scripts/Al50/util.h"

#include "TBox.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TText.h"

#include <iostream>
#include <vector>

TChain* BuildPIDTMEChain() {
  TChain* ch = new TChain();
  ch->Add("data/Al50/psel.3layer.root/PID_RP");
  ch->Add("data/Al50/psel.3layer.root/PID_RD");
  ch->Add("data/Al50/psel.3layer.root/PID_RT");
  ch->Add("data/Al50/psel.3layer.root/PID_RA");
  ch->Add("data/Al50/psel.3layer.root/PID_RU");
  return ch;
}

TChain* BuildNonTMEChain() {
  TChain* ch = new TChain("TMETree/NonTMETree");
  // ch->Add("~/R15bTME/Al50/tme09890.root");
  ch->Add("~/R15bTME/Al50/tme09891.root");
  // ch->Add("~/R15bTME/Al50/tme*.root");
  SetTMEBranchAddresses(ch);
  CollectChannels();
  return ch;
}

void BuildTMEdEvEHists(TChain* ch, TH2*& hall, TH2*& hearly, TH2*& hlate,
                       TH2*& hprompt) {
  ch->Draw("de:e>>hall(200, 0, 5e3, 108, 300, 3e3)",    "",                "goff");
  ch->Draw("de:e>>hearly(200, 0, 5e3, 108, 300, 3e3)",  "t<400",           "goff");
  ch->Draw("de:e>>hlate(200, 0, 5e3, 108, 300, 3e3)",   "400<t",           "goff");
  ch->Draw("de:e>>hprompt(200, 0, 5e3, 108, 300, 3e3)", "-200<t && t<200", "goff");
  hall    = (TH2*)gDirectory->Get("hall");
  hearly  = (TH2*)gDirectory->Get("hearly");
  hlate   = (TH2*)gDirectory->Get("hlate");
  hprompt = (TH2*)gDirectory->Get("hprompt");
  hall    ->SetTitle("SiR dE vs E (No time cut);E [keV];dE [keV]");
  hprompt ->SetTitle("SiR dE vs E (-200ns<t && t<200ns);E [keV];dE [keV]");
  hearly  ->SetTitle("SiR dE vs E (t<400ns);E [keV];dE [keV]");
  hlate   ->SetTitle("SiR dE vs E (400ns<t);E [keV];dE [keV]");
  HistUtils::MatchMaximums(std::vector<TH2*>{hall, hearly, hlate, hprompt});
}

void BuildNonTMEdEvEHist(TChain* ch, TH2*& hnontme) {
  hnontme = new TH2D("hnontme", "SiR dE vs E (NonTME);E [keV];dE [keV]",
                     200, 0, 5e3, 108, 300, 3e3);
  for (int ev = 0; ev < ch->GetEntries(); ++ev) {
    ch->GetEntry(ev);
    std::cout << SiR2 << std::endl;
    if (ev % 100000 == 0) std::cout << ev << "/" << ch->GetEntries() << std::endl;
    for (int i = 0; i < SiR2->size(); ++i) {
      for (int j = 0; j < all_SiR1_channels.size(); ++j) {
        for (int k = 0; k < all_SiR1_channels[j]->size(); ++k) {
          std::cout << SiR2->size() << " " << j << " " << all_SiR1_channels[j]->size() << std::endl;
          if (std::abs(all_SiR1_channels[j]->at(k).tblock-SiR2->at(i).tblock)<200) {
            hnontme->Fill(SiR2->at(i).E+all_SiR1_channels[j]->at(k).E,
                          all_SiR1_channels[j]->at(k).E);
            break;
          }
        }
      }
    }
  }
}

void BuildTMETimeHists(TChain* ch, TH1*& htimemu1, TH1*& htimemu2) {
  ch->Draw("t>>htimemu1(150,-500,1000)", "600<de && de<1200 && 1200<e && e<2000", "goff");
  ch->Draw("t>>htimemu2(150,-500,1000)", "300<de && de<800  && 900<e  && e<1500", "goff");
  htimemu1 = (TH1*)gDirectory->Get("htimemu1");
  htimemu2 = (TH1*)gDirectory->Get("htimemu2");
  htimemu1->SetTitle("SiR Muon Spot Time;t [ns];Height Normalized");
  htimemu2->SetTitle("SiR Muon Spot Time;t [ns];Height Normalized");
  htimemu1->SetStats(false);
  htimemu2->SetStats(false);
  htimemu1->SetLineColor(1);
  htimemu2->SetLineColor(2);
}

void BuildMuSpotBoxes(TBox*& b1, TBox*& b2) {
  b1 = new TBox(1200, 600, 2000, 1200);
  b2 = new TBox(900, 300,  1500,  800);
  b1->SetLineWidth(3);
  b2->SetLineWidth(3);
  b1->SetFillStyle(0);
  b2->SetFillStyle(0);
  b1->SetLineColor(1);
  b2->SetLineColor(2);
}

void muon_spot_lifetime() {
  TCanvas* c = new TCanvas();
  TChain* tmes = BuildPIDTMEChain();
  TChain* nontmes = BuildNonTMEChain();

  TBox *muspot1, *muspot2;
  BuildMuSpotBoxes(muspot1, muspot2);
  TH2 *hall, *hearly, *hlate, *hprompt, *hnontme;
  BuildTMEdEvEHists(tmes, hall, hearly, hlate, hprompt);
  BuildNonTMEdEvEHist(nontmes, hnontme);
  TH1 *htimemu1, *htimemu2;
  BuildTMETimeHists(tmes, htimemu1, htimemu2);

  TCanvas* cevde = new TCanvas("cevde", "EvdE Plots", 1400, 1000);
  cevde  ->Divide(2,2);
  cevde  ->cd(1);
  hall   ->Draw("COL");
  cevde  ->cd(2);
  hprompt->Draw("COL");
  cevde  ->cd(3);
  hearly ->Draw("COL");
  cevde  ->cd(4);
  hlate  ->Draw("COL");
  cevde  ->SaveAs("img/evde_r_tcuts.png");

  TCanvas* cnontme = new TCanvas("cnontme", "EvdE Plots", 1400, 500);
  cnontme->Divide(2);
  cnontme->cd(1);
  hall->Draw("COL");
  cnontme->cd(2);
  hnontme->Draw("COL");

  TCanvas* ct = new TCanvas("ct", "Muon Spots", 1400, 500);
  ct->Divide(2);
  ct->cd(1);
  hall->Draw("COL");
  muspot1->Draw();
  muspot2->Draw();
  int nmu1_all  = htimemu1->GetEntries();
  int nmu2_all  = htimemu2->GetEntries();
  int nmu1_tcut = htimemu1->Integral(htimemu1->FindFixBin(-200),
                                     htimemu1->FindFixBin(200));
  int nmu2_tcut = htimemu2->Integral(htimemu2->FindFixBin(-200),
                                     htimemu2->FindFixBin(200));
  TText* nmutitle = new TText(200, 0.7, "Total (Prompt)");
  TText* nmu1lab  = new TText(200, 0.5,
                              TString::Format("%d (%d)", nmu1_all, nmu1_tcut));
  TText* nmu2lab  = new TText(200, 0.3,
                              TString::Format("%d (%d)", nmu2_all, nmu2_tcut));
  nmu1lab ->SetTextColor(1);
  nmu2lab ->SetTextColor(2);
  htimemu1->Scale(1./htimemu1->GetMaximum());
  htimemu2->Scale(1./htimemu2->GetMaximum());
  ct      ->cd(2);
  htimemu1->Draw();
  htimemu2->Draw("SAME");
  nmutitle->Draw();
  nmu1lab ->Draw();
  nmu2lab ->Draw();
  ct      ->SaveAs("img/t_muspots.png");
}
