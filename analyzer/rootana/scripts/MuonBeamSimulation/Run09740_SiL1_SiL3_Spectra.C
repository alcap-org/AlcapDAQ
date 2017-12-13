void Run09740_SiL1_SiL3_Spectra() {

  std::string basename = "/gpfs/home/edmonds_a/data/out/";
  std::string Si16a_version = "v30"; // 10 keV bins = v30

  const int n_Si16a_runs = 1;
  const int first_Si16a_run = 9743;

  std::string filenames[n_Si16a_runs];
  std::stringstream temp_filename;
  for (int i_Si16a_run = 0; i_Si16a_run < n_Si16a_runs; ++i_Si16a_run) {
    int run_number = first_Si16a_run + i_Si16a_run;
    temp_filename.str("");
    temp_filename << basename << Si16a_version << "/out0" << run_number << ".root";
    filenames[i_Si16a_run] = temp_filename.str();
  }

  TH1F* SiL1_hists[n_Si16a_runs] = {0};
  TH1F* SiL3_hists[n_Si16a_runs] = {0};
  Int_t colours[n_Si16a_runs] = {kBlack};

  TLegend* leg = new TLegend(0.64,0.54,0.79,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  int n_SiL1_channels = 16; // will ignore the first and last strips later

  TCanvas* c1 = new TCanvas("c1", "c1");

  std::stringstream histname;
  for (int i_run = 0; i_run < n_Si16a_runs; ++i_run) {
    TFile* file = new TFile(filenames[i_run].c_str(), "READ");
    if (file->IsZombie()) {
      std::cout << "Problem opening file " << filenames[i_run] << std::endl;
      continue;
    }
    std::cout << filenames[i_run] << std::endl;

    for (int i_channel = 1; i_channel < n_SiL1_channels-1; ++i_channel) {
      histname.str("");
      histname << "PlotTAP_Energy/hSiL1-" << i_channel+1 << "-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Energy;1";

      TH1F* hist = (TH1F*) file->Get(histname.str().c_str());
      if (!hist) {
	std::cout << "Problem finding histogram " << histname.str() << std::endl;
	continue;
      }
      hist->SetDirectory(0);
      hist->Sumw2();

      if (!SiL1_hists[i_run]) {
	std::stringstream newhistname;
	newhistname << "hSiL1Spectrum_" << i_run;
	SiL1_hists[i_run] = (TH1F*) hist->Clone(newhistname.str().c_str());
	SiL1_hists[i_run]->SetDirectory(0);
      }
      else {
	SiL1_hists[i_run]->Add(hist);
      }
    }
    SiL1_hists[i_run]->SetLineWidth(1);
    SiL1_hists[i_run]->SetLineColor(colours[i_run]);
    SiL1_hists[i_run]->SetMarkerStyle(kFullDotMedium);
    SiL1_hists[i_run]->SetMarkerColor(colours[i_run]);
    SiL1_hists[i_run]->SetTitle("Run 9743 (SF=1.05)");


    SiL1_hists[i_run]->Rebin(5);

    SiL1_hists[i_run]->GetXaxis()->SetRangeUser(200,2000);
    SiL1_hists[i_run]->Draw("HIST E");
    leg->AddEntry(SiL1_hists[i_run], "SiL1", "l");


    histname.str("");
    histname << "PlotTAP_Energy/hSiL3-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Energy;1";

    TH1F* hist = (TH1F*) file->Get(histname.str().c_str());
    if (!hist) {
      std::cout << "Problem finding histogram " << histname.str() << std::endl;
      continue;
    }
    hist->SetDirectory(0);
    hist->Sumw2();
    
    if (!SiL3_hists[i_run]) {
      std::stringstream newhistname;
      newhistname << "hSiL3Spectrum_" << i_run;
      SiL3_hists[i_run] = (TH1F*) hist->Clone(newhistname.str().c_str());
      SiL3_hists[i_run]->SetDirectory(0);
    }
    SiL3_hists[i_run]->SetLineWidth(1);
    SiL3_hists[i_run]->SetLineColor(kRed);
    SiL3_hists[i_run]->SetMarkerStyle(kFullDotMedium);
    SiL3_hists[i_run]->SetMarkerColor(kRed);
    SiL3_hists[i_run]->SetTitle("Run 9743 (SF=1.05)");
    SiL3_hists[i_run]->Rebin(5);
    SiL3_hists[i_run]->GetXaxis()->SetRangeUser(200,2000);
    SiL3_hists[i_run]->Draw("HIST E SAMES");

    leg->AddEntry(SiL3_hists[i_run], "SiL3", "l");

    file->Close();
  }
  c1->SetLogy();
  leg->Draw();
}
