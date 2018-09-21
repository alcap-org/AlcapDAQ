#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>

// Compare my TMs and Marks, and unfold Al50 data with both.

void get_raw_hists(TH1*& hl, TH1*& hr) {
  TFile* f = new TFile("data/Al100/pselal100.root");
  TTree* tr[2] = { (TTree*)f->Get("PID_LP"), (TTree*)f->Get("PID_RP") };
  tr[0]->Draw("e>>hlraw(48,0,24e3)",
              "t > 400 && abs(dt) < 200 && e3 == 0 && e < 14e3");
  tr[1]->Draw("e>>hrraw(48,0,24e3)",
              "t > 400 && abs(dt) < 200 && e3 == 0 && e < 14e3");
  TH1* h[2] = { (TH1*)gDirectory->Get("hlraw"),
                (TH1*)gDirectory->Get("hrraw") };
  hl = h[0];
  hr = h[1];
}

void unfold_al50(TH1* hlraw, TH1* hrraw, TH2*& hlresp, TH2*& hrresp,
                 TH1*& hle, TH1*& hre) {
  TFile* ftm = new TFile("data/Al50/tmal50p_frommudist.root");
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)ftm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)ftm->Get("SiR_TM") };
  hlresp = resp[0]->Hresponse();
  hrresp = resp[1]->Hresponse();
  TH1* htrue[2] = { resp[0]->Htruth(), resp[1]->Htruth() };
  hlresp  ->Rebin2D(5, 5);
  hrresp  ->Rebin2D(5, 5);
  htrue[0]->Rebin(5);
  htrue[1]->Rebin(5);
  RooUnfoldBayes* unf[2] = { new RooUnfoldBayes(resp[1], hlraw, 6),
                             new RooUnfoldBayes(resp[0], hrraw, 6) };
  hle = unf[0]->Hreco();
  hre = unf[1]->Hreco();
}

void unfold_al100(TH1* hlraw, TH1* hrraw, TH2*& hlresp, TH2*& hrresp,
                  TH1*& hle, TH1*& hre) {
  TFile* ftm = new TFile("data/Al100/tm.root");
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)ftm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)ftm->Get("SiR_TM") };
  hlresp = resp[0]->Hresponse();
  hrresp = resp[1]->Hresponse();
  TH1* htrue[2] = { resp[0]->Htruth(), resp[1]->Htruth() };
  hlresp  ->Rebin2D(5, 5);
  hrresp  ->Rebin2D(5, 5);
  htrue[0]->Rebin(5);
  htrue[1]->Rebin(5);
  RooUnfoldBayes* unf[2] = { new RooUnfoldBayes(resp[1], hlraw, 6),
                             new RooUnfoldBayes(resp[0], hrraw, 6) };
  hle = unf[0]->Hreco();
  hre = unf[1]->Hreco();
}

void compare_al50_al100_tms() {

  TH1 *hraw[2] = {}, *he_al100[2] = {}, *he_al50[2] = {};
  TH2 *hresp_al100[2], *hresp_al50[2];
  get_raw_hists(hraw[0], hraw[1]);
  unfold_al100(hraw[0], hraw[1], hresp_al100[0], hresp_al100[1],
               he_al100[0], he_al100[1]);
  unfold_al50 (hraw[0], hraw[1], hresp_al50[0],  hresp_al50[1],
               he_al50[0],  he_al50[1]);

  double caprate = 0.609;
  TF1* fnctime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  fnctime->SetParameters(52.7, 864.); // (Si time resolution, muAl lifetime)
  double tcuteff = fnctime->Integral(400, 1e6)/fnctime->Integral(0, 1e6); // ~65%
  double nmu_hi = 105e6; // Al100

  he_al50 [0]->Scale(1./(nmu_hi*caprate*tcuteff));
  he_al50 [1]->Scale(1./(nmu_hi*caprate*tcuteff));
  he_al100[0]->Scale(1./(nmu_hi*caprate*tcuteff));
  he_al100[1]->Scale(1./(nmu_hi*caprate*tcuteff));

  double rate_al100[3][2] = {
    {
      he_al100[0]->Integral(he_al100[0]->FindFixBin(0), he_al100[0]->FindFixBin(10e3)),
      he_al100[1]->Integral(he_al100[1]->FindFixBin(0), he_al100[1]->FindFixBin(10e3))
    },
    {
      he_al100[0]->Integral(he_al100[0]->FindFixBin(3e3), he_al100[0]->FindFixBin(10e3)),
      he_al100[1]->Integral(he_al100[1]->FindFixBin(3e3), he_al100[1]->FindFixBin(10e3))
    },
    {
      he_al100[0]->Integral(he_al100[0]->FindFixBin(4e3), he_al100[0]->FindFixBin(10e3)),
      he_al100[1]->Integral(he_al100[1]->FindFixBin(4e3), he_al100[1]->FindFixBin(10e3))
    }
   };
  double rate_al50[3][2] = {
    {
      he_al50[0]->Integral(he_al50[0]->FindFixBin(0), he_al50[0]->FindFixBin(10e3)),
      he_al50[1]->Integral(he_al50[1]->FindFixBin(0), he_al50[1]->FindFixBin(10e3))
    },
    {
      he_al50[0]->Integral(he_al50[0]->FindFixBin(3e3), he_al50[0]->FindFixBin(10e3)),
      he_al50[1]->Integral(he_al50[1]->FindFixBin(3e3), he_al50[1]->FindFixBin(10e3))
    },
    {
      he_al50[0]->Integral(he_al50[0]->FindFixBin(4e3), he_al50[0]->FindFixBin(10e3)),
      he_al50[1]->Integral(he_al50[1]->FindFixBin(4e3), he_al50[1]->FindFixBin(10e3))
    }
   };

  he_al50    [0]->SetLineColor(kRed);
  he_al50    [1]->SetLineColor(kRed);
  hresp_al100[0]->GetXaxis()->SetRangeUser(0, 14e3);
  hresp_al100[0]->GetYaxis()->SetRangeUser(0, 14e3);
  hresp_al100[1]->GetXaxis()->SetRangeUser(0, 14e3);
  hresp_al100[1]->GetYaxis()->SetRangeUser(0, 14e3);
  he_al100   [0]->GetXaxis()->SetRangeUser(0, 14e3);
  he_al100   [1]->GetXaxis()->SetRangeUser(0, 14e3);
  he_al50    [0]->GetXaxis()->SetRangeUser(0, 14e3);
  he_al50    [1]->GetXaxis()->SetRangeUser(0, 14e3);
  he_al50    [0]->SetTitle("Left Unfolded Energy;E [keV];Proton/Capture/500 keV");
  he_al50    [1]->SetTitle("Right Unfolded Energy;E [keV];Proton/Capture/500 keV");
  hresp_al100[0]->SetTitle("Al100 Left TM (Mark);E_{meas} [MeV];E_{true} [MeV]");
  hresp_al100[1]->SetTitle("Al100 Right TM (Mark);E_{meas} [MeV];E_{true} [MeV]");
  hresp_al50 [0]->SetTitle("Al50 Left TM (John);E_{meas} [MeV];E_{true} [MeV]");
  hresp_al50 [1]->SetTitle("Al50 Right TM (John);E_{meas} [MeV];E_{true} [MeV]");
  he_al100   [0]->SetStats(false);
  he_al100   [1]->SetStats(false);
  he_al50    [0]->SetStats(false);
  he_al50    [1]->SetStats(false);
  he_al50    [0]->GetYaxis()->SetTitleOffset(1.4);
  he_al50    [1]->GetYaxis()->SetTitleOffset(1.4);

  char str100[64], str50[64];
  TLegend* leg[2] = { new TLegend(0.6, 0.6, 0.9, 0.9),
                      new TLegend(0.6, 0.6, 0.9, 0.9) };
  leg[0]->SetNColumns(3);
  leg[1]->SetNColumns(3);
  leg[0]->AddEntry((TObject*)nullptr, "",                          "");
  leg[0]->AddEntry((TObject*)nullptr, "Al100",                     "");
  leg[0]->AddEntry((TObject*)nullptr, "Al50",                      "");
  leg[1]->AddEntry((TObject*)nullptr, "",                          "");
  leg[1]->AddEntry((TObject*)nullptr, "Al100",                     "");
  leg[1]->AddEntry((TObject*)nullptr, "Al50",                      "");
  leg[0]->AddEntry((TObject*)nullptr, "Total",                     "");
  leg[1]->AddEntry((TObject*)nullptr, "Total",                     "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al100[0][0]), "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al50 [0][0]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al100[0][1]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al50 [0][1]), "");
  leg[0]->AddEntry((TObject*)nullptr, "3-10 MeV",                  "");
  leg[1]->AddEntry((TObject*)nullptr, "3-10 MeV",                  "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al100[1][0]), "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al50 [1][0]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al100[1][1]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al50 [1][1]), "");
  leg[0]->AddEntry((TObject*)nullptr, "4-10 MeV",                  "");
  leg[1]->AddEntry((TObject*)nullptr, "4-10 MeV",                  "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al100[2][0]), "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al50 [2][0]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al100[2][1]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_al50 [2][1]), "");

  TCanvas* cresp = new TCanvas("cresp", "", 1400, 1000);
  cresp->Divide(2, 2);
  cresp->cd(1);
  hresp_al100[0]->Draw("COL");
  cresp->cd(2);
  hresp_al100[1]->Draw("COL");
  cresp->cd(3);
  hresp_al50[0]->Draw("COL");
  cresp->cd(4);
  hresp_al50[1]->Draw("COL");
  cresp->SaveAs("img/comp_al50_al100_tms.png");

  TCanvas* creco = new TCanvas("creco", "", 1400, 500);
  creco->Divide(2);
  creco->cd(1);
  he_al50 [0]->Draw();
  he_al100[0]->Draw("SAME");
  leg[0]->Draw();
  creco->cd(2);
  he_al50 [1]->Draw();
  he_al100[1]->Draw("SAME");
  leg[1]->Draw();
  creco->SaveAs("img/comp_al50_al100_e.png");
}
