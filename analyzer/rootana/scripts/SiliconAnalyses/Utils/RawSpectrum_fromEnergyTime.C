#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct RawSpectrum_fromEnergyTimeArgs {
  std::string infilename;
  std::string inhistname;

  std::string outfilename;
  std::string outdirname;
  
  double min_time;
  double max_time;

  int rebin_factor;
};

void RawSpectrum_fromEnergyTime(const RawSpectrum_fromEnergyTimeArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH2F* hEnergyTime = (TH2F*) file->Get(args.inhistname.c_str());
  if (!hEnergyTime) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
    return;
  }

  double min_time = args.min_time;
  double max_time = args.max_time;
  TTree* cuttree = new TTree("cuttree", "cuttree");
  cuttree->Branch("min_time", &min_time);
  cuttree->Branch("max_time", &max_time);
  cuttree->Fill();

  int min_time_bin = hEnergyTime->GetXaxis()->FindBin(args.min_time);
  int max_time_bin = hEnergyTime->GetXaxis()->FindBin(args.max_time)-1;
  std::string outhistname = "hRawSpectrum";
  TH1D* hRawSpectrum = hEnergyTime->ProjectionY(outhistname.c_str(), min_time_bin, max_time_bin);
  hRawSpectrum->Sumw2();
  hRawSpectrum->Rebin(args.rebin_factor);
  
  std::cout << args.outdirname << ": " << hRawSpectrum->GetName() << " " << hRawSpectrum->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hRawSpectrum->Write();
  cuttree->Write();
  outfile->Write();
  outfile->Close();
}
