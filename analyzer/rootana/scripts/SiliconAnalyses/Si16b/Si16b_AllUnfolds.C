#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_ResponseMatrix.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_TimeCut.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_FlatBackground.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_DecayElectronCorrection.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_GeometricAcceptance.C"

void Si16b_AllUnfolds() {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  std::string infilename = "~/data/results/Si16b/raw_spectra.root";
  std::string outfilename = "~/data/results/Si16b/unfold.root";


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  // For protons, deuterons etc
  const int n_particles = 5;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha", "proton_combined"};
  TH1D* hSum = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string i_particle_name = particle_names[i_particle];

    std::string inhistname = i_particle_name + "/hRawSpectrum";
    std::string incuttreename = i_particle_name + "/cuttree";
    std::string outdirname = "TimeCut_" + i_particle_name;
    Si16b_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname);

    std::string datahistname = outdirname + "/hCorrectedSpectrum";
    std::string mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_" + i_particle_name + "s_new_wBugFix.root";
    std::string mcresponsename = "SiR_response";
    int rebin_factor = 50;
    outdirname = "ResponseMatrix_" + i_particle_name;
    Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

    // Sum the histograms for a total
    if (i_particle < 4) {
      std::string final_histname = "ResponseMatrix_" + i_particle_name + "/hUnfoldedSpectrum";
      TFile* outfile2 = new TFile(outfilename.c_str(), "UPDATE");
      TH1D* final_hist = (TH1D*) outfile2->Get(final_histname.c_str());
      if (!final_hist) {
	std::cout << "Error: Problem getting histogram " << final_histname << std::endl;
	return;
      }
      if (!hSum) {
	hSum = (TH1D*) final_hist->Clone("hUnfoldedSpectrum");
	hSum->SetDirectory(0);
      }
      else {
	hSum->Add(final_hist);
      }
      outfile2->Close();
    }
  }

  TFile* outfile2 = new TFile(outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile2->mkdir("AllParticles");
  outdir->cd();
  hSum->Write();
  outfile2->Write();
  outfile2->Close();


  // For inclusive analysis (SiL3)
  std::string indirname = "SiL3_TimeSlice1000_5000";
  std::string inhistname = indirname + "/hRawSpectrum";
  std::string cutfilename = infilename;
  std::string cuttreename = indirname + "/cuttree";
  std::string corrfilename = infilename;
  std::string corrhistname = "SiL3_FlatBkg/hRawSpectrum";
  std::string corrtreename = "SiL3_FlatBkg/cuttree";
  std::string outdirname = "FlatBkg_SiL3";
  Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "TimeCut_SiL3";
  Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, cuttreename, outdirname);

  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "GeomAcceptance_SiL3";
  Si16b_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 0.01002);
  
  std::string datahistname = "TimeCut_SiL3/hCorrectedSpectrum";
  std::string mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_proton_combineds_new_wBugFix.root";
  std::string mcresponsename = "SiL_two_layer_response";
  int rebin_factor = 12;
  outdirname = "ResponseMatrix_SiL3";
  Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);
  //  Si16b_Unfold_DecayElectronCorrection(infilename, outfilename);

  // For SiR2 analysis
  indirname = "SiR2_TimeSlice1000_5000";
  inhistname = indirname + "/hRawSpectrum";
  cutfilename = infilename;
  cuttreename = indirname + "/cuttree";
  corrfilename = infilename;
  corrhistname = "SiR2_FlatBkg/hRawSpectrum";
  corrtreename = "SiR2_FlatBkg/cuttree";
  outdirname = "FlatBkg_SiR2";
  Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "TimeCut_SiR2";
  Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, cuttreename, outdirname);

  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "GeomAcceptance_SiR2";
  Si16b_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 0.01074);
}
