#include "scripts/PlotTME_Si16b_EvdE/Cut.h"

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TNtuple.h"
#include "TH2.h"

#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooPlot.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include <iostream>
#include <sstream>

TH1* PlotFromNTuple(std::string filename, const std::vector<Cut*>& cuts, std::string dirname, std::string ntuplename, std::string histname, std::string x_varname, int n_bins_x, double min_x, double max_x, std::string y_varname="", int n_bins_y=0, double min_y=0, double max_y=0) {

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << filename.c_str() << std::endl;
    return NULL;
  }
  // Check that the correct EvdE folder was created
  TDirectoryFile* evde_dir = (TDirectoryFile*) file->Get(dirname.c_str());
  if (!evde_dir) {
    std::cout << "Error: " << dirname << " folder doesn't exist in output file" << std::endl;
    return NULL;
  }

  // Form the full TCut
  TCut full_cut;
  for (std::vector<Cut*>::const_iterator i_cut = cuts.begin(); i_cut != cuts.end(); ++i_cut) {
    full_cut += (*i_cut)->cut;
  }
  //  full_cut += "SiR1Time<2000 && SiR1Time>=100";
  std::cout << "Full Cut = " << full_cut << std::endl;
  
  // Get the ntuple of hits for this arm
  TNtuple* arm_hits = (TNtuple*) evde_dir->Get(ntuplename.c_str());
  if (!arm_hits) {
    std::cout << "Error: Could not find " << ntuplename << std::endl;
    return NULL;
  }

  TH1* hHist;
  std::string drawcmd;
  if (y_varname == "") {
    hHist = new TH1F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x);
    drawcmd = x_varname + ">>" + histname;
  }
  else {
    hHist = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x, n_bins_y,min_y,max_y);
    drawcmd = y_varname + ":" + x_varname + ">>" + histname;
  }
  arm_hits->Draw(drawcmd.c_str(), full_cut, "goff");
    
  return hHist;
}
