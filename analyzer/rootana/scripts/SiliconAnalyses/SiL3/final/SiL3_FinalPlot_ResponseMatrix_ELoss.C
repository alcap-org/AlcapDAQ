void SiL3_FinalPlot_ResponseMatrix_ELoss(std::string savedir = "") {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  int n_sim_particles[n_particles] = {1, 1, 1, 1};

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string n_M = std::to_string(n_sim_particles[i_particle]);
    std::string infilename = "~/data/mc/SiL3/" + particle + "Corr_" + n_M + "M_Geom-P1_" + particle + "-flat_Thresh0-1MeV.root";
    std::string inhistname = "SiL_hELoss";
  
    TFile* infile = new TFile(infilename.c_str(), "READ");
    TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

    TCanvas* c_Response = new TCanvas();
    
    std::string histtitle;
    histtitle = "SiL3 MC, " + particle + " Energy Loss";
    hResponseMatrix->SetTitle(histtitle.c_str());
    hResponseMatrix->SetStats(false);
    //  gStyle->SetOptFit(1);
    hResponseMatrix->GetYaxis()->SetTitleOffset(1.3);
    hResponseMatrix->Draw("COLZ");

    alcaphistogram(hResponseMatrix);
    alcapSimulation(hResponseMatrix);
    alcaplabel(particle, hResponseMatrix);
    hResponseMatrix->SetDrawOption("COLZ");
    
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_SiL3Dataset_ResponseMatrix_ELoss_" + particle;
      
      std::string pngname = savename + ".png";
      c_Response->SaveAs(pngname.c_str());
    }
  }

  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
