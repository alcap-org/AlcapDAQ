void Si16b_FinalPlot_TwoLayer_Lifetime() {

  std::string infilename = "~/data/results/Si16b/lifetime_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TCanvas* c_time = new TCanvas("c_time", "c_time");
  c_time->SetLogy(1);

  TLegend* leg = new TLegend(0.50,0.45,0.90,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  
  const int n_particles = 4;
  std::string particles[n_particles] = { "Proton", "Deuteron", "Triton", "Alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];

    std::string dirname = this_particle + "_wTimeCut_SingleExp";
    //    std::string dirname = this_particle + "_wTimeCut_DoubleExp";
    std::string histname = dirname + "/hTime";
    std::string fitname = dirname + "/muonic_atom_lifetime";

    TH1F* hTime = (TH1F*) infile->Get(histname.c_str());
    if (!hTime) {
      continue;
    }
    std::string histtitle = "Si16b Dataset, Time";
    hTime->SetTitle(histtitle.c_str());
    hTime->SetStats(false);
    hTime->SetLineColor(colours[i_particle]);
    hTime->SetMarkerColor(colours[i_particle]);
    hTime->GetXaxis()->SetRangeUser(0, 10000);
    hTime->Draw("P E SAME");

    TF1* fit = (TF1*) infile->Get(fitname.c_str());
    fit->SetLineColor(colours[i_particle]);
    fit->Draw("LSAME");

    leglabel.str("");
    leglabel << this_particle << std::fixed << std::setprecision(1) << " (#tau = " << fit->GetParameter(3) << " #pm " << fit->GetParError(3) << ")";
    leg->AddEntry(hTime, leglabel.str().c_str(), "l");
  }
  leg->Draw();
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.35, 0.80, "AlCap Preliminary");

}
