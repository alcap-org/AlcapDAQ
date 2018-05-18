#include "./scripts/Al50/util.h"

#include "TFile.h"
#include "THnSparse.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

void R15b_Al50_MCMuStopsToMCInputTree(const char* ifname=nullptr,
                                      const char* ofname=nullptr,
                                      bool verbose=false) {
  if (!ifname) return;

  TFile* ifile = new TFile(ifname, "READ");
  if (ifile->IsZombie())
    PrintAndThrow("Cannot find file!");
  TTree* tr = (TTree*)ifile->Get("tree");

  vector<string> *particles = nullptr, *vols = nullptr;
  vector<int>    *stops = nullptr;
  vector<double> *xs = nullptr, *ys = nullptr, *zs = nullptr;
  tr->SetBranchAddress("M_particleName", &particles);
  tr->SetBranchAddress("M_volName",      &vols);
  tr->SetBranchAddress("M_stopped",      &stops);
  tr->SetBranchAddress("M_x",      &xs); // cm
  tr->SetBranchAddress("M_y",      &ys); // cm
  tr->SetBranchAddress("M_z",      &zs); // cm

  Int_t    bins[3] = {10000, 10000, 10000};
  Double_t xmin[3] = {-50., -50., -50}, xmax[3] = {50., 50., 50.};
  THnSparse* h = new THnSparseF("mustopdist", "Muon Stopping Distribution",
                                3, bins, xmin, xmax);
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 10000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    for (int j = 0; j < particles->size(); ++j) {
      if (stops->at(j) && particles->at(j) == "mu-" &&
          vols->at(j) == "Target") {
        double x[3] = { 10*xs->at(j), 10*ys->at(j), 10*zs->at(j) };
        h->Fill(x);
        break;
      }
    }
  }

  TFile* ofile = new TFile(ofname, "RECREATE");
  h->Write();
  ifile->Close();
  ofile->Close();
}
