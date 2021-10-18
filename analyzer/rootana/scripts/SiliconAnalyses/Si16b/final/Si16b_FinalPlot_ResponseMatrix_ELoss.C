void Si16b_FinalPlot_ResponseMatrix_ELoss(std::string savedir = "") {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particles[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  int n_sim_particles[n_particles] = {10, 10, 10, 12};
  double zoomed_max_y[n_particles] = {2.5, 3, 3.5, 10};
  
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];
    std::string n_M = std::to_string(n_sim_particles[i_particle]);
    std::string infilename = "~/data/mc/Si16b/respMatrix_" + n_M + "M_Geom-P5_" + particle + "_1MeVBins.root";
    std::string inhistname = "SiR_hELoss";
  
    TFile* infile = new TFile(infilename.c_str(), "READ");
    TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

    TCanvas* c_Response = new TCanvas();
    
    std::string histtitle;
    histtitle = "Si16b MC, Right Arm, " + particle + " Energy Loss";
    hResponseMatrix->SetTitle(histtitle.c_str());
    hResponseMatrix->SetStats(false);
    //  gStyle->SetOptFit(1);
    hResponseMatrix->GetXaxis()->SetTitleOffset(1.0);
    hResponseMatrix->GetYaxis()->SetTitleOffset(1.05);
    hResponseMatrix->GetXaxis()->SetLabelSize(0.05);
    hResponseMatrix->GetYaxis()->SetLabelSize(0.045);
    hResponseMatrix->GetXaxis()->SetTitleSize(0.05);
    hResponseMatrix->GetYaxis()->SetTitleSize(0.05);
    hResponseMatrix->SetXTitle("True Energy [MeV]");
    hResponseMatrix->SetYTitle("Energy Loss [MeV]");
    hResponseMatrix->Draw("COLZ");
    
    //    alcaphistogram(hResponseMatrix);
    TLatex* latex = new TLatex();
    std::string text = "AlCap #bf{#it{" + Particle + " Simulation}}";
    latex->DrawLatexNDC(0.5, 0.8, text.c_str());
    //    latex->DrawLatexNDC(0.5, 0.8, "AlCap #bf{#it{Simulation}}");
    //    alcapSimulation(hResponseMatrix);
    //    alcaplabel(particle, hResponseMatrix);
    hResponseMatrix->SetDrawOption("COLZ");
    
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_ResponseMatrix_ELoss_" + particle;
      
      std::string pngname = savename + ".png";
      c_Response->SaveAs(pngname.c_str());

      hResponseMatrix->GetYaxis()->SetRangeUser(0, zoomed_max_y[i_particle]);
      savename = savedir + "AlCapData_Si16bDataset_ResponseMatrix_ELoss_" + particle + "_Zoom";
      pngname = savename + ".png";
      c_Response->SaveAs(pngname.c_str());
    }
  }

  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
