void Ti50_FinalPlot_TimeSpectrum_EnergyCuts() {

  std::string infilename = "~/data/results/Ti50/lifetime.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");


  double min_energy_cut = 0;
  double max_energy_cut = 15500;
  double energy_cut_step = 500;

  int n_bins = (max_energy_cut - min_energy_cut) / energy_cut_step;
  TH1F* hFitVsCut = new TH1F("hFitVsCut", "Fitted Lifetime (expo only) vs Energy Cut", n_bins,min_energy_cut,max_energy_cut);
  hFitVsCut->SetXTitle("Energy Cut [keV]");
  hFitVsCut->SetYTitle("Lifetime Fit [ns]");
    
  std::stringstream energy_cut_str;
  for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {

    energy_cut_str.str("");
    energy_cut_str << i_min_energy_cut;
    
    std::string base_indirname = "SiL3_" + energy_cut_str.str() + "keVCut";

    std::string canvasname = "c_Time_" + energy_cut_str.str() + "keVCut";
    //    TCanvas* c_Lifetime = new TCanvas(canvasname.c_str(), canvasname.c_str());
    //    c_Lifetime->SetLogy();
   
    std::string indirname = base_indirname;
    std::string inhistname = indirname + "/hTime";
    std::string infitname = indirname + "/muonic_atom_lifetime";
    
    TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
    hLifetime->SetMarkerStyle(kFullCircle);
    hLifetime->SetMarkerColor(kBlack);
    hLifetime->SetLineColor(kBlack);

    std::string histtitle = "Ti50 Dataset, SiL3, E > " + energy_cut_str.str() + "keV Cut";
    hLifetime->SetTitle(histtitle.c_str());
    hLifetime->SetStats(false);
    hLifetime->GetXaxis()->SetRangeUser(-1000, 20000);
    //    hLifetime->SetMinimum(0.5);
    //    hLifetime->SetMaximum(2e7);
    hLifetime->GetFunction("muonic_atom_lifetime")->SetBit(TF1::kNotDraw);
    //    hLifetime->Draw("P E");

    TF1* fit = (TF1*) infile->Get(infitname.c_str());
    //    fit->Draw("LSAME");

    TLegend* leg = new TLegend(0.5,0.65,0.9,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);
    
    std::stringstream leglabel;
    leglabel << std::setprecision(3) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(2) << fit->GetParError(1) << " ns";
    std::cout << "AE: " << leglabel.str() << std::endl;
    leg->AddEntry(fit, leglabel.str().c_str(), "l");
    
    //    leg->Draw();

    std::string pngname = "~/plots/2018-11-06/AlCapData_Ti50Dataset_Lifetime_" + energy_cut_str.str() + "keVCut.png";

    int i_bin = hFitVsCut->FindBin(i_min_energy_cut);
    hFitVsCut->SetBinContent(i_bin, fit->GetParameter(1));
    hFitVsCut->SetBinError(i_bin, fit->GetParError(1));
    //    c_Lifetime->SaveAs(pngname.c_str());
  }

  hFitVsCut->Draw("HIST E");

  double literature_value = 329.3;
  double literature_uncertainty = 1.3;

  TLine* literature_line = new TLine(min_energy_cut, literature_value, max_energy_cut, literature_value);
  literature_line->SetLineColor(kRed);
  literature_line->SetLineWidth(1);
  literature_line->Draw("LSAME");

  TBox* literature_uncertainty_box = new TBox(min_energy_cut, literature_value-literature_uncertainty, max_energy_cut, literature_value+literature_uncertainty);
  literature_uncertainty_box->SetLineColor(kRed);
  literature_uncertainty_box->SetLineWidth(1);
  literature_uncertainty_box->SetLineStyle(kDashed);
  literature_uncertainty_box->SetFillStyle(0);

  literature_uncertainty_box->Draw("LSAME");
}
