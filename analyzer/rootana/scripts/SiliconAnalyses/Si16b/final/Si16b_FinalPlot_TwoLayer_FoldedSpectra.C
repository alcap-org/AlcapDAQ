void Si16b_FinalPlot_TwoLayer_FoldedSpectra() {

  std::string infilename = "~/data/results/Si16b/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TCanvas* c_energy = new TCanvas("c_energy", "c_energy");

  TLegend* leg = new TLegend(0.50,0.45,0.90,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  int rebin_factor = 10;
  std::stringstream axistitle;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];

    std::string dirname = this_particle + "_SiR_timecut";
    std::string histname = dirname + "/hEvdE_TwoLayer_12not3";
    std::string newhistname = this_particle;
    
    TH2F* hEvdE = (TH2F*) infile->Get(histname.c_str());
    TH1F* hEnergy = (TH1F*) hEvdE->ProjectionX(newhistname.c_str());
    std::string histtitle = "Si16b Dataset, Individual Particle Analyses, Folded Spectra";
    hEnergy->SetTitle(histtitle.c_str());
    hEnergy->SetStats(false);
    hEnergy->SetLineColor(colours[i_particle]);
    hEnergy->SetMarkerColor(colours[i_particle]);
    hEnergy->Rebin(rebin_factor);

    axistitle.str("");
    axistitle << "Count / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
    hEnergy->SetYTitle(axistitle.str().c_str());
    //    hEnergy->GetXaxis()->SetRangeUser(0, 10000);
    hEnergy->Draw("HIST E SAME");

    leglabel.str("");
    leglabel << this_particle << " (" << hEnergy->Integral() << " entries)";
    leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");
  }
  leg->Draw();
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.35, 0.80, "AlCap Preliminary");

}
