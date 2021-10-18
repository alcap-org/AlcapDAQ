#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"

#include <iostream>
#include <sstream>

#include "../../Al50/psel.h"

enum PSelParticle {invalid=-1, proton=0, deuteron, triton, alpha};

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
  double min_tdiff;
  double max_tdiff;
  double tdiff_width;

  bool layer_coincidence_vetos;
  double min_layer12_coincidence_time;
  double max_layer12_coincidence_time;
  double min_layer23_coincidence_time;
  double max_layer23_coincidence_time;
  
  bool do_thin_time_cut;
  double min_thin_time_cut;
  double max_thin_time_cut;
  bool do_thick_time_cut;
  double min_thick_time_cut;
  double max_thick_time_cut;

  bool do_cut;
  std::string cutfilename;
  std::string cutname;

  bool do_psel;
  PSelParticle psel_particle;

  bool do_thin_channel_cut;
  int thin_channel_cut;
  bool do_third_channel_cut;
  int third_channel_cut;

  bool debug;
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

  histname = "hThickTime_TwoLayer_12";
  int n_time_bins = (args.max_time - args.min_time) / args.time_width;
  TH1F* hThickTime_TwoLayer_12 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThickTime_TwoLayer_12->SetXTitle("t_{2} [ns]");
  std::stringstream axislabel;
  axislabel << "Count / " << args.time_width << " ns";
  hThickTime_TwoLayer_12->SetYTitle(axislabel.str().c_str());

  histname = "hThickTime_TwoLayer_12not3";
  TH1F* hThickTime_TwoLayer_12not3 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThickTime_TwoLayer_12not3->SetXTitle("t_{2} [ns]");
  hThickTime_TwoLayer_12not3->SetYTitle(axislabel.str().c_str());

  histname = "hThickTime_TwoLayer_123";
  TH1F* hThickTime_TwoLayer_123 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThickTime_TwoLayer_123->SetXTitle("t_{2} [ns]");
  hThickTime_TwoLayer_123->SetYTitle(axislabel.str().c_str());

  histname = "hThickTime_ThreeLayer_123";
  TH1F* hThickTime_ThreeLayer_123 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThickTime_ThreeLayer_123->SetXTitle("t_{2} [ns]");
  hThickTime_ThreeLayer_123->SetYTitle(axislabel.str().c_str());

  histname = "hThinTime_TwoLayer_12";
  TH1F* hThinTime_TwoLayer_12 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThinTime_TwoLayer_12->SetXTitle("t_{1} [ns]");
  axislabel.str("");
  axislabel << "Count / " << args.time_width << " ns";
  hThinTime_TwoLayer_12->SetYTitle(axislabel.str().c_str());

  histname = "hThinTime_TwoLayer_12not3";
  TH1F* hThinTime_TwoLayer_12not3 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThinTime_TwoLayer_12not3->SetXTitle("t_{1} [ns]");
  hThinTime_TwoLayer_12not3->SetYTitle(axislabel.str().c_str());

  histname = "hThinTime_TwoLayer_123";
  TH1F* hThinTime_TwoLayer_123 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThinTime_TwoLayer_123->SetXTitle("t_{1} [ns]");
  hThinTime_TwoLayer_123->SetYTitle(axislabel.str().c_str());

  histname = "hThinTime_ThreeLayer_123";
  TH1F* hThinTime_ThreeLayer_123 = new TH1F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time);
  hThinTime_ThreeLayer_123->SetXTitle("t_{1} [ns]");
  hThinTime_ThreeLayer_123->SetYTitle(axislabel.str().c_str());

  histname = "hTDiff_TwoLayer_12";
  int n_tdiff_bins = (args.max_tdiff - args.min_tdiff) / args.tdiff_width;
  TH1F* hTDiff_TwoLayer_12 = new TH1F(histname.c_str(), histname.c_str(), n_tdiff_bins,args.min_tdiff,args.max_tdiff);
  hTDiff_TwoLayer_12->SetXTitle("t_{2} - t_{1} [ns]");
  axislabel.str("");
  axislabel << "Count / " << args.tdiff_width << " ns";
  hTDiff_TwoLayer_12->SetYTitle(axislabel.str().c_str());

  histname = "hTDiff_TwoLayer_12not3";
  TH1F* hTDiff_TwoLayer_12not3 = new TH1F(histname.c_str(), histname.c_str(), n_tdiff_bins,args.min_tdiff,args.max_tdiff);
  hTDiff_TwoLayer_12not3->SetXTitle("TDiff [ns]");
  hTDiff_TwoLayer_12not3->SetYTitle(axislabel.str().c_str());

  histname = "hTDiff_TwoLayer_123";
  TH1F* hTDiff_TwoLayer_123 = new TH1F(histname.c_str(), histname.c_str(), n_tdiff_bins,args.min_tdiff,args.max_tdiff);
  hTDiff_TwoLayer_123->SetXTitle("TDiff [ns]");
  hTDiff_TwoLayer_123->SetYTitle(axislabel.str().c_str());

  histname = "hTDiff_ThreeLayer_123";
  TH1F* hTDiff_ThreeLayer_123 = new TH1F(histname.c_str(), histname.c_str(), n_tdiff_bins,args.min_tdiff,args.max_tdiff);
  hTDiff_ThreeLayer_123->SetXTitle("t_{3} - t_{2} [ns]");
  hTDiff_ThreeLayer_123->SetYTitle(axislabel.str().c_str());

  histname = "hEvt_TwoLayer_12";
  TH2F* hEvt_TwoLayer_12 = new TH2F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time, n_x_energy_bins,args.min_x_energy,args.max_x_energy);
  hEvt_TwoLayer_12->SetXTitle("t_{2} [ns]");
  hEvt_TwoLayer_12->SetYTitle("E_{1} + E_{2} [keV]");

  histname = "hEvt_TwoLayer_12not3";
  TH2F* hEvt_TwoLayer_12not3 = new TH2F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time, n_x_energy_bins,args.min_x_energy,args.max_x_energy);
  hEvt_TwoLayer_12not3->SetXTitle("t_{2} [ns]");
  hEvt_TwoLayer_12not3->SetYTitle("E_{1} + E_{2} [keV]");

  histname = "hEvt_TwoLayer_123";
  TH2F* hEvt_TwoLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time, n_x_energy_bins,args.min_x_energy,args.max_x_energy);
  hEvt_TwoLayer_123->SetXTitle("t_{2} [ns]");
  hEvt_TwoLayer_123->SetYTitle("E_{1} + E_{2} [keV]");

  histname = "hEvt_ThreeLayer_123";
  TH2F* hEvt_ThreeLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_time_bins,args.min_time,args.max_time, n_x_energy_bins,args.min_x_energy,args.max_x_energy);
  hEvt_ThreeLayer_123->SetXTitle("t_{2} [ns]");
  hEvt_ThreeLayer_123->SetYTitle("E_{1} + E_{2} + E_{3} [keV]");


  double rotated_energy_width = 0.01;
  double min_rotated_x_energy = -1.5;
  double max_rotated_x_energy = 2.0;
  int n_rotated_x_energy_bins = (max_rotated_x_energy - min_rotated_x_energy) / rotated_energy_width;
  double min_rotated_y_energy = -1.5;
  double max_rotated_y_energy = 2.0;
  int n_rotated_y_energy_bins = (max_rotated_y_energy - min_rotated_y_energy) / rotated_energy_width;
  histname = "hRotatedEvdE_TwoLayer_12";
  TH2F* hRotatedEvdE_TwoLayer_12 = new TH2F(histname.c_str(), histname.c_str(), n_rotated_x_energy_bins,min_rotated_x_energy,max_rotated_x_energy,
					    n_rotated_y_energy_bins,min_rotated_y_energy,max_rotated_y_energy);
  hRotatedEvdE_TwoLayer_12->SetXTitle("0.7071*(TMath::Log10(E_{1} + E_{2})-TMath::Log10(E_{1}))");
  hRotatedEvdE_TwoLayer_12->SetYTitle("0.7071*(TMath::Log10(E_{1} + E_{2})+TMath::Log10(E_{1}))");

  histname = "hRotatedEvdE_TwoLayer_12not3";
  TH2F* hRotatedEvdE_TwoLayer_12not3 = new TH2F(histname.c_str(), histname.c_str(), n_rotated_x_energy_bins,min_rotated_x_energy,max_rotated_x_energy,
					    n_rotated_y_energy_bins,min_rotated_y_energy,max_rotated_y_energy);
  hRotatedEvdE_TwoLayer_12not3->SetXTitle("0.7071*(TMath::Log10(E_{1} + E_{2})-TMath::Log10(E_{1}))");
  hRotatedEvdE_TwoLayer_12not3->SetYTitle("0.7071*(TMath::Log10(E_{1} + E_{2})+TMath::Log10(E_{1}))");

  histname = "hRotatedEvdE_TwoLayer_123";
  TH2F* hRotatedEvdE_TwoLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_rotated_x_energy_bins,min_rotated_x_energy,max_rotated_x_energy,
					    n_rotated_y_energy_bins,min_rotated_y_energy,max_rotated_y_energy);
  hRotatedEvdE_TwoLayer_123->SetXTitle("0.7071*(TMath::Log10(E_{1} + E_{2})-TMath::Log10(E_{1}))");
  hRotatedEvdE_TwoLayer_123->SetYTitle("0.7071*(TMath::Log10(E_{1} + E_{2})+TMath::Log10(E_{1}))");

  histname = "hRotatedEvdE_ThreeLayer_123";
  TH2F* hRotatedEvdE_ThreeLayer_123 = new TH2F(histname.c_str(), histname.c_str(), n_rotated_x_energy_bins,min_rotated_x_energy,max_rotated_x_energy,
					    n_rotated_y_energy_bins,min_rotated_y_energy,max_rotated_y_energy);
  hRotatedEvdE_ThreeLayer_123->SetXTitle("0.7071*(TMath::Log10(E_{1} + E_{2} + E_{3})-TMath::Log10(E_{1} + E_{2}))");
  hRotatedEvdE_ThreeLayer_123->SetYTitle("0.7071*(TMath::Log10(E_{1} + E_{2} + E_{3})+TMath::Log10(E_{1} + E_{2}))");

  std::string outtreename = "cuttree";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());
  cuttree->Branch("layer_coincidence_vetos", &args.layer_coincidence_vetos);
  if (args.layer_coincidence_vetos) {
    cuttree->Branch("min_layer12_coincidence_time", &args.min_layer12_coincidence_time);
    cuttree->Branch("max_layer12_coincidence_time", &args.max_layer12_coincidence_time);
    cuttree->Branch("min_layer23_coincidence_time", &args.min_layer23_coincidence_time);
    cuttree->Branch("max_layer23_coincidence_time", &args.max_layer23_coincidence_time);
  }
  cuttree->Branch("do_thin_time_cut", &args.do_thin_time_cut);
  if (args.do_thin_time_cut) {
    cuttree->Branch("min_thin_time_cut", &args.min_thin_time_cut);
    cuttree->Branch("max_thin_time_cut", &args.max_thin_time_cut);
  }
  cuttree->Branch("do_thick_time_cut", &args.do_thick_time_cut);
  if (args.do_thick_time_cut) {
    cuttree->Branch("min_thick_time_cut", &args.min_thick_time_cut);
    cuttree->Branch("max_thick_time_cut", &args.max_thick_time_cut);
  }
  cuttree->Branch("do_cut", &args.do_cut);
  if (args.do_cut) {
    cutInfo.file = new TFile(args.cutfilename.c_str(), "READ");
    if (cutInfo.file->IsZombie()) {
      std::cout << "Error: Cut file " << args.cutfilename.c_str() << " was not opened correctly" << std::endl;
      return;
    }
    std::string two_layer_cutname = args.cutname;
    cutInfo.tCutG_two_layer = (TCutG*) cutInfo.file->Get(two_layer_cutname.c_str());
    if (!cutInfo.tCutG_two_layer) {
      two_layer_cutname += "_two_layer";
      cutInfo.tCutG_two_layer = (TCutG*) cutInfo.file->Get(two_layer_cutname.c_str());
    }
    if (!cutInfo.tCutG_two_layer) {
      std::cout << "Error: Can't get TCutG " << two_layer_cutname.c_str() << std::endl;
      //      return;
    }
    
    std::string three_layer_cutname = args.cutname;// + "_three_layer";
    cutInfo.tCutG_three_layer = (TCutG*) cutInfo.file->Get(three_layer_cutname.c_str());
    if (!cutInfo.tCutG_three_layer) {
      std::cout << "Error: Can't get TCutG " << three_layer_cutname.c_str() << std::endl;
      //      return;
    }

    cutInfo.file->Close();
  }
  cuttree->Branch("do_thin_channel_cut", &args.do_thin_channel_cut);
  if (args.do_thin_channel_cut) {
    cuttree->Branch("thin_channel_cut", &args.thin_channel_cut);
  }
  cuttree->Branch("do_third_channel_cut", &args.do_third_channel_cut);
  if (args.do_third_channel_cut) {
    cuttree->Branch("third_channel_cut", &args.third_channel_cut);
  }

  bool is_si16b_analysis = true;
  vector<ParticleLikelihood::PSelPow> pls_r = ParticleLikelihood::LoadParticleLikelihoodsPow('r', is_si16b_analysis);
  vector<ParticleLikelihood::PSelPow> pls_r_3L = ParticleLikelihood::LoadThreeLayerParticleLikelihoodsPow('r', is_si16b_analysis);
  
  double thin_energy;
  double thin_time;
  int thin_tpi_id;
  double thick_energy;
  double thick_time;
  int thick_tpi_id;
  double third_energy;
  double third_time;
  int third_tpi_id;
  int thin_channel;
  int third_channel;
  armtree->SetBranchAddress((args.thin_layer+"_energy").c_str(), &thin_energy);
  armtree->SetBranchAddress((args.thin_layer+"_time").c_str(), &thin_time);
  armtree->SetBranchAddress((args.thin_layer+"_tpi_id").c_str(), &thin_tpi_id);
  armtree->SetBranchAddress((args.thick_layer+"_energy").c_str(), &thick_energy);
  armtree->SetBranchAddress((args.thick_layer+"_time").c_str(), &thick_time);
  armtree->SetBranchAddress((args.thick_layer+"_tpi_id").c_str(), &thick_tpi_id);
  armtree->SetBranchAddress((args.third_layer+"_energy").c_str(), &third_energy);
  armtree->SetBranchAddress((args.third_layer+"_time").c_str(), &third_time);
  armtree->SetBranchAddress((args.third_layer+"_tpi_id").c_str(), &third_tpi_id);
  armtree->SetBranchAddress((args.thin_layer+"_channel").c_str(), &thin_channel);
  armtree->SetBranchAddress((args.third_layer+"_channel").c_str(), &third_channel);

  int run_id;
  int block_id;
  int tme_id;
  armtree->SetBranchAddress("run_id", &run_id);
  armtree->SetBranchAddress("block_id", &block_id);
  armtree->SetBranchAddress("tme_id", &tme_id);

  int n_entries = armtree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    armtree->GetEntry(i_entry);
    if (i_entry % 1000000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    // convert to MeV
    thin_energy = thin_energy*1e-3;
    thick_energy = thick_energy*1e-3;
    third_energy = third_energy*1e-3;
    
    double total_energy = (thin_energy+thick_energy);
    double total_3L_energy = (thin_energy+thick_energy+third_energy);

    // make sure we have at least a thin and a thick hit
    if (thin_tpi_id<0 || thick_tpi_id<0) {
      continue; // to next arm event
    }

    // check that this is the thin channel we care about
    if (args.do_thin_channel_cut) {
      if (thin_channel != args.thin_channel_cut) {
	continue; //to next arm event
      }
    }
    
    if (args.layer_coincidence_vetos) {
      if ( (thick_time-thin_time) < args.min_layer12_coincidence_time ||
	   (thick_time-thin_time) > args.max_layer12_coincidence_time) {
	continue; // to next arm event
      }
    }

    if (args.do_thin_time_cut) {
      if (thin_tpi_id>=0) {
	if (thin_time < args.min_thin_time_cut || thin_time > args.max_thin_time_cut) {
	  continue; // to next arm event
	}
      }
    }
    if (args.do_thick_time_cut) {
      if (thick_tpi_id>=0) {
	if (thick_time < args.min_thick_time_cut || thick_time > args.max_thick_time_cut) {
	  continue; // to next arm event
	}
      }
    }

    if (third_tpi_id >= 0) {// if we have a pulse in the third layer
      if (args.layer_coincidence_vetos) {
	if ( (third_time-thick_time) < args.min_layer23_coincidence_time ||
	     (third_time-thick_time) > args.max_layer23_coincidence_time) { // check if we want a coincidence cut
	  continue; // to next arm event
	}
      }
      // check that this is the third channel we care about
      if (args.do_third_channel_cut) {
	// if third_channel_cut is less than 0, then just let it be any channel
	if (args.third_channel_cut >=0 && third_channel != args.third_channel_cut) {
	  continue; //to next arm event
	}
      }

      
      bool is_particle = true;
      if (args.do_cut && cutInfo.tCutG_three_layer) { // we might not have a three layer cut...
	if (!cutInfo.tCutG_three_layer->IsInside(total_energy+third_energy, thin_energy+thick_energy) ) {
	  is_particle = false;
	}
      }
      if (args.do_psel) { // only defined for protons
	if (args.psel_particle==0) {
	  if(!pls_r_3L[args.psel_particle].IsParticle(total_energy+third_energy, thin_energy+thick_energy)) {
	    is_particle = false;
	  }
	}
	else {
	  is_particle = false;
	}
      }
      if (is_particle) {
	hEvdE_ThreeLayer_123->Fill(total_3L_energy, thin_energy+thick_energy);
	hSingleDetAxes_ThreeLayer_23->Fill(third_energy, thick_energy);
	hSingleDetAxes_ThreeLayer_13->Fill(third_energy, thin_energy);
	hThickTime_ThreeLayer_123->Fill(thick_time);
	hThinTime_ThreeLayer_123->Fill(thin_time);
	hTDiff_ThreeLayer_123->Fill(third_time - thick_time);
	hEvt_ThreeLayer_123->Fill(thick_time, total_3L_energy);
	hRotatedEvdE_ThreeLayer_123->Fill(0.7071*(TMath::Log10(total_3L_energy/1e3)-TMath::Log10((thin_energy+thick_energy)/1e3)), 0.7071*(TMath::Log10(total_3L_energy/1e3)+TMath::Log10((thin_energy+thick_energy)/1e3)));
      }

      // For the two layer plots, we care about whether it passes the two layer cut not the three layer plot
      is_particle = true;
      if (args.do_cut && cutInfo.tCutG_two_layer) {
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
	hEvdE_TwoLayer_123->Fill(total_energy, thin_energy);
	hThickTime_TwoLayer_123->Fill(thick_time);
	hThinTime_TwoLayer_123->Fill(thin_time);
	hTDiff_TwoLayer_123->Fill(thick_time - thin_time);
	hSingleDetAxes_TwoLayer_123->Fill(thick_energy, thin_energy);
	hEvt_TwoLayer_123->Fill(thick_time, total_energy);
	hRotatedEvdE_TwoLayer_123->Fill(0.7071*(TMath::Log10(total_energy/1e3)-TMath::Log10(thin_energy/1e3)), 0.7071*(TMath::Log10(total_energy/1e3)+TMath::Log10(thin_energy/1e3)));
      }
    }
    else { // this is essentially vetoing on the third layer
      bool is_particle = true;
      if (args.do_cut && cutInfo.tCutG_two_layer) {
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
	hThickTime_TwoLayer_12not3->Fill(thick_time);	
	hThinTime_TwoLayer_12not3->Fill(thin_time);
	hTDiff_TwoLayer_12not3->Fill(thick_time - thin_time);
	hEvt_TwoLayer_12not3->Fill(thick_time, total_energy);
	hRotatedEvdE_TwoLayer_12not3->Fill(0.7071*(TMath::Log10(total_energy/1e3)-TMath::Log10(thin_energy/1e3)), 0.7071*(TMath::Log10(total_energy/1e3)+TMath::Log10(thin_energy/1e3)));
      
	if (args.debug) {
	  std::cout << args.outdirname << ": Run #" << run_id << ", Block #" << block_id << ", TME #" << tme_id << ", SiR1-" << thin_channel+1 << " TPI #" << thin_tpi_id << ", SiR2 TPI #" << thick_tpi_id << std::endl;
	}
      }
    }
    
    bool is_particle = true;
    if (args.do_cut && cutInfo.tCutG_two_layer) {
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
      hThickTime_TwoLayer_12->Fill(thick_time);
      hThinTime_TwoLayer_12->Fill(thin_time);
      hTDiff_TwoLayer_12->Fill(thick_time - thin_time);
      hEvt_TwoLayer_12->Fill(thick_time, total_energy);
      hRotatedEvdE_TwoLayer_12->Fill(0.7071*(TMath::Log10(total_energy/1e3)-TMath::Log10(thin_energy/1e3)), 0.7071*(TMath::Log10(total_energy/1e3)+TMath::Log10(thin_energy/1e3)));
    }
  }

  std::cout << "EvdE Plot (" << args.outdirname << "): " << hEvdE_TwoLayer_12->GetName() << " " << hEvdE_TwoLayer_12->GetEntries() << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  std::string outdirname = args.outdirname;
  TDirectory* outdir = outfile->mkdir(outdirname.c_str());
  outdir->cd();
  hEvdE_TwoLayer_12->Write();
  hEvdE_ThreeLayer_123->Write();
  hEvdE_TwoLayer_12not3->Write();
  hEvdE_TwoLayer_123->Write();
  hThickTime_TwoLayer_12->Write();
  hThickTime_TwoLayer_12not3->Write();
  hThickTime_TwoLayer_123->Write();
  hThickTime_ThreeLayer_123->Write();
  hThinTime_TwoLayer_12->Write();
  hThinTime_TwoLayer_12not3->Write();
  hThinTime_TwoLayer_123->Write();
  hThinTime_ThreeLayer_123->Write();
  hTDiff_TwoLayer_12->Write();
  hTDiff_TwoLayer_12not3->Write();
  hTDiff_TwoLayer_123->Write();
  hTDiff_ThreeLayer_123->Write();
  hEvt_TwoLayer_12->Write();
  hEvt_TwoLayer_12not3->Write();
  hEvt_TwoLayer_123->Write();
  hEvt_ThreeLayer_123->Write();
  cuttree->Fill();
  cuttree->Write();
  if (args.do_cut) {
    if (cutInfo.tCutG_two_layer) {
      cutInfo.tCutG_two_layer->Write();
    }
    if (cutInfo.tCutG_three_layer) {
      cutInfo.tCutG_three_layer->Write();
    }
  }
  hSingleDetAxes_TwoLayer_12->Write();
  hSingleDetAxes_TwoLayer_123->Write();
  hSingleDetAxes_ThreeLayer_23->Write();
  hSingleDetAxes_ThreeLayer_13->Write();
  hRotatedEvdE_TwoLayer_12->Write();
  hRotatedEvdE_ThreeLayer_123->Write();
  hRotatedEvdE_TwoLayer_12not3->Write();
  hRotatedEvdE_TwoLayer_123->Write();

  outfile->Write();
  outfile->Close();

  delete outfile;
  delete cuttree;
  delete hEvdE_TwoLayer_12;
  delete hEvdE_ThreeLayer_123;
  delete hEvdE_TwoLayer_12not3;
  delete hEvdE_TwoLayer_123;
  delete hThickTime_TwoLayer_12;
  delete hThickTime_TwoLayer_12not3;
  delete hThickTime_TwoLayer_123;
  delete hThickTime_ThreeLayer_123;
  delete hThinTime_TwoLayer_12;
  delete hThinTime_TwoLayer_12not3;
  delete hThinTime_TwoLayer_123;
  delete hThinTime_ThreeLayer_123;
  delete hTDiff_TwoLayer_12;
  delete hTDiff_TwoLayer_12not3;
  delete hTDiff_TwoLayer_123;
  delete hTDiff_ThreeLayer_123;
  delete hEvt_TwoLayer_12;
  delete hEvt_TwoLayer_12not3;
  delete hEvt_TwoLayer_123;
  delete hEvt_ThreeLayer_123;
  delete hSingleDetAxes_TwoLayer_12;
  delete hSingleDetAxes_TwoLayer_123;
  delete hSingleDetAxes_ThreeLayer_23;
  delete hSingleDetAxes_ThreeLayer_13;
  delete hRotatedEvdE_TwoLayer_12;
  delete hRotatedEvdE_ThreeLayer_123;
  delete hRotatedEvdE_TwoLayer_12not3;
  delete hRotatedEvdE_TwoLayer_123;
  delete in_file;
}
