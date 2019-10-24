#include "./scripts/Al50/util.h"

#include "TFile.h"
#include "TH3.h"
#include "TH3I.h"
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
  TFile* ofile = new TFile(ofname, "RECREATE");
  if (ifile->IsZombie())
    PrintAndThrow("Cannot find file!");

  TTree* itr = (TTree*)ifile->Get("tree");
  vector<string> *particles = nullptr, *vols = nullptr;
  vector<int>    *stops = nullptr;
  vector<double> *xs = nullptr, *ys = nullptr, *zs = nullptr;
  itr->SetBranchAddress("M_particleName", &particles);
  itr->SetBranchAddress("M_volName",      &vols);
  itr->SetBranchAddress("M_stopped",      &stops);
  itr->SetBranchAddress("M_local_Ox",     &xs); // cm
  itr->SetBranchAddress("M_local_Oy",     &ys); // cm
  itr->SetBranchAddress("M_local_Oz",     &zs); // cm

  TTree* otr = new TTree("mustoppos", "Muon stopping positions");
  // otr->SetDirectory(ofile);
  double x, y, z;
  otr->Branch("x", &x);
  otr->Branch("y", &y);
  otr->Branch("z", &z);

  TH3* h = new TH3I("mustopdist", "Muon Stopping Distribution",
                    100, -50., 50., 100, -50., 50., 2000, -0.050, 0.050);
  h->SetDirectory(ofile);
  for (int i = 0; i < itr->GetEntries(); ++i) {
    itr->GetEntry(i);
    if (verbose && i % 10000 == 0)
      std::cout << i << "/" << itr->GetEntries() << std::endl;
    for (int j = 0; j < particles->size(); ++j) {
      if (stops->at(j) && particles->at(j) == "mu-" &&
          vols->at(j) == "Target") {
        // MC output is in cm, but input needs to be in mm.
        x = 10*xs->at(j); y = 10*ys->at(j); z = 10*zs->at(j);
        otr->Fill();
        h->Fill(x, y, z);
        break;
      }
    }
  }

  ofile->Write();
  ifile->Close();
  ofile->Close();
}
