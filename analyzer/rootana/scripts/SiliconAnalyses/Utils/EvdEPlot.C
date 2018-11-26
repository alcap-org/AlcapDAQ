#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"

#include <iostream>
#include <sstream>

struct CutInfo {
  TFile* file;
  TCutG* tCutG_two_layer;
  TCutG* tCutG_three_layer;
} cutInfo;

struct EvdEPlotArgs {
  std::string infilename;
  std::string treename;
  std::string outfilename;
  std::string outdirname;
  
  double min_x_energy;
  double max_x_energy;
  double x_energy_width;
  double min_y_energy;
  double max_y_energy;
  double y_energy_width;
  double min_time;
  double max_time;
  double time_width;

  bool layer_coincidence_veto;
  double layer_coincidence_time;
  
  bool early_time_veto;
  double early_time_cut;

  bool do_cut;
  std::string cutfilename;
  std::string cutname;
};

void EvdEPlot(EvdEPlotArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  if (in_file->IsZombie()) {
    std::cout << "Problem opening file " << args.infilename << std::endl;
    return;
  }
  TTree* armtree = (TTree*) in_file->Get(args.treename.c_str());
  if (!armtree) {
    std::cout << "Problem getting tree " << args.treename << std::endl;
    return;
  }

  int n_x_energy_bins = (args.max_x_energy - args.min_x_energy) / args.x_energy_width;
  int n_y_energy_bins = (args.max_y_energy - args.min_y_energy) / args.y_energy_width;
  std::string histname = "hEvdE_TwoLayer";
  TH2F* hEvdE_TwoLayer = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_TwoLayer->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_TwoLayer->SetYTitle("E_{1} [keV]");
  histname = "hSingleDetAxes_TwoLayer";
  TH2F* hSingleDetAxes_TwoLayer = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_TwoLayer->SetXTitle("E_{2} [keV]");
  hSingleDetAxes_TwoLayer->SetYTitle("E_{1} [keV]");

  histname = "hEvdE_ThreeLayer_123";
  TH2F* hEvdE_ThreeLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_ThreeLayer_123->SetXTitle("E_{1} + E_{2} + E_{3} [keV]");
  hEvdE_ThreeLayer_123->SetYTitle("E_{1} + E_{2} [keV]");
  histname = "hEvdE_ThreeLayer_12";
  TH2F* hEvdE_ThreeLayer_12 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_ThreeLayer_12->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_ThreeLayer_12->SetYTitle("E_{1} [keV]");

  histname = "hSingleDetAxes_ThreeLayer_12";
  TH2F* hSingleDetAxes_ThreeLayer_12 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_ThreeLayer_12->SetXTitle("E_{2} [keV]");
  hSingleDetAxes_ThreeLayer_12->SetYTitle("E_{1} [keV]");
  histname = "hSingleDetAxes_ThreeLayer_23";
  TH2F* hSingleDetAxes_ThreeLayer_23 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_ThreeLayer_23->SetXTitle("E_{3} [keV]");
  hSingleDetAxes_ThreeLayer_23->SetYTitle("E_{2} [keV]");
  histname = "hSingleDetAxes_ThreeLayer_13";
  TH2F* hSingleDetAxes_ThreeLayer_13 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_ThreeLayer_13->SetXTitle("E_{3} [keV]");
  hSingleDetAxes_ThreeLayer_13->SetYTitle("E_{1} [keV]");

  histname = "hTime";
  int n_time_bins = (args.max_time - args.min_time) / args.time_width;
  TH1F* hTime = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hTime->SetXTitle("Time [ns]");
  std::stringstream axislabel;
  axislabel << "Count / " << args.time_width << " ns";
  hTime->SetYTitle(axislabel.str().c_str());

  std::string outtreename = "cuttree";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());
  cuttree->Branch("layer_coincidence_veto", &args.layer_coincidence_veto);
  if (args.layer_coincidence_veto) {
    cuttree->Branch("layer_coincidence_time", &args.layer_coincidence_time);
  }
  cuttree->Branch("early_time_veto", &args.early_time_veto);
  if (args.early_time_veto) {
    cuttree->Branch("early_time_cut", &args.early_time_cut);
  }
  cuttree->Branch("do_cut", &args.do_cut);
  if (args.do_cut) {
    cutInfo.file = new TFile(args.cutfilename.c_str(), "READ");
    std::string two_layer_cutname = args.cutname + "_two_layer";
    cutInfo.tCutG_two_layer = (TCutG*) cutInfo.file->Get(two_layer_cutname.c_str());

    std::string three_layer_cutname = args.cutname + "_three_layer";
    cutInfo.tCutG_three_layer = (TCutG*) cutInfo.file->Get(three_layer_cutname.c_str());

    cutInfo.file->Close();
  }

  double thin_energy;
  double thin_time;
  int thin_tpi_id;
  double thick_energy;
  double thick_time;
  int thick_tpi_id;
  double third_energy;
  double third_time;
  int third_tpi_id;
  armtree->SetBranchAddress("thin_energy", &thin_energy);
  armtree->SetBranchAddress("thin_time", &thin_time);
  armtree->SetBranchAddress("thin_tpi_id", &thin_tpi_id);
  armtree->SetBranchAddress("thick_energy", &thick_energy);
  armtree->SetBranchAddress("thick_time", &thick_time);
  armtree->SetBranchAddress("thick_tpi_id", &thick_tpi_id);
  armtree->SetBranchAddress("third_energy", &third_energy);
  armtree->SetBranchAddress("third_time", &third_time);
  armtree->SetBranchAddress("third_tpi_id", &third_tpi_id);
  
  int n_entries = armtree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    armtree->GetEntry(i_entry);
    if (i_entry % 10000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    double total_energy = thin_energy+thick_energy;

    // make sure we have at least a thin and a thick hit
    if (thin_tpi_id<0 || thick_tpi_id<0) {
      continue; // to next arm event
    }
    
    if (args.layer_coincidence_veto) {
      if (std::fabs(thin_time - thick_time) >= args.layer_coincidence_time) {
	continue; // to next arm event
      }
    }

    if (args.early_time_veto) {
      if ( (thin_tpi_id>=0 && thin_time < args.early_time_cut) ||
	   (thick_tpi_id>=0 && thick_time < args.early_time_cut) ) {
	continue; // to next arm events
      }
    }

    if (third_tpi_id >= 0 && std::fabs(thick_time - third_time) < args.layer_coincidence_time) { // if we have a pulse in the third layer and it's coincidenct
      if (args.do_cut && cutInfo.tCutG_three_layer) { // we might not have a three layer cut...
	if (!cutInfo.tCutG_three_layer->IsInside(total_energy+third_energy, thin_energy+thick_energy) ) {
	  continue; // to the next arm event
	}
      }
      hEvdE_ThreeLayer_123->Fill(total_energy+third_energy, thin_energy+thick_energy);
      hSingleDetAxes_ThreeLayer_12->Fill(thick_energy, thin_energy);
      hSingleDetAxes_ThreeLayer_23->Fill(third_energy, thick_energy);
      hSingleDetAxes_ThreeLayer_13->Fill(third_energy, thin_energy);
    }
    else { // this is essentially vetoing on the third layer
      if (args.do_cut) {
	if (!cutInfo.tCutG_two_layer->IsInside(total_energy, thin_energy) ) {
	  continue; // to the next arm event
	}
      }
      hEvdE_ThreeLayer_12->Fill(total_energy, thin_energy);
    }

    if (args.do_cut) {
      if (!cutInfo.tCutG_two_layer->IsInside(total_energy, thin_energy) ) {
	continue; // to the next arm event
      }
    }
    hEvdE_TwoLayer->Fill(total_energy, thin_energy); // don't veto on the third layer for the two layer analysis
    hSingleDetAxes_TwoLayer->Fill(thick_energy, thin_energy); // don't veto on the third layer for the two layer analysis
    hTime->Fill(thin_time);
  }
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  std::string outdirname = args.outdirname;
  TDirectory* outdir = outfile->mkdir(outdirname.c_str());
  outdir->cd();
  hEvdE_TwoLayer->Write();
  hEvdE_ThreeLayer_123->Write();
  hEvdE_ThreeLayer_12->Write();
  hTime->Write();
  cuttree->Fill();
  cuttree->Write();
  if (args.do_cut) {
    cutInfo.tCutG_two_layer->Write();
    if (cutInfo.tCutG_three_layer) {
      cutInfo.tCutG_three_layer->Write();
    }
  }
  hSingleDetAxes_TwoLayer->Write();
  hSingleDetAxes_ThreeLayer_12->Write();
  hSingleDetAxes_ThreeLayer_23->Write();
  hSingleDetAxes_ThreeLayer_13->Write();

  outfile->Write();
  outfile->Close();
}
