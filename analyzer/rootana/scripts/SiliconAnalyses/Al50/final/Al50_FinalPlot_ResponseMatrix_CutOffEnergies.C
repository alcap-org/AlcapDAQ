void Al50_FinalPlot_ResponseMatrix_CutOffEnergies(std::string savedir = "") {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};

  std::string armname = "SiL";//"SiR";
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string infilename = "/data/R15bMC/respMatrix_100M_Al50_" + particle + "_100keVBins.root";

    std::string responsename = armname + "_";
    if (particle == "proton" && armname == "SiR") {
      responsename += "three";
    }
    else {
      responsename += "two";
    }
    responsename += "_layer_response";
  
    TFile* infile = new TFile(infilename.c_str(), "READ");
    RooUnfoldResponse* response = (RooUnfoldResponse*) infile->Get(responsename.c_str());

    TH1F* hTruth = (TH1F*) (response->Hresponse()->ProjectionY(particle.c_str()));
    TCanvas* c_Response = new TCanvas();
    //    hTruth->Rebin(10);
    hTruth->Draw("HIST E");

    // Start at the maximum bin to find where we should cut off the raw spectrum
    int start_bin = hTruth->GetMaximumBin();
    double start_bin_content = hTruth->GetBinContent(start_bin);
    double cut_at_fraction = 0.90;
    double cut_value = cut_at_fraction * start_bin_content;
    TLine* cut_line = new TLine(hTruth->GetXaxis()->GetXmin(), cut_value, hTruth->GetXaxis()->GetXmax(), cut_value);
    cut_line->SetLineColor(kRed);
    cut_line->Draw("LSAME");

    std::stringstream text;
    TLatex* latex = new TLatex();
    latex->SetTextColor(kRed);
    latex->SetTextSize(0.03);
    latex->SetTextAlign(13);
    text.str("");
    text << cut_at_fraction*100 << "% threshold";
    latex->DrawLatex(0.5, cut_value-50, text.str().c_str());
    
    
    double min_valid = hTruth->GetXaxis()->GetXmin();
    double max_valid = hTruth->GetXaxis()->GetXmax();
    for (int i_bin = start_bin; i_bin >= 1; --i_bin) {
      double this_bin_content = hTruth->GetBinContent(i_bin);
      double this_fraction = this_bin_content / start_bin_content;
      if (this_fraction < cut_at_fraction) {
	std::cout << particle << " start cut bin #" << i_bin << " [" << hTruth->GetXaxis()->GetBinLowEdge(i_bin) << " MeV, " << hTruth->GetXaxis()->GetBinUpEdge(i_bin) << " MeV] " << std::endl;
	min_valid = hTruth->GetXaxis()->GetBinUpEdge(i_bin);
	break;
      }
    }
    for (int i_bin = start_bin; i_bin < hTruth->GetNbinsX(); ++i_bin) {
      double this_bin_content = hTruth->GetBinContent(i_bin);
      double this_fraction = this_bin_content / start_bin_content;
      if (this_fraction < cut_at_fraction) {
	std::cout << particle << " end cut bin #" << i_bin << " [" << hTruth->GetXaxis()->GetBinLowEdge(i_bin) << " MeV, " << hTruth->GetXaxis()->GetBinUpEdge(i_bin) << " MeV] " << std::endl;
	max_valid = hTruth->GetXaxis()->GetBinLowEdge(i_bin);
	break;
      }
    }

    TArrow* valid_line = new TArrow(min_valid, cut_value, max_valid, cut_value, 0.5, "<>");
    valid_line->SetLineColor(kBlack);
    valid_line->SetLineWidth(2);
    valid_line->Draw("LSAME");

    text.str("");
    text << "Valid Range: " << min_valid << " MeV -- " << max_valid << " MeV";
    latex->SetTextAlign(23);
    latex->SetTextColor(kBlack);
    latex->DrawLatex(min_valid + (max_valid-min_valid)/2, cut_value-50, text.str().c_str());

    text.str("");
    text << "Al50 Dataset (" <<  particle << ", " << armname << ")";
    latex->SetTextSize(0.06);
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.5, 0.95, text.str().c_str());
  }

  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
