#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include <iostream>
#include <sstream>

struct RawSpectrum_fromEvdEArgs {

  std::string datafilename;
  std::vector<std::string> datahistnames;
  std::vector<double> scale_ratios;
  std::string datacuttreename;

  std::string outfilename;
  std::string outdirname;
};

void RawSpectrum_fromEvdE(RawSpectrum_fromEvdEArgs& args) {

  TFile* data_file = new TFile(args.datafilename.c_str(), "READ");
  TTree* cut_tree = (TTree*) data_file->Get(args.datacuttreename.c_str());
  TH1D* raw_spectrum = NULL;
  for (std::vector<std::string>::const_iterator i_histname = args.datahistnames.begin(); i_histname != args.datahistnames.end(); ++i_histname) {
    TH2F* hEvdE = (TH2F*) data_file->Get(i_histname->c_str());
    if (!hEvdE) {
      std::cout << "Problem getting histogram " << i_histname->c_str() << std::endl;
      return;
    }

    double scale_ratio = args.scale_ratios.at(i_histname - args.datahistnames.begin());

    if (!raw_spectrum) {
      raw_spectrum = hEvdE->ProjectionX();
      std::string newname = "hRawSpectrum";
      raw_spectrum->SetName(newname.c_str());
    }
    else {
      raw_spectrum->Add(hEvdE->ProjectionX(), scale_ratio);
    }
  }
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  raw_spectrum->Write();
  cut_tree->Write();
  
  outfile->Write();
  outfile->Close();
}
