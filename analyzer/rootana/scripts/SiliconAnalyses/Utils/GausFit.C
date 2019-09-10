#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct GausFitArgs {
  std::string infilename;
  std::string inhistname;

  std::string outfilename;
  std::string outhisttitle;
  std::string outdirname;
};

void GausFit(const GausFitArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hEnergy = (TH1F*) file->Get(args.inhistname.c_str());
  if (!hEnergy) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
    return;
  }

  hEnergy->SetName("hFittedSpectrum");
  hEnergy->SetTitle(args.outhisttitle.c_str());
  
  std::stringstream axislabel;
  axislabel.str("");
  axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
  hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
  
  int max_bin = hEnergy->GetMaximumBin();
  double max_content = hEnergy->GetBinContent(max_bin);
  double max_bin_center = hEnergy->GetBinCenter(max_bin);
  TF1* fit = new TF1("gaus_fit", "[0]*TMath::Gaus(x, [1], [2])", max_bin_center-200, max_bin_center+200);
  fit->SetParameters(max_content, max_bin_center, 200);
  hEnergy->Fit("gaus_fit", "R");
  
  std::cout << args.outdirname << ": " << hEnergy->GetName() << " " << hEnergy->GetEntries() << " entries, fit mean = " << fit->GetParameter(1) << " +/- " << fit->GetParError(1) << ", sigma = " << fit->GetParameter(2) << " +/- " << fit->GetParError(2) << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hEnergy->Write();
  fit->Write();
  outfile->Write();
  outfile->Close();
}
