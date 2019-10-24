#include "TArrow.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

#include <map>
#include <string>
#include <vector>

void construct_distribution(const char* fname, TGraph*& g, TH1*& h) {
  TFile* f = new TFile(fname);
  TTree* tr = (TTree*)f->Get("tree");
  tr->SetBranchStatus("*",              0);
  tr->SetBranchStatus("M_stopped",      1);
  tr->SetBranchStatus("M_Ox",           1);
  tr->SetBranchStatus("M_Oz",           1);
  tr->SetBranchStatus("M_volName",      1);
  tr->SetBranchStatus("M_particleName", 1);
  std::vector<int> *stoppeds = nullptr;
  std::vector<double> *xs = nullptr, *zs = nullptr;
  std::vector<std::string> *vols = nullptr, *ptypes = nullptr;
  tr->SetBranchAddress("M_stopped",      &stoppeds);
  tr->SetBranchAddress("M_Ox",           &xs);
  tr->SetBranchAddress("M_Oz",           &zs);
  tr->SetBranchAddress("M_volName",      &vols);
  tr->SetBranchAddress("M_particleName", &ptypes);
  std::vector<double> vx, vz, vx_detected, vz_detected;
  std::map<std::string,int> stopvols, stopvols_detected;
  int nmu = 0;
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    bool detected = false;
    for (int j = 0; j < xs->size(); ++j) {
      if (!detected && ptypes->at(j) == "mu-" && vols->at(j) == "SiT")
        detected = true;
      if (stoppeds->at(j) == 1 && ptypes->at(j) == "mu-") {
        vx.push_back(xs->at(j));
        vz.push_back(zs->at(j));
        ++stopvols[vols->at(j)];
        if (detected) {
          vx_detected.push_back(xs->at(j));
          vz_detected.push_back(zs->at(j));
          ++stopvols_detected[vols->at(j)];
          ++nmu;
        }
      }
    }
  }
  g = new TGraph(vx.size());
  for (int i = 0; i < vx_detected.size(); ++i)
    g->SetPoint(i, vz_detected[i], vx_detected[i]);

  std::map<std::string,int> voltype_stops;
  voltype_stops["Target"] = stopvols_detected["Target"];
  voltype_stops["Upstream Shielding"] =
  stopvols_detected["CollimatorPlastic"] + stopvols_detected["CollimatorLead"];
  voltype_stops["Downstream Shielding"] =
  stopvols_detected["BackWall"] + stopvols_detected["TargetMount"];
  voltype_stops["Chamber"] =
  stopvols_detected["Chamber"]        + stopvols_detected["ChamberIn"]   +
  stopvols_detected["BeamWindow"]     + stopvols_detected["BeamWindowT"] +
  stopvols_detected["DetectorWindow"] + stopvols_detected["flUL"]        +
  stopvols_detected["flUR"];
  voltype_stops["SiT"]       = stopvols_detected["SiT"];
  voltype_stops["SiR"]       =
  stopvols_detected["SiR1"] + stopvols_detected["SiR2"];
  voltype_stops["SiL"]       =
  stopvols_detected["SiL1"] + stopvols_detected["SiL3"];
  h = new TH1D("h", "Stopping Volumes;Volume;Stopping fraction normalized to SiT", 7, 0., 1.);
  h->GetXaxis()->SetBinLabel(1, "Target");
  h->GetXaxis()->SetBinLabel(2, "Upstream Shielding");
  h->GetXaxis()->SetBinLabel(3, "Downstream Shielding");
  h->GetXaxis()->SetBinLabel(4, "Chamber");
  h->GetXaxis()->SetBinLabel(5, "SiT");
  h->GetXaxis()->SetBinLabel(6, "SiR");
  h->GetXaxis()->SetBinLabel(7, "SiL");
  h->Fill("Target",               voltype_stops["Target"]);
  h->Fill("Upstream Shielding",   voltype_stops["Upstream Shielding"]);
  h->Fill("Downstream Shielding", voltype_stops["Downstream Shielding"]);
  h->Fill("Chamber",              voltype_stops["Chamber"]);
  h->Fill("SiT",                  voltype_stops["SiT"]);
  h->Fill("SiR",                  voltype_stops["SiR"]);
  h->Fill("SiL",                  voltype_stops["SiL"]);
  h->Scale(1./nmu);
}

void mustops_chamber() {
  TGraph *gsi = nullptr, *gal = nullptr;
  TH1    *hsi = nullptr, *hal = nullptr;
  construct_distribution("data/mc/R15bSi16bMuPreSiTAllSD_1.root", gsi, hsi);
  construct_distribution("data/mc/R15bAl50MuPreSiTAllSD_1.root",  gal, hal);

  gsi->SetName("gsi");
  gal->SetName("gal");
  gsi->SetTitle("Si16b stopping position of muons;z [cm];x [cm]");
  gal->SetTitle("Al50 stopping position of muons;z [cm];x [cm]");
  gsi->SetMarkerStyle(1);
  gal->SetMarkerStyle(1);
  TArrow* arr = new TArrow(-24, 0, -14, 0, 0.05);
  arr->SetLineColor(kRed);
  TLatex* txt = new TLatex(-22, 3, "#mu^{-}");
  txt->SetTextColor(kRed);
  hsi->SetLineColor(kBlack);
  hal->SetLineColor(kBlack);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hsi, "Si16b");
  leg->AddEntry(hal, "Al50");

  gStyle->SetOptStat(0);

  TCanvas* csi = new TCanvas("csi", "positions", 1400, 500);
  csi->Divide(2);
  csi->cd(1);
  gsi->Draw("AP");
  arr->Draw();
  txt->Draw();
  csi->cd(2);
  hsi->Draw("HIST");
  csi->SaveAs("img/chamber_si16b_stopping_distribution.png");

  TCanvas* cal = new TCanvas("cal", "positions", 1400, 500);
  cal->Divide(2);
  cal->cd(1);
  gal->Draw("AP");
  arr->Draw();
  txt->Draw();
  cal->cd(2);
  hal->Draw("HIST");
  cal->SaveAs("img/chamber_al50_stopping_distribution.png");

  TCanvas* ccomp = new TCanvas("ccomp", "compare", 700, 500);
  hsi->SetLineColor(kRed);
  hal->Draw("SAME HIST");
  hsi->Draw("SAME HIST");
  leg->Draw();
  ccomp->SaveAs("img/chamber_stopvols_compare_si16b_al50.png");
  std::printf("Target stopped muons that were detected:\n\tSi16b: %g\tAl50: %g\n", double(hsi->GetBinContent(1)), double(hal->GetBinContent(1)));
}

