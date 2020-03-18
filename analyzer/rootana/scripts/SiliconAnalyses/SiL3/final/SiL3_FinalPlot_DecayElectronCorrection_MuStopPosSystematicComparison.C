void SiL3_FinalPlot_DecayElectronCorrection_MuStopPosSystematicComparison(std::string savedir = "") {

  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_positions = 3;
  std::string positions[n_positions] = {"", "HigherE", "LowerE"};
  Int_t colours[n_positions] = {kBlack, kBlue, kRed};
  std::string leglabels[n_positions] = {"central value", "higherE", "lowerE"};

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  TLegend* leg = new TLegend(0.45,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::stringstream leglabel;
  for (int i_position = 0; i_position < n_positions; ++i_position) {
    std::string position = positions[i_position];
    Int_t i_colour = colours[i_position];

    std::string foldername = "DecayElectron_TimeSlice2000_4000_allRecoil_MuStopPos" + position;
    //    std::string histname = foldername + "/hCorrectedSpectrum";
    std::string histname = foldername + "/hCorrection";
    TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
      return;
    }

    int rebin_factor = 10;
    spectrum->Rebin(rebin_factor);
    spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, Decay Electron Correction");
    spectrum->SetStats(false);
    spectrum->GetXaxis()->SetRangeUser(0,30000);
    spectrum->GetXaxis()->SetTitle("Energy [keV]");
    spectrum->GetXaxis()->SetTitleOffset(0.9);
    spectrum->GetYaxis()->SetTitleOffset(0.9);    
    spectrum->SetLineColor(colours[i_position]);
    
    std::stringstream axislabel;
    axislabel << "Counts / " << spectrum->GetBinWidth(1) << " keV";
    spectrum->SetYTitle(axislabel.str().c_str());

    alcaphistogram(spectrum);
    spectrum->Draw("HIST E SAME");
    alcapPreliminary(spectrum);

    leglabel.str("");
    leglabel << leglabels[i_position];
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
  }

  leg->Draw();

   if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_DecayElectronCorrection_MuStopPosSystematicComparison";
    
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
