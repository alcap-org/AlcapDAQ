#include "./scripts/Al50/R15b_Al50_psel.C"

#include "TCanvas.h"
#include "TCut.h"
#include "TF1.h"
#include "TFile.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "TTree.h"

#include <cstdio>
#include <string>
using std::string;

class PIDCut {
  TF1* f[2];
  TLine* l[2];
public:
  PIDCut(char lr, char p) {
    static const double elo = 1.9e3, ehi = 17e3;
    f[0] = new TF1("", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]",
                   elo, ehi);
    f[1] = new TF1("", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]",
                   elo, ehi);
    if (lr == 'L' && p == 'p') {
      f[0]->SetParameters(4e3, 35e5, -1.02, -0.85);
      f[1]->SetParameters(4e3, 44e5, -1.00, -0.78);
    } else if (lr == 'R' && p == 'p') {
      f[0]->SetParameters(4e3, 60e5, -1.09, -0.95);
      f[1]->SetParameters(4e3, 11e6, -1.10, -0.85);
    } else if (lr == 'L' && p == 'd') {
      f[0]->SetParameters(4e3, 15e5, -0.85, -0.85);
      f[1]->SetParameters(4e3, 22e5, -0.85, -0.85);
    } else if (lr == 'R' && p == 'd') {
      f[0]->SetParameters(4e3, 22e5, -0.90, -0.90);
      f[1]->SetParameters(4e3, 35e5, -0.90, -0.90);
    } else {
      throw "WRONG PARTICLE OR L/R";
    }
    l[0] = new TLine(elo, f[0]->Eval(elo), elo, f[1]->Eval(elo));
    l[1] = new TLine(ehi, f[0]->Eval(ehi), ehi, f[1]->Eval(ehi));
  }
  void SetLineColor(Color_t c) {
    f[0]->SetLineColor(c);
    f[1]->SetLineColor(c);
    l[0]->SetLineColor(c);
    l[1]->SetLineColor(c);
  }
  void SetLineStyle(Style_t s) {
    f[0]->SetLineStyle(s);
    f[1]->SetLineStyle(s);
    l[0]->SetLineStyle(s);
    l[1]->SetLineStyle(s);
  }
  void Draw() {
    f[0]->Draw("SAME");
    f[1]->Draw("SAME");
    l[0]->Draw("SAME");
    l[1]->Draw("SAME");
  }
};

TGraph* PIDTreeToGraph(TTree* tr, bool standard_cuts=true) {
  double e1, e2, e3;
  double t1, t2, t3;
  // tr->SetBranchStatus("*",  0);
  // tr->SetBranchStatus("e1", 1);
  // tr->SetBranchStatus("e2", 1);
  tr->SetBranchAddress("e1", &e1);
  tr->SetBranchAddress("e2", &e2);
  tr->SetBranchAddress("e3", &e3);
  tr->SetBranchAddress("t1", &t1);
  tr->SetBranchAddress("t2", &t2);
  tr->SetBranchAddress("t3", &t3);
  TGraph* gr = new TGraph(tr->GetEntries());
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (!standard_cuts || (e3==0 && t2>400 && std::abs(t2-t1)<500))
      gr->SetPoint(i, e1+e2, e1);
  }
  tr->ResetBranchAddresses();
  return gr;
}

void r15b_al50_psel_draw(const char* ifname) {
  TFile* ifile = new TFile(ifname);
  TTree* trp[2] = { (TTree*)ifile->Get("PID_LP"), (TTree*)ifile->Get("PID_RP") };
  TTree* trd[2] = { (TTree*)ifile->Get("PID_LD"), (TTree*)ifile->Get("PID_RD") };
  TTree* trt[2] = { (TTree*)ifile->Get("PID_LT"), (TTree*)ifile->Get("PID_RT") };
  TTree* tra[2] = { (TTree*)ifile->Get("PID_LA"), (TTree*)ifile->Get("PID_RA") };
  TTree* tru[2] = { (TTree*)ifile->Get("PID_LU"), (TTree*)ifile->Get("PID_RU") };

  PSelConfig_t conf((TTree*)ifile->Get("config"));
  TimeOffset_t terr(conf.dataset);

  TGraph* grp[2] = { PIDTreeToGraph(trp[0]), PIDTreeToGraph(trp[1]) };
  TGraph* grd[2] = { PIDTreeToGraph(trd[0]), PIDTreeToGraph(trd[1]) };
  TGraph* grt[2] = { PIDTreeToGraph(trt[0]), PIDTreeToGraph(trt[1]) };
  TGraph* gra[2] = { PIDTreeToGraph(tra[0]), PIDTreeToGraph(tra[1]) };
  TGraph* gru[2] = { PIDTreeToGraph(tru[0]), PIDTreeToGraph(tru[1]) };

  TH1* hp[2] = { new TH1D("hpl", "Measured Proton Energy;E [keV];dE [/100keV]",   150, 2e3, 17e3),
                 new TH1D("hpr", "Measured Proton Energy;E [keV];dE [/100keV]",   150, 2e3, 17e3) };
  TH1* hd[2] = { new TH1D("hdl", "Raw Deuteron Spectrum (dt<500 && t>400 && e3==0);E [keV];dE [/100keV]", 150, 2e3, 17e3),
                 new TH1D("hdr", "Raw Deuteron Spectrum (dt<500 && t>400 && e3==0);E [keV];dE [/100keV]", 150, 2e3, 17e3) };
  TH1* ht[2] = { new TH1D("htl", "Raw Triton Spectrum (dt<500 && t>400 && e3==0);E [keV];dE [/100keV]",   150, 2e3, 17e3),
                 new TH1D("htr", "Raw Triton Spectrum (dt<500 && t>400 && e3==0);E [keV];dE [/100keV]",   150, 2e3, 17e3) };
  TH1* ha[2] = { new TH1D("hal", "Raw Alpha Spectrum (dt<500 && t>400 && e3==0);E [keV];dE [/100keV]",    150, 2e3, 17e3),
                 new TH1D("har", "Raw Alpha Spectrum (dt<500 && t>400 && e3==0);E [keV];dE [/100keV]",    150, 2e3, 17e3) };
  const char* tcut_fmt = "(t2+%g)>400 && abs(t2-t1+%g)<500 && e3==0";
  char tcut[2][128];
  std::sprintf(tcut[0], tcut_fmt, terr.t2[0], terr.t2[0]-terr.t1[0]);
  std::sprintf(tcut[1], tcut_fmt, terr.t2[1], terr.t2[1]-terr.t1[1]);
  trp[0]->Draw("e>>hpl", tcut[0], "goff");
  trp[1]->Draw("e>>hpr", tcut[1], "goff");
  trd[0]->Draw("e>>hdl", tcut[0], "goff");
  trd[1]->Draw("e>>hdr", tcut[1], "goff");
  trt[0]->Draw("e>>htl", tcut[0], "goff");
  trt[1]->Draw("e>>htr", tcut[1], "goff");
  tra[0]->Draw("e>>hal", tcut[0], "goff");
  tra[1]->Draw("e>>har", tcut[1], "goff");

  string hptitle[2] = { "SiL Proton PID Cut (",   "SiR Proton PID Cut (" };
  string hdtitle[2] = { "SiL Deuteron PID Cut (", "SiR Deuteron PID Cut (" };
  (hptitle[0] += conf.dataset) += " Data);E [keV];dE [keV]";
  (hptitle[1] += conf.dataset) += " Data);E [keV];dE [keV]";
  (hdtitle[0] += conf.dataset) += " Data);E [keV];dE [keV]";
  (hdtitle[1] += conf.dataset) += " Data);E [keV];dE [keV]";

  grp[0]->SetTitle(hptitle[0].c_str());
  grp[1]->SetTitle(hptitle[1].c_str());
  grp[0]->GetYaxis()->SetRangeUser(0, 3e3);
  grp[1]->GetYaxis()->SetRangeUser(0, 3e3);
  grd[0]->SetTitle(hdtitle[0].c_str());
  grd[1]->SetTitle(hdtitle[1].c_str());
  grd[0]->GetYaxis()->SetRangeUser(0, 3e3);
  grd[1]->GetYaxis()->SetRangeUser(0, 3e3);

  PIDCut pids[2][2] = { { PIDCut('L', 'p'), PIDCut('R', 'p') },
                        { PIDCut('L', 'd'), PIDCut('R', 'd') } };
  pids[0][0].SetLineColor(kRed);
  pids[0][1].SetLineColor(kRed);
  pids[1][0].SetLineColor(kRed);
  pids[1][1].SetLineColor(kRed);

  hp[0]->SetLineColor(ACStyle::kACBlue);
  hp[1]->SetLineColor(ACStyle::kACRed);
  hd[0]->SetLineColor(ACStyle::kACBlue);
  hd[1]->SetLineColor(ACStyle::kACRed);

  gStyle->SetOptStat(0);

  TCanvas* cpidprot = new TCanvas("cpidprot", "cpidprot", 1400, 500);
  cpidprot->Divide(2);
  cpidprot->cd(1);
  grp [0]->Draw("AP");
  grd [0]->Draw("P SAME");
  grt [0]->Draw("P SAME");
  gra [0]->Draw("P SAME");
  gru [0]->Draw("P SAME");
  pids[0][0]. Draw();
  cpidprot->cd(2);
  grp [1]->Draw("AP");
  grd [1]->Draw("P SAME");
  grt [1]->Draw("P SAME");
  gra [1]->Draw("P SAME");
  gru [1]->Draw("P SAME");
  pids[0][1]. Draw();
  cpidprot->SaveAs("img/pidcut_prot.png");

  TCanvas* cpiddeut = new TCanvas("cpiddeut", "cpiddeut", 1400, 500);
  cpiddeut->Divide(2);
  cpiddeut->cd(1);
  grd [0]->Draw("AP");
  grp [0]->Draw("P SAME");
  grt [0]->Draw("P SAME");
  gra [0]->Draw("P SAME");
  gru [0]->Draw("P SAME");
  pids[1][0]. Draw();
  cpiddeut->cd(2);
  grd [1]->Draw("AP");
  grp [1]->Draw("P SAME");
  grt [1]->Draw("P SAME");
  gra [1]->Draw("P SAME");
  gru [1]->Draw("P SAME");
  pids[1][1]. Draw();
  cpiddeut->SaveAs("img/pidcut_deut.png");

  TLegend* l[4]= {
    new TLegend(0.7, 0.7, 0.9, 0.9), new TLegend(0.7, 0.7, 0.9, 0.9),
    new TLegend(0.7, 0.7, 0.9, 0.9), new TLegend(0.7, 0.7, 0.9, 0.9)
  };
  l[0]->AddEntry(hp[0], "SiL");
  l[0]->AddEntry(hp[1], "SiR");
  l[1]->AddEntry(hd[0], "SiL");
  l[1]->AddEntry(hd[1], "SiR");
  l[2]->AddEntry(ht[0], "SiL");
  l[2]->AddEntry(ht[1], "SiR");
  l[3]->AddEntry(ha[0], "SiL");
  l[3]->AddEntry(ha[1], "SiR");

  // TCanvas* ctmp = new TCanvas("ctmp", "ctmp", 1400, 500);
  // ctmp->Divide(2);
  // ctmp->cd(1);
  // grp [1]->Draw("AP");
  // grd [1]->Draw("P SAME");
  // grt [1]->Draw("P SAME");
  // gra [1]->Draw("P SAME");
  // gru [1]->Draw("P SAME");
  // pids[1]. Draw();
  // ctmp->cd(2);
  // hp[1]->Draw("E SAME");
  // hp[0]->Draw("E SAME");
  // l->Draw();
  // ctmp->SaveAs("img/dpf2019pid.png");

  TCanvas* cp = new TCanvas("cp", "cp", 700, 500);
  hp[1]->Draw("E");
  hp[0]->Draw("E SAME");
  l[0]->Draw();
  cp->SaveAs("img/raw_prot.png");

  TCanvas* cd = new TCanvas("cd", "cd", 700, 500);
  hd[1]->Draw("E");
  hd[0]->Draw("E SAME");
  l[1]->Draw();
  cd->SaveAs("img/raw_deut.png");


  TCut ecut[3] = { "2e3<e && e<17e3", "3.5e3<e && e<10e3", "4e3<e && e<8e3"};
  int ntot [2] = { (int)trp[0]->GetEntries(tcut[0] && ecut[0]),
                   (int)trp[1]->GetEntries(tcut[1] && ecut[0]) };
  int nroi1[2] = { (int)trp[0]->GetEntries(tcut[0] && ecut[1]),
                   (int)trp[1]->GetEntries(tcut[1] && ecut[1]) };
  int nroi2[2] = { (int)trp[0]->GetEntries(tcut[0] && ecut[2]),
                   (int)trp[1]->GetEntries(tcut[1] && ecut[2]) };
  std::printf("\tTotal\t3.5-10MeV\t4-8MeV\n");
  std::printf("SiL\t%d\t%d\t\t%d\n", ntot[0], nroi1[0], nroi2[0]);
  std::printf("SiR\t%d\t%d\t\t%d\n", ntot[1], nroi1[1], nroi2[1]);

  // chl->SaveAs("mc_evde_l_raw.png");
  // chr->SaveAs("mc_evde_r_raw.png");
  // cpl->SaveAs("mc_evde_l_cut.png");
  // cpr->SaveAs("mc_evde_r_cut.png");
  // csl->SaveAs("data_evde_l_classified.png");
  // csr->SaveAs("data_evde_r_classified.png");
  // cep->SaveAs("data_e_proton.png");
  // ced->SaveAs("data_e_deuteron.png");
  // cet->SaveAs("data_e_triton.png");
  // cea->SaveAs("data_e_alpha.png");
}
