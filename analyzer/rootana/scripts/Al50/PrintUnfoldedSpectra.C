void PrintUnfoldedSpectra() {

  TFile* file = new TFile("unfolded.root", "READ");
  TCanvas* c1 = new TCanvas("c1", "c1");

  const int n_arms = 2;
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  Int_t colours[n_arms] = {kBlue, kRed};

  TLegend *leg = new TLegend(0.41,0.68,0.61,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response;2";
    RooUnfoldBayes* unfold = (RooUnfoldBayes*) file->Get(response_name.c_str());

    TH1F* unfolded_spectrum = unfold->Hreco(RooUnfold::ErrorTreatment::kErrors);
    unfolded_spectrum->SetStats(false);
    unfolded_spectrum->SetLineWidth(2);
    unfolded_spectrum->SetLineColor(colours[i_arm]);
    std::string histtitle = "Unfolded Proton Energy Spectra";
    unfolded_spectrum->SetTitle(histtitle.c_str());
    unfolded_spectrum->SetXTitle("Energy [keV]");
    unfolded_spectrum->GetXaxis()->SetRangeUser(0, 15000);
    unfolded_spectrum->Draw("HIST E SAME");

    leg->AddEntry(unfolded_spectrum, arm_names[i_arm].c_str(), "l");

    int low_bin = unfolded_spectrum->FindBin(0);
    int high_bin = unfolded_spectrum->FindBin(15000);
    double error = 0;
    std::cout << arm_names[i_arm] << ": Integral(0 - 15 MeV) = " << unfolded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;

    low_bin = unfolded_spectrum->FindBin(4000);
    high_bin = unfolded_spectrum->FindBin(8000);
    error = 0;
    std::cout << arm_names[i_arm] << ": Integral(4 - 8 MeV) = " << unfolded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;

  }

  leg->Draw();

  std::string plotname = "~/plots/ThesisPlots/unfolded-energy-spectra";
  std::string pdfname = plotname+".pdf";
  //  c1->SaveAs(pdfname.c_str());
}
