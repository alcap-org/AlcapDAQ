#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct EvdEPlotArgs {
  std::string infilename;
  std::string treename;
  std::string outfilename;
  std::string outhistname;
  
  double min_x_energy;
  double max_x_energy;
  double x_energy_width;
  double min_y_energy;
  double max_y_energy;
  double y_energy_width;

  bool layer_coincidence_veto;
  double layer_coincidence_time;
  
  bool third_layer_veto;

  bool early_time_veto;
  double early_time_cut;

  bool do_proton_cut;
  TF1* electron_spot_cut;
  TF1* punchthrough_cut;
  TF1* deuteron_removal_cut;
};

void EvdEPlot(EvdEPlotArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* armtree = (TTree*) in_file->Get(args.treename.c_str());

  int n_x_energy_bins = (args.max_x_energy - args.min_x_energy) / args.x_energy_width;
  int n_y_energy_bins = (args.max_y_energy - args.min_y_energy) / args.y_energy_width;

  TH2F* hEvdE = new TH2F(args.outhistname.c_str(), args.outhistname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);

  std::string outtreename = args.outhistname + "_cuts";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());
  cuttree->Branch("layer_coincidence_veto", &args.layer_coincidence_veto);
  if (args.layer_coincidence_veto) {
    cuttree->Branch("layer_coincidence_time", &args.layer_coincidence_time);
  }
  cuttree->Branch("third_layer_veto", &args.third_layer_veto);
  cuttree->Branch("early_time_veto", &args.early_time_veto);
  if (args.early_time_veto) {
    cuttree->Branch("early_time_cut", &args.early_time_cut);
  }
  cuttree->Branch("do_proton_cut", &args.do_proton_cut);

  double thin_energy;
  double thin_time;
  double thick_energy;
  double thick_time;
  double third_energy;
  double third_time;
  int third_tpi_id;
  armtree->SetBranchAddress("thin_energy", &thin_energy);
  armtree->SetBranchAddress("thin_time", &thin_time);
  armtree->SetBranchAddress("thick_energy", &thick_energy);
  armtree->SetBranchAddress("thick_time", &thick_time);
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
    
    if (args.layer_coincidence_veto) {
      if (std::fabs(thin_time - thick_time) >= args.layer_coincidence_time) {
	continue; // to next arm event
      }
    }

    if (args.third_layer_veto) {
      if (third_tpi_id >= 0) {
	continue; // to next arm event
      }
    }

    if (args.early_time_veto) {
      if (thin_time < args.early_time_cut || thick_time < args.early_time_cut) {
	continue; // to next arm events
      }
    }

    if (args.do_proton_cut) {
      bool passes_electron_spot = (thin_energy > args.electron_spot_cut->Eval(total_energy));
      bool passes_punchthrough = (thin_energy > args.punchthrough_cut->Eval(total_energy));
      bool passes_deuteron_removal = (thin_energy < args.deuteron_removal_cut->Eval(total_energy));

      if (args.third_layer_veto) {
	if (!passes_electron_spot || !passes_deuteron_removal) {
	  continue; // to the next arm events
	}
      }
      else {
	if (!passes_electron_spot || !passes_punchthrough || !passes_deuteron_removal) {
	  continue; // to the next arm events
	}
      }
    }

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
