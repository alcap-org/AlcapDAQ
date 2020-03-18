void Si16b_FinalPlot_GeometricAcceptances(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_3sigma.root";

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particles[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  double total_sim_particles[n_particles] = {10e6, 10e6, 10e6, 12e6};

  numbers_file << "% Si16b_FinalPlot_GeometricAcceptances.C" << std::endl;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];
    std::string inhistname = "ResponseMatrix_" + particle + "_TCutG/hResponseMatrix";
  
    TFile* infile = new TFile(infilename.c_str(), "READ");
    TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

    double selected = hResponseMatrix->GetEntries();
    double uncertainty = std::sqrt(selected);
    double geom_acceptance = selected / total_sim_particles[i_particle];
    double geom_acceptance_err = (uncertainty / selected) * geom_acceptance;
    std::cout << particle << " Selected = " << selected << " +/- " << uncertainty << ", Geom Acceptance = " << geom_acceptance << " +/- " << geom_acceptance_err << std::endl;

    numbers_file << std::fixed << std::setprecision(4);
    numbers_file << "\\newcommand\\Sib" << Particle << "GeomAcceptance{" << geom_acceptance << "}" << std::endl;

  }
  numbers_file << std::endl;
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
