void Silicon_InclusiveChargedParticleSpectrum() {

  const int n_datasets = 2;
  std::string datasetnames[n_datasets] = {"Si16b_passive", "SiL3_active"};
  Int_t colours[n_datasets] = {kRed, kBlue};
  const int n_spectra = 3;
  std::string spectranames[n_spectra] = {"FlatBackground/hInputSpectrum", "FlatBackground/hCorrectedSpectrum", "DecayElectronCorrection/hCorrectedSpectrum"};
  TCanvas* canvases[n_spectra] = {0};

  TLegend* leg = new TLegend(0.30,0.55,0.70,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  double norm_point = 400; // normalise to this point in the spectrum
  double norm_factor = -1;
  for (int i_dataset = 0; i_dataset < n_datasets; ++i_dataset) {
    std::string i_datasetname = datasetnames[i_dataset];
    Int_t i_colour = colours[i_dataset];
    
    std::string filename = "~/data/results/" + i_datasetname + "/unfold.root";
    TFile* file = new TFile(filename.c_str(), "READ");

    for (int i_spectra = 0; i_spectra < n_spectra; ++i_spectra) {
      std::string i_spectraname = spectranames[i_spectra];
      TCanvas* i_canvas = canvases[i_spectra];

      if (!i_canvas) {
	std::stringstream canvasname;
	canvasname << "c" << i_spectra;
	i_canvas = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
	i_canvas->SetLogy();
	canvases[i_spectra] = i_canvas;
      }
      else {
	i_canvas->cd();
      }


      TH1F* spectrum = (TH1F*) file->Get(i_spectraname.c_str());
      if (!spectrum) {
	std::cout << "Error: Problem getting spectrum " << i_spectraname << std::endl;
	return;
      }
      //      spectrum->Sumw2();
      spectrum->SetLineColor(i_colour);

      if (i_spectra != 2) {
	double this_point = spectrum->GetBinContent(spectrum->FindBin(norm_point));
	if (norm_factor < 0) {
	  norm_factor = this_point;
	}
	else {
	  spectrum->Scale(norm_factor/this_point);
	}
      }
    
      spectrum->Draw("HIST E SAMES");

      if (i_spectra==0) {
	leg->AddEntry(spectrum, i_datasetname.c_str(), "l");
      }

      TLine* peak_line = new TLine(2500, spectrum->GetMinimum(), 2500, spectrum->GetMaximum());
      peak_line->SetLineColor(kBlack);
      peak_line->Draw("LSAME");

      TLine* trough_line = new TLine(1400, spectrum->GetMinimum(), 1400, spectrum->GetMaximum());
      trough_line->SetLineColor(kBlack);
      trough_line->Draw("LSAME");
    }
  }

  for (int i_spectra = 0; i_spectra < n_spectra; ++i_spectra) {
    TCanvas* i_canvas = canvases[i_spectra];
    i_canvas->cd();
    leg->Draw();
  }
}
