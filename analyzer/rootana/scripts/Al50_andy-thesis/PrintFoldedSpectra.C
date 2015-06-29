void PrintFoldedSpectra() {

  TFile* file = new TFile("unfolded.root", "READ");
  TCanvas* c1 = new TCanvas("c1", "c1");

  const int n_arms = 2;
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  Int_t colours[n_arms] = {kBlue, kRed};

  TLegend *leg = new TLegend(0.61,0.68,0.81,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);

  for (int i_arm = 1; i_arm >= 0; --i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response;2";
    RooUnfoldBayes* unfold = (RooUnfoldBayes*) file->Get(response_name.c_str());

    TH1F* folded_spectrum = (TH1F*) unfold->Hmeasured();
    folded_spectrum->SetStats(false);
    folded_spectrum->SetLineWidth(2);
    folded_spectrum->SetLineColor(colours[i_arm]);
    //    std::string histtitle = "Measured Proton Energy Spectra";
    std::string histtitle = "";
    folded_spectrum->SetTitle(histtitle.c_str());
    folded_spectrum->SetYTitle("Counts");
    folded_spectrum->GetYaxis()->SetTitleOffset(1.3);
    folded_spectrum->SetXTitle("E_{1} + E_{2} [keV]");
    folded_spectrum->GetXaxis()->SetRangeUser(0, 20000);
    folded_spectrum->Draw("HIST E SAME");

    leg->AddEntry(folded_spectrum, arm_names[i_arm].c_str(), "l");

    int low_bin = folded_spectrum->FindBin(0);
    int high_bin = folded_spectrum->FindBin(15000);
    double error = 0;
    std::cout << arm_names[i_arm] << ": Integral(0 - 15 MeV) = " << folded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;

    low_bin = folded_spectrum->FindBin(4000);
    high_bin = folded_spectrum->FindBin(8000);
    error = 0;
    std::cout << arm_names[i_arm] << ": Integral(4 - 8 MeV) = " << folded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;

    low_bin = folded_spectrum->FindBin(2500);
    high_bin = folded_spectrum->FindBin(10000);
    error = 0;
    std::cout << arm_names[i_arm] << ": Integral(2.5 - 10 MeV) = " << folded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;
  }

  leg->Draw();

  std::string plotname = "~/data/out/v94/plots/folded-energy-spectra";
  std::string pdfname = plotname+".png";
  //  c1->SaveAs(pdfname.c_str());
}
