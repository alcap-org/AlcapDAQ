#include "unfold/Al50_Unfold_ResponseMatrix.C"

void Al50_AllUnfolds() {

  gROOT->ProcessLine(".L ~/libRooUnfold.so");

  std::string infilename = "/data/ssd3/R15bAnalysis/hists/al50-folded.root";
  std::string outfilename = "~/data/results/Al50/unfold_fromMarkHists_midHighE.root";
      
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_particles = 1;
  std::string particle_names[n_particles] = {"proton"};

  const int n_arms = 2;
  std::string armnames[n_arms] = {"SiR", "SiL"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string armname = armnames[i_arm];
    
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string i_particle_name = particle_names[i_particle];
      
      std::string datahistname = "h";
      datahistname += i_particle_name[0];
      datahistname += armname;

      std::string mcresponsename = armname + "_two_layer_response";
      if (i_particle_name == "proton" && armname == "SiR") {
	mcresponsename = armname + "_three_layer_response";
      }
      std::string mcfilename = "/data/R15bMC/respMatrix_100M_Al50_" + i_particle_name + "_100keVBins_midHighE.root";//_notAllEntries.root";

      double bin_width_val = 0.5;
      double highE_cut = 100000;
      std::string outdirname = "ResponseMatrix_" + armname;
      Al50_Unfold_ResponseMatrix(infilename, outfilename, datahistname, mcfilename, mcresponsename, bin_width_val, outdirname, highE_cut);
    }
  }
}
