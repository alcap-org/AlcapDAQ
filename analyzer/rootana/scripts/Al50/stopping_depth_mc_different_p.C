#include "TCanvas.h"
#include "TCut.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH3.h"
#include "TH3D.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>
#include <vector>

using std::vector;

void using_histfiles() {
  vector<TFile*> ifs50 = {
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom25.6MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom25.7MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom25.8MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom25.9MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.0MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.1MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.2MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.3MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.4MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.5MeV.root"),
    new TFile("data/Al50/momscan/mudist/mudist_al50_mom26.6MeV.root")
  };
  static const int n50 = ifs50.size();
  vector<TH3*> h3s50;
  vector<TH1*> hxs50, hys50, hzs50;
  for (int i = 0; i < n50; ++i) {
    TH3* h3 = (TH3*)ifs50[i]->Get("mustopdist");
    char hxname[32], hyname[32], hzname[32];
    std::sprintf(hxname, "x50_%d",  i);
    std::sprintf(hyname, "y50_%d",  i);
    std::sprintf(hzname, "z50_%d",  i);
    h3s50.push_back(h3);
    hxs50.push_back(h3->ProjectionX(hxname));
    hys50.push_back(h3->ProjectionY(hyname));
    hzs50.push_back(h3->ProjectionZ(hzname));
    hxs50.back()->SetTitle("X Target Stop Distribution;Local X [mm]");
    hys50.back()->SetTitle("Y Target Stop Distribution;Local Y [mm]");
    hzs50.back()->SetTitle("Z Target Stop Distribution;Local Z [mm]");
    hzs50.back()->GetXaxis()->SetRangeUser(-0.025, 0.025);
  }
  TLegend* leg50 = new TLegend(0.35, 0.10, 0.65, 0.50);
  leg50->SetNColumns(2);
  leg50->SetHeader("Equivilent Beam Set Point");
  leg50->AddEntry(hys50[0], "25.6 MeV/c");
  leg50->AddEntry(hys50[1], "25.7 MeV/c");
  leg50->AddEntry(hys50[2], "25.8 MeV/c");
  leg50->AddEntry(hys50[3], "25.9 MeV/c");
  leg50->AddEntry(hys50[4], "26.0 MeV/c");
  leg50->AddEntry(hys50[5], "26.1 MeV/c");
  leg50->AddEntry(hys50[6], "26.2 MeV/c");
  leg50->AddEntry(hys50[7], "26.3 MeV/c");
  leg50->AddEntry(hys50[8], "26.4 MeV/c");
  leg50->AddEntry(hys50[9], "26.5 MeV/c");


  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 2100, 500);
  c->Divide(3);
  c->cd(1); for (int i = 0; i < n50; ++i) hxs50[i]->Draw("PLC SAME"); leg50->Draw();
  c->cd(2); for (int i = 0; i < n50; ++i) hys50[i]->Draw("PLC SAME"); leg50->Draw();
  c->cd(3); for (int i = 0; i < n50; ++i) hzs50[i]->Draw("PLC SAME"); leg50->Draw();
  c->SaveAs("img/stopping_rates_mc_different_p_histfiles.png");
}

void using_treefiles() {
  // vector<TFile*> ifs100 = {
  //   new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.2.root"),
  //   new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.3.root"),
  //   new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.4.root"),
  //   new TFile("data/mc/momscan/R15bAl100MuPreSiT.p26.5.root"),
  //   new TFile("data/mc/momscan/R15bAl100MuPreSiT.p29.9.root")
  // };
  // int n100 = ifs100.size();
  vector<TFile*> ifs50 = {
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.6MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.7MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.8MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.9MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.0MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.1MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.2MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.3MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.4MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.5MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.6MeV.1.root")
  };
  int n50 = ifs50.size();

  vector<TTree*> trs100, trs50;
  vector<TH1*> hs100, hs50;
  vector<double> stop100, stop50, pt100, pt50, scat100, scat50;
  // for (int i = 0; i < n100; ++i) {
  //   std::cout << "Al100: " << i+1 <<  "/" << n100 << std::endl;
  //   TTree* tr = (TTree*)ifs100[i]->Get("tree");
  //   tr->SetBranchStatus("*", false);
  //   tr->SetBranchStatus("M_stopped",      true);
  //   tr->SetBranchStatus("M_local_Oz",     true);
  //   tr->SetBranchStatus("M_volName",      true);
  //   tr->SetBranchStatus("M_particleName", true);
  //   tr->SetBranchStatus("M_edep",         true);
  //   trs100.push_back(tr);
  //   char hname[32], cmd[64];
  //   sprintf(hname, "h100_%d", i);
  //   sprintf(cmd, "1e4*M_local_Oz>>+%s", hname);
  //   TCut stopcut = "M_volName==\"Target\" && M_particleName==\"mu-\" && M_stopped==1";
  //   TCut ptcut   = "M_volName==\"Target\" && M_particleName==\"mu-\" && M_stopped==0";
  //   TCut detcut  = "M_volName==\"SiT\"    && M_particleName==\"mu-\" && M_edep>200e-6";

  //   TH1* h = new TH1D(hname, "MC Al100 #mu Depth;Stopping Depth [#mum];Cts/#mum",
  //                     100, -50, 50);
  //   tr->Draw(cmd, stopcut, "goff");
  //   hs100.push_back(h);

  //   double n = tr->GetEntries(detcut);
  //   stop100.push_back(tr->GetEntries(stopcut)/n);
  //   pt100  .push_back(tr->GetEntries(ptcut)/n);
  //   scat100.push_back(1.-stop100.back()-pt100.back());
  // }
  for (int i = 0; i < n50; ++i) {
    std::cout << "Al50: " << i+1 << "/" << n50 << std::endl;
    TTree* tr = (TTree*)ifs50[i]->Get("tree");
    tr->SetBranchStatus("*", false);
    tr->SetBranchStatus("M_stopped",      true);
    tr->SetBranchStatus("M_local_Oz",     true);
    tr->SetBranchStatus("M_volName",      true);
    tr->SetBranchStatus("M_particleName", true);
    tr->SetBranchStatus("M_edep",         true);
    trs50.push_back(tr);
    char hname[32], cmd[64];
    sprintf(hname, "h50_%d", i);
    sprintf(cmd, "1e4*M_local_Oz>>+%s", hname);
    TCut stopcut = "M_volName==\"Target\" && M_particleName==\"mu-\" && M_stopped==1";
    TCut ptcut   = "M_volName==\"Target\" && M_particleName==\"mu-\" && M_stopped==0";
    TCut detcut  = "M_volName==\"SiT\"    && M_particleName==\"mu-\" && M_edep>200e-6";

    TH1* h = new TH1D(hname, "MC Al50 #mu Depth;Stopping Depth [#mum];Cts/#mum",
                      50, -25, 25);
    tr->Draw(cmd, stopcut, "goff");
    hs50.push_back(h);

    double n = tr->GetEntries(detcut);
    stop50.push_back(tr->GetEntries(stopcut)/n);
    pt50  .push_back(tr->GetEntries(ptcut)/n);
    scat50.push_back(1.-stop50.back()-pt50.back());
  }

  vector<double> e100 = { 26.2, 26.3, 26.4, 26.5, 29.9 };
  vector<double> e50  = { 25.6, 25.7, 25.8, 25.9, 26.0, 26.1, 26.2, 26.3, 26.4,
                          26.5, 26.6 };
  // TGraph* gstop100   = new TGraph(e100.size(), e100.data(), stop100.data());
  // TGraph* gpt100     = new TGraph(e100.size(), e100.data(), pt100  .data());
  // TGraph* gscat100   = new TGraph(e100.size(), e100.data(), scat100.data());
  TGraph* gstop50    = new TGraph(e50.size(), e50.data(), stop50.data());
  TGraph* gpt50      = new TGraph(e50.size(), e50.data(), pt50  .data());
  TGraph* gscat50    = new TGraph(e50.size(), e50.data(), scat50.data());
  // TMultiGraph* mg100 = new TMultiGraph("mg100", "Al100 MC;Beam set point (Data equivalent) [MeV/c];Fraction [/SiT hit]");
  TMultiGraph* mg50  = new TMultiGraph("mg50",  "Al50 MC;Beam set point (Data equivalent) [MeV/c]; Fraction [/SiT hit]");
  // gstop100->SetMarkerStyle(8);
  // gpt100  ->SetMarkerStyle(8);
  // gscat100->SetMarkerStyle(8);
  gstop50 ->SetMarkerStyle(8);
  gpt50   ->SetMarkerStyle(8);
  gscat50 ->SetMarkerStyle(8);
  // gstop100->SetMarkerColor(kGreen);
  // gpt100  ->SetMarkerColor(kRed);
  // gscat100->SetMarkerColor(kBlue);
  gstop50 ->SetMarkerColor(kGreen);
  gpt50   ->SetMarkerColor(kRed);
  gscat50 ->SetMarkerColor(kBlue);
  // mg100->Add(gstop100);
  // mg100->Add(gpt100);
  // mg100->Add(gscat100);
  mg50 ->Add(gstop50);
  mg50 ->Add(gpt50);
  mg50 ->Add(gscat50);

  // TLegend* leggr100 = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* leggr50  = new TLegend(0.7, 0.5, 0.9, 0.7);
  // leggr100->AddEntry(gstop100, "Stops",          "p");
  // leggr100->AddEntry(gpt100  , "Punch-throughs", "p");
  // leggr100->AddEntry(gscat100, "Scatters",       "p");
  leggr50 ->AddEntry(gstop50,  "Stops",          "p");
  leggr50 ->AddEntry(gpt50  ,  "Punch-throughs", "p");
  leggr50 ->AddEntry(gscat50,  "Scatters",       "p");

  // TLegend* leg100 = new TLegend(0.1, 0.5, 0.4, 0.9);
  // leg100->SetHeader("Equivilent Beam Set Point");
  // leg100->AddEntry(hs100[0], "26.2 MeV/c", "l");
  // leg100->AddEntry(hs100[1], "26.3 MeV/c", "l");
  // leg100->AddEntry(hs100[2], "26.4 MeV/c", "l");
  // leg100->AddEntry(hs100[3], "26.5 MeV/c", "l");
  // leg100->AddEntry(hs100[4], "29.9 MeV/c", "l");
  TLegend* leg50 = new TLegend(0.1, 0.2, 0.4, 0.9);
  leg50->SetHeader("Equivilent Beam Set Point");
  leg50->AddEntry(hs50[0], "25.6 MeV/c");
  leg50->AddEntry(hs50[1], "25.7 MeV/c");
  leg50->AddEntry(hs50[2], "25.8 MeV/c");
  leg50->AddEntry(hs50[3], "25.9 MeV/c");
  leg50->AddEntry(hs50[4], "26.0 MeV/c");
  leg50->AddEntry(hs50[5], "26.1 MeV/c");
  leg50->AddEntry(hs50[6], "26.2 MeV/c");
  leg50->AddEntry(hs50[7], "26.3 MeV/c");
  leg50->AddEntry(hs50[8], "26.4 MeV/c");
  leg50->AddEntry(hs50[9], "26.5 MeV/c");

  gStyle->SetOptStat(0);
  // TCanvas* c = new TCanvas("c", "c", 1400, 500);
  TCanvas* c = new TCanvas("c", "c", 700, 500);
  // c->Divide(2);
  // c->cd(1); for (auto h : hs100) h->Draw("PLC SAME"); leg100->Draw();
  /*c->cd(2);*/ for (auto h : hs50)  h->Draw("PLC SAME"); leg50 ->Draw();
  c->SaveAs("img/stopping_depth_mc_different_p.png");

  // TCanvas* cmg = new TCanvas("cmg", "cmg", 1400, 500);
  TCanvas* cmg = new TCanvas("cmg", "cmg", 700, 500);
  // cmg->Divide(2);
  // cmg->cd(1); mg100->Draw("AP"); leggr100->Draw();
  /*cmg->cd(2);*/ mg50 ->Draw("AP"); leggr50 ->Draw();
  cmg->SaveAs("img/stopping_rates_mc_different_p.png");

}


void stopping_depth_mc_different_p() {
  using_histfiles();
  // using_treefiles();
}
