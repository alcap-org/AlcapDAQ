#include "scripts/Al50/util.h"

#include "TCanvas.h"
#include "TCut.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"

#include <vector>

using std::vector;


vector<double> get_al100_stopping_rates_different_p_sims(vector<double>& er) {
  er.clear();
  vector<TFile*> ifs {
    new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.2.root"),
    new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.3.root"),
    new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.4.root"),
    new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.5.root"),
    new TFile("data/mc/momscan/R15bAl100MuPreSiT.p29.9.root")
  };
  vector<TTree*> trs;
  vector<double> r;
  for (auto f : ifs) {
    trs.push_back((TTree*)f->Get("tree"));
    trs.back()->SetBranchStatus("*",              false);
    trs.back()->SetBranchStatus("M_particleName", true);
    trs.back()->SetBranchStatus("M_stopped",      true);
    trs.back()->SetBranchStatus("M_volName",      true);
    trs.back()->SetBranchStatus("M_edep",         true);
    TCut ismustop = "M_particleName==\"mu-\" && M_stopped==1 && M_volName==\"Target\"";
    TCut istrig   = "M_particleName==\"mu-\" && M_stopped==0 && M_volName==\"SiT\" && M_edep>200e-6";
    r .push_back(double(trs.back()->GetEntries(ismustop))/trs.back()->GetEntries(istrig));
    er.push_back(std::sqrt(double(trs.back()->GetEntries(ismustop)))/trs.back()->GetEntries(istrig));
  }
  return r;
}


vector<double> get_al50_stopping_rates_different_p_sims(vector<double>& er) {
  er.clear();
  vector<TFile*> ifs {
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.p25.9.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.p26.2.root")
  };
  vector<TTree*> trs;
  vector<double> r;
  for (auto f : ifs) {
    trs.push_back((TTree*)f->Get("tree"));
    trs.back()->SetBranchStatus("*",              false);
    trs.back()->SetBranchStatus("M_particleName", true);
    trs.back()->SetBranchStatus("M_stopped",      true);
    trs.back()->SetBranchStatus("M_volName",      true);
    trs.back()->SetBranchStatus("M_edep",         true);
    TCut ismustop = "M_particleName==\"mu-\" && M_stopped==1 && M_volName==\"Target\"";
    TCut istrig   = "M_particleName==\"mu-\" && M_stopped==0 && M_volName==\"SiT\" && M_edep>200e-6";
    r.push_back(double(trs.back()->GetEntries(ismustop))/trs.back()->GetEntries(istrig));
    er.push_back(std::sqrt(double(trs.back()->GetEntries(ismustop)))/trs.back()->GetEntries(istrig));
  }
  return r;
}


// PP 10us, not prompt
//            nxhi  enx nmuhi     enmuhi  nblocks ntme
// Al100 26.2 4956  123 9410490   380717  66061   16006420
// Al100 26.3 63475 431 120519474 3932944 796189  200003546
// Al100 26.4 4741  126 9003408   373336  52772   15025709
// Al100 26.5 887   55  1684950   117015  10383   3116897
// Al100 29.9 1288  70  2445899   154132  18790   4448216
// Al50  25.9 75094 560 142580303 4673688 717132  325484661
// Al50  26.2 19461 273 36951400  1288096 181915  95542458
void stopping_rates_different_p() {

  vector<double> p100    = { 26.2,      26.3,      26.4,     26.5,    29.9    };
  vector<double> nx100   = { 4956,      63475,     4741,     887,     1288    };
  vector<double> nmu100  = { 9410490,   120519474, 9003408,  1684950, 2445899 };
  vector<double> enmu100 = { 380717,    3932944,   373336,   117015,  154132  };
  vector<double> ntme100 = { 16006420,  200003546, 15025709, 3116897, 4448216 };
  vector<double> nb100   = { 66061,     796189,    52772,    10383,   18790   };
  vector<double> p50     = { 25.9,      26.2     };
  vector<double> nx50    = { 75094,     19461    };
  vector<double> nmu50   = { 142580303, 36951400 };
  vector<double> enmu50  = { 4673688,   1288096  };
  vector<double> ntme50  = { 325484661, 95542458 };
  vector<double> nb50    = { 717132,    181915   };
  int n100 = p100.size(), n50 = p50.size();
  vector<double> rt100, rmu100, rtme100, ert100, ermu100, ertme100, ermu100sim;
  for (int i = 0; i < n100; ++i) {
    double t = nb100[i]*0.096;
    rt100     .push_back(nmu100[i] /t);
    rmu100    .push_back(nmu100[i] /ntme100[i]);
    rtme100   .push_back(ntme100[i]/t);
    ert100    .push_back(enmu100[i]/nmu100[i]*rt100.back());
    ermu100   .push_back(enmu100[i]/nmu100[i]*rmu100.back());
    ertme100  .push_back(rtme100.back()/std::sqrt(ntme100[i]));
    // ermu100sim.push_back(std::sqrt(rmu100sim.back()));
  }
  vector<double> rmu100sim = get_al100_stopping_rates_different_p_sims(ermu100sim);
  vector<double> rt50, rmu50, rtme50, ert50, ermu50, ertme50, ermu50sim;
  for (int i = 0; i < n50; ++i) {
    double t = nb50[i]*0.096;
    rt50     .push_back(nmu50[i] /t);
    rmu50    .push_back(nmu50[i] /ntme50[i]);
    rtme50   .push_back(ntme50[i]/t);
    ert50    .push_back(enmu50[i]/nmu50[i]*rt50.back());
    ermu50   .push_back(enmu50[i]/nmu50[i]*rmu50.back());
    ertme50  .push_back(rtme50.back()/std::sqrt(ntme50[i]));
    // ermu50sim.push_back(std::sqrt(rmu50sim.back()));
  }
  vector<double> rmu50sim = get_al50_stopping_rates_different_p_sims(ermu50sim);

  TGraph* grt100   = new TGraphErrors(n100, p100.data(), rt100  .data(), nullptr, ert100  .data());
  TGraph* grt50    = new TGraphErrors(n50,  p50 .data(), rt50   .data(), nullptr, ert50   .data());
  TGraph* grmu100  = new TGraphErrors(n100, p100.data(), rmu100 .data(), nullptr, ermu100 .data());
  TGraph* grmu50   = new TGraphErrors(n50,  p50 .data(), rmu50  .data(), nullptr, ermu50  .data());
  TGraph* grtme100 = new TGraphErrors(n100, p100.data(), rtme100.data(), nullptr, ertme100.data());
  TGraph* grtme50  = new TGraphErrors(n50,  p50 .data(), rtme50 .data(), nullptr, ertme50 .data());

  TGraph* grmu100sim = new TGraphErrors(n100, p100.data(), rmu100sim.data(), nullptr, ermu100sim.data());
  TGraph* grmu50sim  = new TGraphErrors(n50,  p50 .data(), rmu50sim .data(), nullptr, ermu50sim .data());

  grt100  ->SetMarkerColor(kBlack);
  grmu100 ->SetMarkerColor(kBlack);
  grtme100->SetMarkerColor(kBlack);
  grt100  ->SetMarkerStyle(20);
  grmu100 ->SetMarkerStyle(20);
  grtme100->SetMarkerStyle(20);
  grt100  ->SetTitle("Al100 Muon Rate (Data, PP);p [MeV/c];Rate [#mu/s]");
  grmu100 ->SetTitle("Al100 Muon Rate (Data, PP);p [MeV/c];Rate [#mu/TME]");
  grtme100->SetTitle("Al100 TME Rate (Data, PP);p [MeV/c];Rate [TME/s]");
  grt100  ->GetYaxis()->SetTitleOffset(1.3);
  grmu100 ->GetYaxis()->SetTitleOffset(1.3);
  grtme100->GetYaxis()->SetTitleOffset(1.3);

  grmu100sim->SetMarkerColor(kBlack);
  grmu100sim->SetMarkerStyle(21);
  grmu100sim->SetTitle("Al100 Muon Rate (MC);p [MeV/c];Rate [#mu/gen#mu]");
  grmu100sim->GetYaxis()->SetTitleOffset(1.3);

  grt50  ->SetMarkerColor(kBlack);
  grmu50 ->SetMarkerColor(kBlack);
  grtme50->SetMarkerColor(kBlack);
  grt50  ->SetMarkerStyle(20);
  grmu50 ->SetMarkerStyle(20);
  grtme50->SetMarkerStyle(20);
  grt50  ->SetTitle("Al50 Muon Rate (PP);p [MeV/c];Rate [#mu/s]");
  grmu50 ->SetTitle("Al50 Muon Rate (PP);p [MeV/c];Rate [#mu/TME]");
  grtme50->SetTitle("Al50 TME Rate (PP);p [MeV/c];Rate [TME/s]");
  grt50  ->GetYaxis()->SetTitleOffset(1.3);
  grmu50 ->GetYaxis()->SetTitleOffset(1.3);
  grtme50->GetYaxis()->SetTitleOffset(1.3);

  grmu50sim->SetMarkerColor(kBlack);
  grmu50sim->SetMarkerStyle(21);
  grmu50sim->SetTitle("Al50 Muon Rate (MC);p [MeV/c];Rate [#mu/gen#mu]");
  grmu50sim->GetYaxis()->SetTitleOffset(1.3);

  TCanvas* c100 = new TCanvas("c100", "c100", 1400, 1000);
  c100->Divide(2, 2);
  c100->cd(1); grt100  ->Draw("AP");
  c100->cd(2); grmu100 ->Draw("AP");
  c100->cd(3); grtme100->Draw("AP");
  c100->cd(4); grmu100sim->Draw("AP");
  c100->SaveAs("img/al100_murates.png");


  TCanvas* c50 = new TCanvas("c50", "c50", 1400, 1000);
  c50->Divide(2, 2);
  c50->cd(1); grt50  ->Draw("AP");
  c50->cd(2); grmu50 ->Draw("AP");
  c50->cd(3); grtme50->Draw("AP");
  c50->cd(4); grmu50sim->Draw("AP");
  c50->SaveAs("img/al50_murates.png");
}
