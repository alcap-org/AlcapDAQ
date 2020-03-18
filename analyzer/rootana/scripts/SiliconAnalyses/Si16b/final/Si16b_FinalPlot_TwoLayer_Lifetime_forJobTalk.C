void Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk(std::string savedir = "") {

  std::string infilename = "~/data/results/Si16b/lifetime_newPP_geq1TgtPulse_3sigma.root";
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
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];

    std::string dirname = this_particle + "_SingleExp";
    //    std::string dirname = this_particle + "_SingleExp_FlatBkg";
    //    std::string dirname = this_particle + "_DoubleExp_FlatBkg";
    std::string histname = dirname + "/hTime";
    std::string fitname = dirname + "/muonic_atom_lifetime";

    TH1F* hTime = (TH1F*) infile->Get(histname.c_str());
    if (!hTime) {
      std::cout << "Can't find histogram " << histname << std::endl;
      continue;
    }
    std::string histtitle = "Si16b Dataset, Time";
    hTime->SetTitle(histtitle.c_str());
    hTime->SetStats(false);
    hTime->SetLineColor(colours[i_particle]);
    hTime->SetMarkerColor(colours[i_particle]);
    hTime->SetLineWidth(2);
    hTime->GetXaxis()->SetRangeUser(0, 10000);
    hTime->GetXaxis()->SetTitleOffset(1.0);
    hTime->GetYaxis()->SetTitleOffset(1.0);
    hTime->GetXaxis()->SetLabelSize(0.04);
    hTime->GetYaxis()->SetLabelSize(0.04);
    hTime->Draw("E1 SAME");

    TF1* fit = (TF1*) infile->Get(fitname.c_str());
    fit->SetLineColor(colours[i_particle]);
    fit->Draw("LSAME");

    leglabel.str("");
    leglabel << this_particle << std::fixed << std::setprecision(1) << " (#tau = " << fit->GetParameter(3) << " #pm " << fit->GetParError(3) << " ns)";
    leg->AddEntry(hTime, leglabel.str().c_str(), "l");

    std::cout << leglabel.str() << ", flat bkg = " << fit->GetParameter(4) << " #pm " << fit->GetParError(4) << std::endl;

    //    alcaphistogram(hTime);
    if (i_particle == 0) {
      //      alcapPreliminary(hTime);
      TLatex* latex = new TLatex();
      latex->DrawLatexNDC(0.25, 0.8, "AlCap #it{#bf{Preliminary}}");
    }
    else {
      hTime->SetDrawOption("E1 SAME");
    }
  }
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_TwoLayer_Lifetime_forJobTalk";

    std::string pdfname = savename + ".pdf";
    c_time->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_time->SaveAs(pngname.c_str());
  }
}
