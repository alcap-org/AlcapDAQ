#ifndef CountStoppedMuons_CrossCheckSpectrum_C_
#define CountStoppedMuons_CrossCheckSpectrum_C_

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

struct CountStoppedMuons_CrossCheckSpectrumArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outtreename;

  double min_muon_energy;
  double max_muon_energy;
};

void CountStoppedMuons_CrossCheckSpectrum(const CountStoppedMuons_CrossCheckSpectrumArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << args.infilename << std::endl;
  }
  TH2F* hEnergyEnergy = (TH2F*) file->Get(args.inhistname.c_str());
  if (!hEnergyEnergy) {
    std::cout << "Problem getting histogram " << args.inhistname << std::endl;
  }
  TH1F* hCrossCheck_Spectrum = (TH1F*) hEnergyEnergy->ProjectionY("hCrossCheck_Spectrum");
  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  n_stopped_muons = hCrossCheck_Spectrum->IntegralAndError(1, hEnergyEnergy->GetNbinsX(), n_stopped_muons_error);
  std::cout << "AE: " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;


  TTree* direct_count_tree = new TTree(args.outtreename.c_str(), "");
  direct_count_tree->Branch("n_stopped_muons", &n_stopped_muons);
  direct_count_tree->Branch("n_stopped_muons_error", &n_stopped_muons_error);

  direct_count_tree->Fill();
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hCrossCheck_Spectrum->Write();
  direct_count_tree->Write();
  outfile->Write();
  outfile->Close();
}

#endif
