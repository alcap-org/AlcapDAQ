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

// Compare my TMs and Marks.

void get_raw_hists(TH1*& hl, TH1*& hr) {
  TFile* f = new TFile("data/Al100/pselal100.root");
  TTree* tr[2] = { (TTree*)f->Get("PID_LP"), (TTree*)f->Get("PID_RP") };
  tr[0]->Draw("e/1e3>>hlraw(48,0,24)",
              "t > 400 && abs(dt) < 200 && e3 == 0 && e/1e3 < 14");
  tr[1]->Draw("e/1e3>>hrraw(48,0,24)",
              "t > 400 && abs(dt) < 200 && e3 == 0 && e/1e3 < 14");
  TH1* h[2] = { (TH1*)gDirectory->Get("hlraw"),
                (TH1*)gDirectory->Get("hrraw") };
  hl = h[0];
  hr = h[1];
}


void unfold_mw(TH1*  hlraw,  TH1* hrraw, TH2*& hlresp, TH2*& hrresp,
               TH1*& hle,    TH1*& hre,  TH1*& hltrue, TH1*& hrtrue,
               TH1*& hlmeas, TH1*& hrmeas) {
  TFile* ftm = new TFile("data/Al100/unfolding.proton.0-14.200k.target.8000.leftright.y-0.x+0.z+0.005.root");
  hlresp = (TH2*)ftm->Get("SiL/hTransferLeft");
  hrresp = (TH2*)ftm->Get("SiR/hTransferRight");
  hltrue = (TH1*)ftm->Get("SiL/hInputLeft");
  hrtrue = (TH1*)ftm->Get("SiR/hInputRight");
  hlmeas = (TH1*)ftm->Get("SiL/hMeasLeft");
  hrmeas = (TH1*)ftm->Get("SiR/hMeasRight");
  RooUnfoldResponse* resp[2] = {
    new RooUnfoldResponse(nullptr, hltrue, hlresp),
    new RooUnfoldResponse(nullptr, hrtrue, hrresp)
  };
  RooUnfoldBayes* unf[2] = {
    new RooUnfoldBayes(resp[0], hlraw, 6),
    new RooUnfoldBayes(resp[1], hrraw, 6)
  };
  hle = unf[0]->Hreco();
  hre = unf[1]->Hreco();
  // SCALE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Solid angle efficiency based on runlog 385
  // Left distance: 133.2 mm (14/16 strips active)
  // Right distance: 120.8 mm
  // 50mm by 50mm detector
  const double SAeff[2] = { 0.009811372127292606, 0.013633151977336205 };
  hle->Scale(1./SAeff[0]);
  hre->Scale(1./SAeff[1]);
}


void unfold_jq(TH1*  hlraw,  TH1* hrraw, TH2*& hlresp, TH2*& hrresp,
               TH1*& hle,    TH1*& hre,  TH1*& hltrue, TH1*& hrtrue,
               TH1*& hlmeas, TH1*& hrmeas) {
  TFile* ftm = new TFile("data/Al100/tm.root");
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)ftm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)ftm->Get("SiR_TM") };
  hlresp = resp[0]->Hresponse();
  hrresp = resp[1]->Hresponse();
  hltrue = resp[0]->Htruth();    hrtrue = resp[1]->Htruth();
  hlmeas = resp[0]->Hmeasured(); hrmeas = resp[1]->Hmeasured();
  //hlfake = resp[0]->Hfakes();    hrfake = resp[1]->Hfakes();
  hlresp->GetXaxis()->Set(hlresp->GetNbinsX(),
                          hlresp->GetXaxis()->GetXmin()/1e3,
                          hlresp->GetXaxis()->GetXmax()/1e3);
  hlresp->GetYaxis()->Set(hlresp->GetNbinsY(),
                          hlresp->GetYaxis()->GetXmin()/1e3,
                          hlresp->GetYaxis()->GetXmax()/1e3);
  hrresp->GetXaxis()->Set(hrresp->GetNbinsX(),
                          hrresp->GetXaxis()->GetXmin()/1e3,
                          hrresp->GetXaxis()->GetXmax()/1e3);
  hrresp->GetYaxis()->Set(hrresp->GetNbinsY(),
                          hrresp->GetYaxis()->GetXmin()/1e3,
                          hrresp->GetYaxis()->GetXmax()/1e3);
  hltrue->GetXaxis()->Set(hltrue->GetNbinsX(),
                          hltrue->GetXaxis()->GetXmin()/1e3,
                          hltrue->GetXaxis()->GetXmax()/1e3);
  hrtrue->GetXaxis()->Set(hrtrue->GetNbinsX(),
                          hrtrue->GetXaxis()->GetXmin()/1e3,
                          hrtrue->GetXaxis()->GetXmax()/1e3);
  hlmeas->GetXaxis()->Set(hlmeas->GetNbinsX(),
                          hlmeas->GetXaxis()->GetXmin()/1e3,
                          hlmeas->GetXaxis()->GetXmax()/1e3);
  hrmeas->GetXaxis()->Set(hrmeas->GetNbinsX(),
                          hrmeas->GetXaxis()->GetXmin()/1e3,
                          hrmeas->GetXaxis()->GetXmax()/1e3);
  hlresp->Rebin2D(5, 5);
  hrresp->Rebin2D(5, 5);
  hltrue->Rebin(5);
  hrtrue->Rebin(5);
  hlmeas->Rebin(5);
  hrmeas->Rebin(5);
  RooUnfoldBayes* unf[2] = { new RooUnfoldBayes(resp[0], hlraw, 6),
                             new RooUnfoldBayes(resp[1], hrraw, 6) };
  hle = unf[0]->Hreco();
  hre = unf[1]->Hreco();
}


void compare_al100_tms_jqmw() {
  TH1 *hraw[2] = {}, *he_mw[2] = {}, *he_jq[2] = {},
      *htrue_mw[2] = {}, *htrue_jq[2] = {},
      *hmeas_mw[2] = {}, *hmeas_jq[2] = {};
  TH2 *hresp_mw[2], *hresp_jq[2];
  get_raw_hists(hraw[0], hraw[1]);
  unfold_mw(hraw[0], hraw[1], hresp_mw[0], hresp_mw[1], he_mw[0], he_mw[1],
            htrue_mw[0], htrue_mw[1], hmeas_mw[0], hmeas_mw[1]);
  unfold_jq(hraw[0], hraw[1], hresp_jq[0], hresp_jq[1], he_jq[0], he_jq[1],
            htrue_jq[0], htrue_jq[1], hmeas_jq[0], hmeas_jq[1]);

  double caprate = 0.609;
  TF1* fnctime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  fnctime->SetParameters(52.7, 864.); // (Si time resolution, muAl lifetime)
  double tcuteff = fnctime->Integral(400, 1e6)/fnctime->Integral(0, 1e6); // ~65%
  double nmu_hi = 105e6; // Al100

  he_jq[0]->Scale(1./(nmu_hi*caprate*tcuteff));
  he_jq[1]->Scale(1./(nmu_hi*caprate*tcuteff));
  he_mw[0]->Scale(1./(nmu_hi*caprate*tcuteff));
  he_mw[1]->Scale(1./(nmu_hi*caprate*tcuteff));

  double rate_mw[3][2] = {
    {
      he_mw[0]->Integral(he_mw[0]->FindFixBin(0), he_mw[0]->FindFixBin(10)),
      he_mw[1]->Integral(he_mw[1]->FindFixBin(0), he_mw[1]->FindFixBin(10))
    },
    {
      he_mw[0]->Integral(he_mw[0]->FindFixBin(3), he_mw[0]->FindFixBin(10)),
      he_mw[1]->Integral(he_mw[1]->FindFixBin(3), he_mw[1]->FindFixBin(10))
    },
    {
      he_mw[0]->Integral(he_mw[0]->FindFixBin(4), he_mw[0]->FindFixBin(10)),
      he_mw[1]->Integral(he_mw[1]->FindFixBin(4), he_mw[1]->FindFixBin(10))
    }
  };
  double rate_jq[3][2] = {
    {
      he_jq[0]->Integral(he_jq[0]->FindFixBin(0), he_jq[0]->FindFixBin(10)),
      he_jq[1]->Integral(he_jq[1]->FindFixBin(0), he_jq[1]->FindFixBin(10))
    },
    {
      he_jq[0]->Integral(he_jq[0]->FindFixBin(3), he_jq[0]->FindFixBin(10)),
      he_jq[1]->Integral(he_jq[1]->FindFixBin(3), he_jq[1]->FindFixBin(10))
    },
    {
      he_jq[0]->Integral(he_jq[0]->FindFixBin(4), he_jq[0]->FindFixBin(10)),
      he_jq[1]->Integral(he_jq[1]->FindFixBin(4), he_jq[1]->FindFixBin(10))
    }
  };

  he_mw   [0]->SetLineColor(kBlue);
  he_mw   [1]->SetLineColor(kBlue);
  he_jq   [0]->SetLineColor(kRed);
  he_jq   [1]->SetLineColor(kRed);
  hmeas_mw[0]->SetLineColor(kBlue);
  hmeas_mw[1]->SetLineColor(kBlue);
  hmeas_jq[0]->SetLineColor(kRed);
  hmeas_jq[1]->SetLineColor(kRed);
  hresp_mw[0]->GetXaxis()->SetRangeUser(0, 14);
  hresp_mw[0]->GetYaxis()->SetRangeUser(0, 14);
  hresp_mw[1]->GetXaxis()->SetRangeUser(0, 14);
  hresp_mw[1]->GetYaxis()->SetRangeUser(0, 14);
  he_mw   [0]->GetXaxis()->SetRangeUser(0, 14);
  he_mw   [1]->GetXaxis()->SetRangeUser(0, 14);
  he_jq   [0]->GetXaxis()->SetRangeUser(0, 14);
  he_jq   [1]->GetXaxis()->SetRangeUser(0, 14);
  hmeas_mw[0]->GetXaxis()->SetRangeUser(0, 16);
  hmeas_mw[1]->GetXaxis()->SetRangeUser(0, 16);
  hmeas_jq[0]->GetXaxis()->SetRangeUser(0, 16);
  hmeas_jq[1]->GetXaxis()->SetRangeUser(0, 16);
  hresp_mw[0]->SetMaximum(hresp_jq[0]->GetMaximum());
  hresp_mw[1]->SetMaximum(hresp_jq[1]->GetMaximum());
  he_jq   [0]->SetTitle("Al100 Left Unfolded Energy;E [keV];Proton/Capture/500 keV");
  he_jq   [1]->SetTitle("Al100 Right Unfolded Energy;E [keV];Proton/Capture/500 keV");
  hresp_mw[0]->SetTitle("Al100 Left TM (Mark);E_{meas} [MeV];E_{true} [MeV]");
  hresp_mw[1]->SetTitle("Al100 Right TM (Mark);E_{meas} [MeV];E_{true} [MeV]");
  hresp_jq[0]->SetTitle("Al100 Left TM (John);E_{meas} [MeV];E_{true} [MeV]");
  hresp_jq[1]->SetTitle("Al100 Right TM (John);E_{meas} [MeV];E_{true} [MeV]");
  hmeas_mw[0]->SetTitle("Al100 Left Measured Energy (MC);E [keV];Normalized");
  hmeas_mw[1]->SetTitle("Al100 Right Measured Energy (MC);E [keV];Normalized");
  he_mw   [0]->SetStats(false);
  he_mw   [1]->SetStats(false);
  he_jq   [0]->SetStats(false);
  he_jq   [1]->SetStats(false);
  hmeas_mw[0]->SetStats(false);
  hmeas_mw[1]->SetStats(false);
  hmeas_jq[0]->SetStats(false);
  hmeas_jq[1]->SetStats(false);
  he_jq   [0]->GetYaxis()->SetTitleOffset(1.4);
  he_jq   [1]->GetYaxis()->SetTitleOffset(1.4);

  char str100[64], str50[64];
  TLegend* leg[2] = { new TLegend(0.6, 0.6, 0.9, 0.9),
                      new TLegend(0.6, 0.6, 0.9, 0.9) };
  leg[0]->SetNColumns(3);
  leg[1]->SetNColumns(3);
  leg[0]->AddEntry((TObject*)nullptr, "",                       "");
  leg[0]->AddEntry((TObject*)nullptr, "Mark",                   "");
  leg[0]->AddEntry((TObject*)nullptr, "John",                   "");
  leg[1]->AddEntry((TObject*)nullptr, "",                       "");
  leg[1]->AddEntry((TObject*)nullptr, "Mark",                   "");
  leg[1]->AddEntry((TObject*)nullptr, "John",                   "");
  leg[0]->AddEntry((TObject*)nullptr, "Total",                  "");
  leg[1]->AddEntry((TObject*)nullptr, "Total",                  "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_mw[0][0]), "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_jq[0][0]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_mw[0][1]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_jq[0][1]), "");
  leg[0]->AddEntry((TObject*)nullptr, "3-10 MeV",               "");
  leg[1]->AddEntry((TObject*)nullptr, "3-10 MeV",               "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_mw[1][0]), "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_jq[1][0]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_mw[1][1]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_jq[1][1]), "");
  leg[0]->AddEntry((TObject*)nullptr, "4-10 MeV",               "");
  leg[1]->AddEntry((TObject*)nullptr, "4-10 MeV",               "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_mw[2][0]), "");
  leg[0]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_jq[2][0]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_mw[2][1]), "");
  leg[1]->AddEntry((TObject*)nullptr, TString::Format("%0.02g", rate_jq[2][1]), "");

  TCanvas* cresp = new TCanvas("cresp", "", 1400, 1000);
  cresp->Divide(2, 2);
  cresp->cd(1);
  hresp_mw[0]->DrawNormalized("COL");
  cresp->cd(2);
  hresp_mw[1]->DrawNormalized("COL");
  cresp->cd(3);
  hresp_jq[0]->DrawNormalized("COL");
  cresp->cd(4);
  hresp_jq[1]->DrawNormalized("COL");
  cresp->SaveAs("img/comp_mwjq_tms.png");

  TCanvas* creco = new TCanvas("creco", "", 1400, 500);
  creco->Divide(2);
  creco->cd(1);
  he_jq [0]->Draw();
  he_mw[0]->Draw("SAME");
  leg[0]->Draw();
  creco->cd(2);
  he_jq [1]->Draw();
  he_mw[1]->Draw("SAME");
  leg[1]->Draw();
  creco->SaveAs("img/comp_mwjq_e.png");

  TCanvas* ceff = new TCanvas("ceff", "", 1400, 500);
  ceff->Divide(2);
  ceff->cd(1);
  hmeas_mw[0]->DrawNormalized();
  hmeas_jq[0]->DrawNormalized("SAME");
  ceff->cd(2);
  hmeas_mw[1]->DrawNormalized();
  hmeas_jq[1]->DrawNormalized("SAME");
  ceff->SaveAs("img/comp_mwjq_emeas.png");
}
