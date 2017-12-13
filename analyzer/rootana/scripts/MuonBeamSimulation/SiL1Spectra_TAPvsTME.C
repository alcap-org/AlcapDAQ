void SiL1Spectra_TAPvsTME() {

  std::string basename = "/gpfs/home/edmonds_a/data/out/";
  std::string Si16a_version = "v30"; // 10 keV bins = v30

  const int n_Si16a_runs = 1;
  const int first_Si16a_run = 9740;
  const int n_total_runs = 2*n_Si16a_runs;

  std::string filenames[n_total_runs];
  std::stringstream temp_filename;
  for (int i_Si16a_run = 0; i_Si16a_run < n_Si16a_runs; ++i_Si16a_run) {
    int run_number = first_Si16a_run + i_Si16a_run;
    temp_filename.str("");
    temp_filename << basename << Si16a_version << "/out0" << run_number << ".root";
    filenames[2*i_Si16a_run] = temp_filename.str();

    temp_filename.str("");
    temp_filename << "out0" << run_number << "_tme-stopped-muon-discrepancy.root";
    filenames[2*i_Si16a_run+1] = temp_filename.str();
  }
  std::string labels[n_total_runs] = {"Run 9740 (TAPs)", "Run 9740 (TMEs)"//, "Run 9742 (SF=1.04)", "Run 9743 (SF=1.05)"
  };

  TH1F* hists[n_total_runs] = {0};
  Int_t colours[n_total_runs] = {kBlack, kRed};

  TLegend* leg = new TLegend(0.64,0.54,0.79,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  int n_SiL1_channels = 16; // will ignore the first and last strips later

  TCanvas* c1 = new TCanvas("c1", "c1");

  std::stringstream histname;
  for (int i_run = 0; i_run < n_total_runs; ++i_run) {
    TFile* file = new TFile(filenames[i_run].c_str(), "READ");
    if (file->IsZombie()) {
      std::cout << "Problem opening file " << filenames[i_run] << std::endl;
      continue;
    }
    std::cout << filenames[i_run] << std::endl;

    if (i_run%2 == 0) {
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
	
	if (!hists[i_run]) {
	  std::stringstream newhistname;
	  newhistname << "hSiL1Spectrum_TAP_" << i_run;
	  hists[i_run] = (TH1F*) hist->Clone(newhistname.str().c_str());
	  hists[i_run]->SetDirectory(0);
	}
	else {
	  hists[i_run]->Add(hist);
	}
      }
      hists[i_run]->Rebin(5);
    }
    else {
      TNtuple* simple_event = (TNtuple*) file->Get("PlotTME_StoppedMuonDiscrepancy/fSimpleEvent");
      if (!simple_event) {
	std::cout << "Problem finding ntuple" << std::endl;
	continue;
      }
      std::stringstream newhistname;
      newhistname << "hSiL1Spectrum_TME_" << i_run;
      if (!hists[i_run-1]) {
	std::cout << "Problem finding previous histogram" << std::endl;
	continue;
      }
      hists[i_run] = (TH1F*) hists[i_run-1]->Clone(newhistname.str().c_str());
      hists[i_run]->Reset();

      std::string drawcmd = "SiL1Energy>>" + newhistname.str();
      std::cout << simple_event->GetEntries() << std::endl;
      std::cout << drawcmd << std::endl;
      std::cout << simple_event->Draw(drawcmd.c_str(), "SiL1Time>200 && SiL1Time>200", "goff") << std::endl;
      std::cout << hists[i_run]->GetEntries() << std::endl;
      hists[i_run]->SetDirectory(0);
    }

    // Normalise to integral
    //    double integral = hists[i_run]->Integral();
    //    hists[i_run]->Scale(1.0/integral);

    // Normalise to number of MIDAS blocks
    //    std::cout << n_midas_blocks[i_run] << std::endl;
    //    hists[i_run]->Scale(1.0/n_midas_blocks[i_run]);

    hists[i_run]->SetLineWidth(1);
    hists[i_run]->SetLineColor(colours[i_run]);
    hists[i_run]->SetMarkerStyle(kFullDotMedium);
    hists[i_run]->SetMarkerColor(colours[i_run]);

    // Normalise to the bin at 1000 keV to check the shape only
    /*    if (i_run > 0) {
      double energy_to_find = 1000;
      int bin = hists[i_run]->FindBin(energy_to_find);
      double this_content = hists[i_run]->GetBinContent(bin);

      double si16b_content = hists[0]->GetBinContent(bin);

      double scale_factor = si16b_content / this_content;
      std::cout << si16b_content << ", " << this_content << ", " << scale_factor << std::endl;
      hists[i_run]->Scale(scale_factor);
    }
    */

    //    hists[i_run]->GetXaxis()->SetRangeUser(200,2000);
    hists[i_run]->Draw("HIST E SAMES");
    leg->AddEntry(hists[i_run], labels[i_run].c_str(), "l");
    file->Close();
  }
  //  c1->SetLogy();
  leg->Draw();
}
