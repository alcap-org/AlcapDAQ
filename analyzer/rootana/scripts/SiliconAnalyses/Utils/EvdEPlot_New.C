#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"

#include <iostream>
#include <sstream>

struct CutInfo {
  TFile* file;
  TCutG* tCutG;
} extraCut;

struct EvdEPlotArgs {
  std::string infilename;
  std::string treename;
  std::string y_varname;
  std::string x_varname;
  std::string cutcmd;
  std::string outfilename;
  std::string outhistname;
  
  double min_x_energy;
  double max_x_energy;
  double x_energy_width;
  double min_y_energy;
  double max_y_energy;
  double y_energy_width;

  bool add_tcutg;
  std::string cutfilename;
  std::string cutname;
};

void EvdEPlot_New(const EvdEPlotArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* siBlockTree = (TTree*) in_file->Get(args.treename.c_str());

  int n_x_energy_bins = (args.max_x_energy - args.min_x_energy) / args.x_energy_width;
  int n_y_energy_bins = (args.max_y_energy - args.min_y_energy) / args.y_energy_width;

  TH2F* hEvdE = new TH2F(args.outhistname.c_str(), args.outhistname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);

  std::string drawcmd = args.y_varname + ":" + args.x_varname + ">>" + args.outhistname.c_str();
  std::string cutcmd = args.cutcmd;
  if (args.add_tcutg) {
    extraCut.file = new TFile(args.cutfilename.c_str(), "READ");
    extraCut.tCutG = (TCutG*) extraCut.file->Get(args.cutname.c_str());
    extraCut.file->Close();

    extraCut.tCutG->SetVarX(args.x_varname.c_str());
    extraCut.tCutG->SetVarY(args.y_varname.c_str());
    cutcmd += " && (" + args.cutname + ")";
  }
  std::cout << "AE: " << drawcmd << std::endl;
  std::cout << "AE: " << cutcmd << std::endl;
  siBlockTree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff");
  std::cout << args.outhistname << " " << hEvdE->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hEvdE->Write();
  TNamed draw_cmd((args.outhistname + "_drawcmd").c_str(), drawcmd.c_str());
  draw_cmd.Write();
  TNamed cut_cmd((args.outhistname + "_cutcmd").c_str(), cutcmd.c_str());
  cut_cmd.Write();
  outfile->Write();
  outfile->Close();
}
