#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct ExtractBandArgs {
  std::string infilename;
  std::string inhistname;
  std::string cutfilename;
  std::string cutname;
  std::string outfilename;
  std::string outhistname;
};

void ExtractBand(ExtractBandArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");

  TH2F* hEvdE = (TH2F*) in_file->Get(args.inhistname.c_str());

  TFile* cut_file = new TFile(args.cutfilename.c_str(), "READ");
  TCutG* cut = (TCutG*) file->Get(args.cutname.c_str());

    hEvdE->Fill(total_energy, thin_energy);
  }
  
  std::cout << args.outhistname << " " << hEvdE->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hEvdE->Write();
  cuttree->Fill();
  cuttree->Write();
  if (args.do_proton_cut) {
    args.electron_spot_cut->Write();
    args.punchthrough_cut->Write();
    args.deuteron_removal_cut->Write();
  }

  outfile->Write();
  outfile->Close();
}
