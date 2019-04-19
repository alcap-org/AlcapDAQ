//#include "scripts/SiliconAnalyses/Ti50/unfold/Ti50_Unfold_ResponseMatrix.C"
//#include "scripts/SiliconAnalyses/Ti50/unfold/Ti50_Unfold_TimeCut.C"
#include "scripts/SiliconAnalyses/Ti50/unfold/Ti50_Unfold_FlatBackground.C"
//#include "scripts/SiliconAnalyses/Ti50/unfold/Ti50_Unfold_DecayElectronCorrection.C"
//#include "scripts/SiliconAnalyses/Ti50/unfold/Ti50_Unfold_GeometricAcceptance.C"

void Ti50_AllUnfolds() {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  std::string infilename = "~/data/results/Ti50/raw_spectra.root";
  std::string outfilename = "~/data/results/Ti50/unfold.root";


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  /*
  // For protons, deuterons etc
  const int n_particles = 5;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha", "proton_combined"};
  TH1D* hSum = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string i_particle_name = particle_names[i_particle];

    std::string inhistname = i_particle_name + "/hRawSpectrum";
    std::string incuttreename = i_particle_name + "/cuttree";
    std::string outdirname = "TimeCut_" + i_particle_name;
    Ti50_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname);

    std::string datahistname = outdirname + "/hCorrectedSpectrum";
    std::string mcfilename = "~/data/mc/Ti50/R15b_Ti50_response-matrix_500keV-bins_" + i_particle_name + "s_new_wBugFix.root";
    std::string mcresponsename = "SiR_response";
    if (i_particle == 0) {
      mcresponsename = "SiR_two_layer_response";
    }
    int rebin_factor = 50;
    outdirname = "ResponseMatrix_" + i_particle_name;
    Ti50_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

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
*/

  const int n_time_slices = 6;
  double min_time_slices[n_time_slices] = {200, 1000, 2000, 3000, 4000, 1000};
  double max_time_slices[n_time_slices] = {400, 2000, 3000, 4000, 5000, 5000};

  std::stringstream time_slice_str;
  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    double i_min_time_slice = min_time_slices[i_time_slice];
    double i_max_time_slice = max_time_slices[i_time_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    // Subtract flat background first
    std::string indirname = "SiL3_" + time_slice_str.str();
    std::string inhistname = indirname + "/hRawSpectrum";
    std::string cutfilename = infilename;
    std::string cuttreename = indirname + "/cuttree";
    std::string corrfilename = infilename;
    std::string corrdirname = "SiL3_FlatBkg";
    std::string corrhistname = corrdirname + "/hRawSpectrum";
    std::string corrtreename = corrdirname + "/cuttree";
    std::string outdirname = "SiL3_FlatBkg_" + time_slice_str.str();
    Ti50_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

    // Now scale to account for the time cut
    /*    std::string incuttreename = indirname + "/cuttree";
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "SiL3_TimeCut_" + time_slice_str.str();
    Ti50_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "SiL3_GeomAcceptance_" + time_slice_str.str();
    Ti50_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 0.01002);
    */
  }
}
