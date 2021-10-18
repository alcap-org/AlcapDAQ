void SiL3_FinalPlot_ResponseMatrix(std::string savedir = "") {

  
  //  std::string infilename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test_KFactor0-85.root";

  const int n_particles = 5;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha", "combined"};

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    //    std::string inhistname = particle + "Escape_TimeSlice3000_4000_allRecoil/hResponseMatrix";
    std::string infilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV__MeV.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    RooUnfoldResponse* response = (RooUnfoldResponse*) infile->Get("SiL_middle_layer_response");
    //    std::string inhistname = "";
    TH2* hResponseMatrix = response->Hresponse();//(TH2F*) infile->Get(inhistname.c_str());

    TCanvas* c_Response = new TCanvas();
    c_Response->SetLogz();

    std::string histtitle;
    histtitle = "SiL3 MC, " + particle + " Escape Response Matrix";
    hResponseMatrix->SetTitle(histtitle.c_str());
    hResponseMatrix->SetStats(false);
    //  gStyle->SetOptFit(1);
    hResponseMatrix->SetXTitle("Observed Energy [MeV]");
    hResponseMatrix->SetYTitle("True Energy [MeV]");
    hResponseMatrix->Draw("COLZ");

    alcaphistogram(hResponseMatrix);
    alcapSimulation(hResponseMatrix);
    alcaplabel(particle, hResponseMatrix);
    hResponseMatrix->SetDrawOption("COLZ");

    if (savedir != "" && particle=="combined") {
      std::string savename = savedir + "AlCapMC_SiL3Dataset_EscapeCorrection_ResponseMatrix_" + particle;
      
      std::string pngname = savename + ".png";
      c_Response->SaveAs(pngname.c_str());
    }
    
    TLatex* latex = new TLatex();
    //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");

    // TCanvas* c_MeasAndTrue = new TCanvas();
    // TH1D* hTrue = hResponseMatrix->ProjectionY("hTrue");
    // hTrue->SetLineColor(kMagenta);
    // hTrue->SetLineWidth(2);
    // hTrue->SetStats(false);
    // TH1D* hMeas = hResponseMatrix->ProjectionX("hMeas");
    // hMeas->SetLineColor(kCyan);
    // hMeas->SetLineWidth(2);
    // hMeas->SetStats(false);
    
    // int rebin_factor = 5;
    // hMeas->Rebin(rebin_factor);
    // hTrue->Rebin(rebin_factor);
    
    // hMeas->Draw("HIST");
    // hTrue->Draw("HIST SAME");

    // TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
    // leg->SetBorderSize(0);
    // leg->SetTextSize(0.03);
    // leg->SetFillColor(kWhite);
    // leg->AddEntry(hMeas, "MC Meas", "l");
    // leg->AddEntry(hTrue, "MC Truth", "l");
    // leg->Draw();
  }
}
