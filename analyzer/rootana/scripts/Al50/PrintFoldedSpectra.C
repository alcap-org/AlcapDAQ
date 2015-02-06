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

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response;2";
    RooUnfoldBayes* unfold = (RooUnfoldBayes*) file->Get(response_name.c_str());

    TH1F* folded_spectrum = (TH1F*) unfold->Hmeasured();
    folded_spectrum->SetStats(false);
    folded_spectrum->SetLineWidth(2);
    folded_spectrum->SetLineColor(colours[i_arm]);
    std::string histtitle = "Measured Proton Energy Spectra";
    folded_spectrum->SetTitle(histtitle.c_str());
    folded_spectrum->SetXTitle("Energy [keV]");
    folded_spectrum->GetXaxis()->SetRangeUser(0, 20000);
    folded_spectrum->Draw("HIST E SAME");

    leg->AddEntry(folded_spectrum, arm_names[i_arm].c_str(), "l");
  }

  leg->Draw();

  std::string plotname = "~/plots/ThesisPlots/folded-energy-spectra";
  std::string pdfname = plotname+".pdf";
  //  c1->SaveAs(pdfname.c_str());
}
