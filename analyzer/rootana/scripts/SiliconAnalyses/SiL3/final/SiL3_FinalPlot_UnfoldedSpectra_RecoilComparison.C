#include "../../../XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_UnfoldedSpectra_RecoilComparison() {

  TCanvas* c_FoldedSpectrum = new TCanvas("c_FoldedSpectrum", "c_FoldedSpectrum");
  c_FoldedSpectrum->SetLogy();
  
  const int n_spectra = 6;
  std::string settings[n_spectra] = {"noRecoil", "pRecoil", "dRecoil", "tRecoil", "aRecoil", "allRecoil"};
  std::string leglabels[n_spectra] = {"no recoil", "p recoil (3.7%)", "d recoil (7.7%)", "t recoil (12%)", "#alpha recoil (17%)", "combined recoil (7.8%)"};
  Int_t colours[n_spectra] = {kBlack, kRed, kCyan, kMagenta, kSpring, kBlue};

  TLegend* leg = new TLegend(0.40,0.55,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  std::string plots_file_name = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test_KFactor0-85.root";
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  for (int i_spectra = 0; i_spectra < n_spectra; ++i_spectra) {
    std::string i_setting = settings[i_spectra];
    std::string full_spectrum_name = "FinalNormalisation_TimeSlice3000_4000_" + i_setting + "_500keVBins/hNormalisedSpectrum";

    //    std::string newname = i_setting;
    TH1F* hUnfoldedSpectrum = (TH1F*) plots_file->Get(full_spectrum_name.c_str());
    if (!hUnfoldedSpectrum) {
      std::cout << "Can't find histogram with name \"" << full_spectrum_name << "\"" << std::endl;
      return;
    }

    int rebin_factor = 1;
    hUnfoldedSpectrum->Rebin(rebin_factor);
    hUnfoldedSpectrum->Scale(1.0 / rebin_factor);
    //  hUnfoldedSpectrum->SetLineColor(kBlue);
    std::string histtitle = "SiL3 Dataset, Normalised Spectrum (3000 ns < t < 4000 ns), Recoil Effects";
    hUnfoldedSpectrum->SetTitle(histtitle.c_str());
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 30000);
    hUnfoldedSpectrum->SetLineColor(colours[i_spectra]);

    std::stringstream axislabel;
    axislabel << "per captured muon / keV";
    std::cout << "AE: " << axislabel.str() << std::endl;
    hUnfoldedSpectrum->SetYTitle(axislabel.str().c_str());
    hUnfoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_spectra];
    leg->AddEntry(hUnfoldedSpectrum, leglabel.str().c_str(), "l");

    if (i_spectra == 0) {
      hUnfoldedSpectrum->Draw("HIST E");
    }
    else {
      hUnfoldedSpectrum->Draw("HIST E SAME");
    }
    std::cout << hUnfoldedSpectrum->GetBinContent(hUnfoldedSpectrum->FindBin(15)) << std::endl;
  }
  
  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.50, 0.90, "AlCap Preliminary");

  leg->Draw();
}
