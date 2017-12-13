void SiL3Spectra() {

  std::string basename = "/gpfs/home/edmonds_a/data/out/";
  std::string Si16b_version = "v31"; // 10keV bins = v31
  std::string Si16a_version = "v30"; // 10 keV bins = v30

  const int n_Si16a_runs = 8;
  //  const int n_Si16a_runs = 3;
  const int first_Si16a_run = 9736;
  //  const int first_Si16a_run = 9739;
  const int n_Si16b_runs = 1;
  const int n_total_runs = n_Si16a_runs + n_Si16b_runs;

  std::string filenames[n_total_runs];
  filenames[0] = basename+Si16b_version+"/Si16b.root";
  std::stringstream temp_filename;
  for (int i_Si16a_run = 0; i_Si16a_run < n_Si16a_runs; ++i_Si16a_run) {
    int run_number = first_Si16a_run + i_Si16a_run;
    temp_filename.str("");
    temp_filename << basename << Si16a_version << "/out0" << run_number << ".root";
    filenames[i_Si16a_run+1] = temp_filename.str();
  }
  std::string labels[n_total_runs] = {"Si16b (SF=1.02)", "Run 9736 (SF=0.99)", "Run 9737 (SF=0.98)", "Run 9738 (SF=1.00)", 
				      "Run 9739 (SF=1.01)", "Run 9740 (SF=1.02)", "Run 9741 (SF=1.03)", "Run 9742 (SF=1.04)", "Run 9743 (SF=1.05)"
  };
  double n_midas_blocks[n_total_runs] = {312032, 3755, 3429, 3509, 
					 3704, 2907, 2845, 3025, 2993,
  };
  TH1F* hists[n_total_runs] = {0};
  TH1F* ratio_hists[n_total_runs] = {0};
  Int_t colours[n_total_runs] = {kBlack, kGray+2, kSpring-1, kMagenta, 
				 kRed, kBlue, kCyan+2, kOrange-3, kRed-9
  };

  TLegend* leg = new TLegend(0.64,0.54,0.79,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  TCanvas* c1 = new TCanvas("c1", "c1");

  std::stringstream histname;
  for (int i_run = 0; i_run < n_total_runs; ++i_run) {
    TFile* file = new TFile(filenames[i_run].c_str(), "READ");
    if (file->IsZombie()) {
      std::cout << "Problem opening file " << filenames[i_run] << std::endl;
      continue;
    }
    std::cout << filenames[i_run] << std::endl;

    histname.str("");
    histname << "PlotTAP_Energy/hSiL3-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Energy;1";

    TH1F* hist = (TH1F*) file->Get(histname.str().c_str());
    if (!hist) {
      std::cout << "Problem finding histogram " << histname.str() << std::endl;
      continue;
    }
    hist->SetDirectory(0);
    hist->Sumw2();
    
    if (!hists[i_run]) {
      std::stringstream newhistname;
      newhistname << "hSiL1Spectrum_" << i_run;
      hists[i_run] = (TH1F*) hist->Clone(newhistname.str().c_str());
      hists[i_run]->SetDirectory(0);
    }
    else {
      hists[i_run]->Add(hist);
    }

    hists[i_run]->SetLineWidth(1);
    hists[i_run]->SetLineColor(colours[i_run]);
    hists[i_run]->SetMarkerStyle(kFullDotMedium);
    hists[i_run]->SetMarkerColor(colours[i_run]);
    

    // Normalise to integral
    //    double integral = hists[i_run]->Integral();
    //    hists[i_run]->Scale(1.0/integral);

    // Normalise to number of MIDAS blocks
    //    std::cout << n_midas_blocks[i_run] << std::endl;
    //    hists[i_run]->Scale(1.0/n_midas_blocks[i_run]);

    hists[i_run]->Rebin(5);

    // Normalise to the bin at 1000 keV to check the shape only
    if (i_run > 0) {
      double energy_to_find = 400;
      int bin = hists[i_run]->FindBin(energy_to_find);
      double this_content = hists[i_run]->GetBinContent(bin);

      double si16b_content = hists[0]->GetBinContent(bin);

      double scale_factor = si16b_content / this_content;
      std::cout << si16b_content << ", " << this_content << ", " << scale_factor << std::endl;
      hists[i_run]->Scale(scale_factor);
    }

    hists[i_run]->GetXaxis()->SetRangeUser(200,2000);
    hists[i_run]->Draw("HIST E SAME");
    leg->AddEntry(hists[i_run], labels[i_run].c_str(), "l");
    file->Close();
  }
  c1->SetLogy();
  leg->Draw();

  TCanvas* c2 = new TCanvas("c2", "c2");
  for (int i_run = 0; i_run < n_total_runs; ++i_run) {
    if (!hists[i_run]) {
      continue;
    }
    std::string newhistname2 = "hRatio_";
    newhistname2 += hists[i_run]->GetName();
    std::cout << newhistname2 << std::endl;

    ratio_hists[i_run] = (TH1F*) hists[i_run]->Clone(newhistname2.c_str());
    ratio_hists[i_run]->SetYTitle("Ratio to Full Si16b Spectrum");

    ratio_hists[i_run]->Divide(hists[0]); // divide by the full Si16b run
    ratio_hists[i_run]->SetMinimum(0);
    ratio_hists[i_run]->SetMaximum(2);
    ratio_hists[i_run]->Draw("HIST E SAME");
  }
  leg->Draw();
}
