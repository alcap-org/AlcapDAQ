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

  for (int i_arm = 1; i_arm >= 0; --i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response;2";
    RooUnfoldBayes* unfold = (RooUnfoldBayes*) file->Get(response_name.c_str());

    TH1F* unfolded_spectrum = unfold->Hreco(RooUnfold::ErrorTreatment::kErrors);
    unfolded_spectrum->SetStats(false);
    unfolded_spectrum->SetLineWidth(2);
    unfolded_spectrum->SetLineColor(colours[i_arm]);
    //    std::string histtitle = "Unfolded Proton Energy Spectra";
    std::string histtitle = "";
    unfolded_spectrum->SetTitle(histtitle.c_str());
    unfolded_spectrum->SetXTitle("E_{1} + E_{2} [keV]");
    unfolded_spectrum->SetYTitle("Counts");
    unfolded_spectrum->GetYaxis()->SetTitleOffset(1.3);
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

    low_bin = unfolded_spectrum->FindBin(3500);
    high_bin = unfolded_spectrum->FindBin(10000);
    error = 0;
    std::cout << arm_names[i_arm] << ": Integral(3.5 - 10 MeV) = " << unfolded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;

    low_bin = unfolded_spectrum->FindBin(2500);
    high_bin = unfolded_spectrum->FindBin(3500);
    error = 0;
    std::cout << arm_names[i_arm] << ": Integral(2.5 - 3.5 MeV) = " << unfolded_spectrum->IntegralAndError(low_bin, high_bin, error) << " +- " << error << std::endl;

    //    unfolded_spectrum->SetMaximum(1100000);
  }

  leg->Draw();

  TLine* low_energy_cut = new TLine(3500, 0,3500, 520000);
  low_energy_cut->SetLineWidth(2);
  low_energy_cut->SetLineStyle(2);
  low_energy_cut->SetLineColor(kBlack);
  //  low_energy_cut->Draw("LSAME");
  TLine* high_energy_cut = new TLine(10000, 0,10000, 520000);
  high_energy_cut->SetLineWidth(2);
  high_energy_cut->SetLineStyle(2);
  high_energy_cut->SetLineColor(kBlack);
  //  high_energy_cut->Draw("LSAME");


  std::string plotname = "~/data/out/v92/plots/unfolded-energy-spectra";
  std::string pdfname = plotname+".png";
  //  c1->SaveAs(pdfname.c_str());
}
