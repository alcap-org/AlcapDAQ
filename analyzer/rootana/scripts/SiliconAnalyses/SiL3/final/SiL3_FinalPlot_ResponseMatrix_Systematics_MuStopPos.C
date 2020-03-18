void SiL3_FinalPlot_ResponseMatrix_Systematics_MuStopPos(std::string savedir = "") {

  
  std::string infilename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_particles = 2;
  std::string particles[n_particles] = {"MuStopPosHigherE", "MuStopPosLowerE"};
  std::string labels[n_particles] = {"higher energy muon beam", "lower energy muon beam"};

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string inhistname = "ResponseMatrix_TimeSlice2000_4000_allRecoil_" + particle + "/hResponseMatrix";
  
    TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());
    if (!hResponseMatrix) {
      std::cout << "Can't find histogram " << inhistname << std::endl;
    }

    TCanvas* c_Response = new TCanvas();
    c_Response->SetLogz();

    std::string histtitle;
    histtitle = "SiL3 MC, " + particle + " Escape Response Matrix";
    hResponseMatrix->SetTitle(histtitle.c_str());
    hResponseMatrix->SetStats(false);
    //  gStyle->SetOptFit(1);
    hResponseMatrix->SetXTitle("Observed Energy [keV]");
    hResponseMatrix->SetYTitle("True Energy [keV]");
    hResponseMatrix->Draw("COLZ");

    alcaphistogram(hResponseMatrix);
    //    alcapSimulation(hResponseMatrix);
    //    alcaplabel(particle, hResponseMatrix);
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.7, 0.5, "AlCap #bf{#it{Simulation}}");
    latex->DrawLatexNDC(0.7, 0.45, labels[i_particle].c_str());
    hResponseMatrix->SetDrawOption("COLZ");

    if (savedir != "") {
      std::string savename = savedir + "AlCapData_SiL3Dataset_ResponseMatrix_" + particle;
      
      std::string pngname = savename + ".png";
      c_Response->SaveAs(pngname.c_str());
    }
    
    //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");

    TCanvas* c_MeasAndTrue = new TCanvas();
    TH1D* hTrue = hResponseMatrix->ProjectionY("hTrue");
    hTrue->SetLineColor(kMagenta);
    hTrue->SetLineWidth(2);
    hTrue->SetStats(false);
    TH1D* hMeas = hResponseMatrix->ProjectionX("hMeas");
    hMeas->SetLineColor(kCyan);
    hMeas->SetLineWidth(2);
    hMeas->SetStats(false);
    
    int rebin_factor = 5;
    hMeas->Rebin(rebin_factor);
    hTrue->Rebin(rebin_factor);
    
    hMeas->Draw("HIST");
    hTrue->Draw("HIST SAME");

    TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);
    leg->AddEntry(hMeas, "MC Meas", "l");
    leg->AddEntry(hTrue, "MC Truth", "l");
    leg->Draw();
  }
}
