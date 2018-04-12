void SiL3Spectra_Run9740() {

  std::string basename = "/gpfs/home/edmonds_a/data/out/";
  std::string Si16a_version = "v30"; // 10 keV bins = v30

  const int n_Si16a_runs = 1;
  const int first_Si16a_run = 9740;
  const int n_total_runs = n_Si16a_runs;

  std::string filenames[n_total_runs];
  std::stringstream temp_filename;
  for (int i_Si16a_run = 0; i_Si16a_run < n_Si16a_runs; ++i_Si16a_run) {
    int run_number = first_Si16a_run + i_Si16a_run;
    temp_filename.str("");
    temp_filename << basename << Si16a_version << "/out0" << run_number << ".root";
    filenames[i_Si16a_run] = temp_filename.str();
  }
  std::string labels[n_total_runs] = {"Run 9740 (SF=1.02)"};
  double n_midas_blocks[n_total_runs] = {2907};
  TH1F* hists[n_total_runs] = {0};
  TH1F* ratio_hists[n_total_runs] = {0};
  Int_t colours[n_total_runs] = {kBlue};

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

    //    hists[i_run]->GetXaxis()->SetRangeUser(200,2000);
    hists[i_run]->Draw("HIST E SAME");
    leg->AddEntry(hists[i_run], labels[i_run].c_str(), "l");
    file->Close();
  }
  //  c1->SetLogy();
  leg->Draw();
}
