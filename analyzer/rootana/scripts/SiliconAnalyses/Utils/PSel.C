#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"

#include "scripts/Al50/psel.h"

#include <iostream>
#include <sstream>

struct CutInfo {
  TFile* file;
  TCutG* tCutG_two_layer;
  TCutG* tCutG_three_layer;
} cutInfo;

struct PSelArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outdirname;
};

void PSel(PSelArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  if (in_file->IsZombie()) {
    std::cout << "Problem opening file " << args.infilename << std::endl;
    return;
  }
  TH2F* hEvdE = (TTree*) in_file->Get(args.inhistname.c_str());
  if (!hEvdE) {
    std::cout << "Problem getting histogram " << args.inhistname << std::endl;
    return;
  }

  vector<ParticleLikelihood::PSelPow> pls_r = ParticleLikelihood::LoadParticleLikelihoodsPow('r');
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  std::string outdirname = args.outdirname;
  TDirectory* outdir = outfile->mkdir(outdirname.c_str());
  outdir->cd();

  outfile->Write();
  outfile->Close();
}
