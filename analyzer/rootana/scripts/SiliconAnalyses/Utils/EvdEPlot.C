#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"

#include <iostream>
#include <sstream>

#include "scripts/Al50/psel.h"

enum PSelParticle {proton=0, deuteron, triton, alpha};

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

  std::string thin_layer;
  std::string thick_layer;
  std::string third_layer;
  std::string veto_layer;
  
  double min_x_energy;
  double max_x_energy;
  double x_energy_width;
  double min_y_energy;
  double max_y_energy;
  double y_energy_width;
  double min_time;
  double max_time;
  double time_width;

  bool layer_coincidence_vetos;
  double layer12_coincidence_time;
  double layer23_coincidence_time;
  
  bool early_time_veto;
  double early_time_cut;

  bool do_cut;
  std::string cutfilename;
  std::string cutname;

  bool do_psel;
  PSelParticle psel_particle;
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
  std::string histname = "hEvdE_TwoLayer_12";
  TH2F* hEvdE_TwoLayer_12 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_TwoLayer_12->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_TwoLayer_12->SetYTitle("E_{1} [keV]");
  histname = "hSingleDetAxes_TwoLayer_12";
  TH2F* hSingleDetAxes_TwoLayer_12 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_TwoLayer_12->SetXTitle("E_{2} [keV]");
  hSingleDetAxes_TwoLayer_12->SetYTitle("E_{1} [keV]");

  histname = "hEvdE_ThreeLayer_123";
  TH2F* hEvdE_ThreeLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_ThreeLayer_123->SetXTitle("E_{1} + E_{2} + E_{3} [keV]");
  hEvdE_ThreeLayer_123->SetYTitle("E_{1} + E_{2} [keV]");
  histname = "hEvdE_TwoLayer_12not3";
  TH2F* hEvdE_TwoLayer_12not3 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_TwoLayer_12not3->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_TwoLayer_12not3->SetYTitle("E_{1} [keV]");
  histname = "hEvdE_TwoLayer_123";
  TH2F* hEvdE_TwoLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hEvdE_TwoLayer_123->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_TwoLayer_123->SetYTitle("E_{1} [keV]");

  histname = "hSingleDetAxes_TwoLayer_123";
  TH2F* hSingleDetAxes_TwoLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_TwoLayer_123->SetXTitle("E_{2} [keV]");
  hSingleDetAxes_TwoLayer_123->SetYTitle("E_{1} [keV]");
  histname = "hSingleDetAxes_ThreeLayer_23";
  TH2F* hSingleDetAxes_ThreeLayer_23 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_ThreeLayer_23->SetXTitle("E_{3} [keV]");
  hSingleDetAxes_ThreeLayer_23->SetYTitle("E_{2} [keV]");
  histname = "hSingleDetAxes_ThreeLayer_13";
  TH2F* hSingleDetAxes_ThreeLayer_13 = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,args.min_x_energy,args.max_x_energy, n_y_energy_bins,args.min_y_energy,args.max_y_energy);
  hSingleDetAxes_ThreeLayer_13->SetXTitle("E_{3} [keV]");
  hSingleDetAxes_ThreeLayer_13->SetYTitle("E_{1} [keV]");

  histname = "hTime_12";
  int n_time_bins = (args.max_time - args.min_time) / args.time_width;
  TH1F* hTime_12 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hTime_12->SetXTitle("Time [ns]");
  std::stringstream axislabel;
  axislabel << "Count / " << args.time_width << " ns";
  hTime_12->SetYTitle(axislabel.str().c_str());

  histname = "hTime_12not3";
  TH1F* hTime_12not3 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hTime_12not3->SetXTitle("Time [ns]");
  hTime_12not3->SetYTitle(axislabel.str().c_str());

  histname = "hTime_123";
  TH1F* hTime_123 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hTime_123->SetXTitle("Time [ns]");
  hTime_123->SetYTitle(axislabel.str().c_str());

  std::string outtreename = "cuttree";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());
  cuttree->Branch("layer_coincidence_vetos", &args.layer_coincidence_vetos);
  if (args.layer_coincidence_vetos) {
    cuttree->Branch("layer12_coincidence_time", &args.layer12_coincidence_time);
    cuttree->Branch("layer23_coincidence_time", &args.layer23_coincidence_time);
  }
  cuttree->Branch("early_time_veto", &args.early_time_veto);
  if (args.early_time_veto) {
    cuttree->Branch("min_time", &args.early_time_cut);
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

  vector<ParticleLikelihood::PSelPow> pls_r = ParticleLikelihood::LoadParticleLikelihoodsPow('r');

  double thin_energy;
  double thin_time;
  int thin_tpi_id;
  double thick_energy;
  double thick_time;
  int thick_tpi_id;
  double third_energy;
  double third_time;
  int third_tpi_id;
  armtree->SetBranchAddress((args.thin_layer+"_energy").c_str(), &thin_energy);
  armtree->SetBranchAddress((args.thin_layer+"_time").c_str(), &thin_time);
  armtree->SetBranchAddress((args.thin_layer+"_tpi_id").c_str(), &thin_tpi_id);
  armtree->SetBranchAddress((args.thick_layer+"_energy").c_str(), &thick_energy);
  armtree->SetBranchAddress((args.thick_layer+"_time").c_str(), &thick_time);
  armtree->SetBranchAddress((args.thick_layer+"_tpi_id").c_str(), &thick_tpi_id);
  armtree->SetBranchAddress((args.third_layer+"_energy").c_str(), &third_energy);
  armtree->SetBranchAddress((args.third_layer+"_time").c_str(), &third_time);
  armtree->SetBranchAddress((args.third_layer+"_tpi_id").c_str(), &third_tpi_id);
  
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
    
    if (args.layer_coincidence_vetos) {
      if (std::fabs(thin_time - thick_time) >= args.layer12_coincidence_time) {
	continue; // to next arm event
      }
    }

    if (args.early_time_veto) {
      if ( (thin_tpi_id>=0 && thin_time < args.early_time_cut) ||
	   (thick_tpi_id>=0 && thick_time < args.early_time_cut) ) {
	continue; // to next arm events
      }
    }

    if (third_tpi_id >= 0 && std::fabs(thick_time - third_time) < args.layer23_coincidence_time) { // if we have a pulse in the third layer and it's coincidenct
      bool is_particle = true;
      if (args.do_cut && cutInfo.tCutG_three_layer) { // we might not have a three layer cut...
	if (!cutInfo.tCutG_three_layer->IsInside(total_energy+third_energy, thin_energy+thick_energy) ) {
	  is_particle = false;
	}
      }
      if (is_particle) {
	hEvdE_ThreeLayer_123->Fill(total_energy+third_energy, thin_energy+thick_energy);
	hSingleDetAxes_ThreeLayer_23->Fill(third_energy, thick_energy);
	hSingleDetAxes_ThreeLayer_13->Fill(third_energy, thin_energy);
      }

      // For the two layer plots, we care about whether it passes the two layer cut not the three layer plot
      is_particle = true;
      if (args.do_cut) {
	if (!cutInfo.tCutG_two_layer->IsInside(total_energy, thin_energy) ) {
	  is_particle = false;
	}
      }
      if (is_particle) {
	hEvdE_TwoLayer_123->Fill(total_energy, thin_energy);
	hTime_123->Fill(thin_time);
	hSingleDetAxes_TwoLayer_123->Fill(thick_energy, thin_energy);
      }
    }
    else { // this is essentially vetoing on the third layer
      bool is_particle = true;
      if (args.do_cut) {
	if (!cutInfo.tCutG_two_layer->IsInside(total_energy, thin_energy) ) {
	  is_particle = false;
	}
      }
      if (args.do_psel) {
	if(!pls_r[args.psel_particle].IsParticle(total_energy, thin_energy)) {
	  is_particle = false;
	}
      }
      if (is_particle) {
	hEvdE_TwoLayer_12not3->Fill(total_energy, thin_energy);
	hTime_12not3->Fill(thin_time);
      }
    }

    bool is_particle = true;
    if (args.do_cut) {
      if (!cutInfo.tCutG_two_layer->IsInside(total_energy, thin_energy) ) {
	is_particle = false;
      }
    }
    if (args.do_psel) {
      if(!pls_r[args.psel_particle].IsParticle(total_energy, thin_energy)) {
	is_particle = false;
      }
    }
    if (is_particle) {
      hEvdE_TwoLayer_12->Fill(total_energy, thin_energy); // don't veto on the third layer for the two layer analysis
      hSingleDetAxes_TwoLayer_12->Fill(thick_energy, thin_energy); // don't veto on the third layer for the two layer analysis
      hTime_12->Fill(thin_time);
    }
  }
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  std::string outdirname = args.outdirname;
  TDirectory* outdir = outfile->mkdir(outdirname.c_str());
  outdir->cd();
  hEvdE_TwoLayer_12->Write();
  hEvdE_ThreeLayer_123->Write();
  hEvdE_TwoLayer_12not3->Write();
  hEvdE_TwoLayer_123->Write();
  hTime_12->Write();
  hTime_12not3->Write();
  hTime_123->Write();
  cuttree->Fill();
  cuttree->Write();
  if (args.do_cut) {
    cutInfo.tCutG_two_layer->Write();
    if (cutInfo.tCutG_three_layer) {
      cutInfo.tCutG_three_layer->Write();
    }
  }
  hSingleDetAxes_TwoLayer_12->Write();
  hSingleDetAxes_TwoLayer_123->Write();
  hSingleDetAxes_ThreeLayer_23->Write();
  hSingleDetAxes_ThreeLayer_13->Write();

  outfile->Write();
  outfile->Close();
}
