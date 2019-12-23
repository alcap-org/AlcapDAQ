#include "../../../XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_FoldedSpectra_TgtPulseComparison() {

  TCanvas* c_FoldedSpectrum = new TCanvas("c_FoldedSpectrum", "c_FoldedSpectrum");
  c_FoldedSpectrum->SetLogy();
  
  //  const int n_spectra = 5;
  //  std::string settings[n_spectra] = {"", "_0TgtPulse", "_1TgtPulse", "_2TgtPulse", "_3TgtPulse"};
  //  std::string leglabels[n_spectra] = {"any number SiL3 (Target) pulses", "0 SiL3 (Target) pulses", "1 SiL3 (Target) pulses", "2 SiL3 (Target) pulses", "3 SiL3 (Target) pulses"};
  //  Int_t colours[n_spectra] = {kBlue, kRed, kBlack, kMagenta, kSpring};
  //  const int n_spectra = 3;
  //  std::string settings[n_spectra] = {"", "_geq2TgtPulse", "_geq2TgtPulse_wMuonDefn"};
  //  std::string leglabels[n_spectra] = {"any number SiL3 (Target) pulses", "#geq 2 SiL3 (Target) pulses", "#geq 2 SiL3 (Target) pulses and Muon Hit"};
  //  Int_t colours[n_spectra] = {kBlue, kRed, kBlack};
  //  const int n_spectra = 2;
  //  std::string settings[n_spectra] = {"_09040", "_geq2TgtPulse_09040"};
  //  std::string leglabels[n_spectra] = {"run 9040, any number SiL3 (Target) pulses", "run 9040, #geq 2 SiL3 (Target) pulses"};
  //  Int_t colours[n_spectra] = {kBlue, kRed};
  const int n_spectra = 5;
  std::string settings[n_spectra] = {"_geq2TgtPulse_newPP20us", "_0TgtPulse", "_1TgtPulse", "_2TgtPulse", "_3TgtPulse"};
  std::string leglabels[n_spectra] = {"#geq 2 SiL3 (Target) pulses", "0 SiL3 (Target) pulses", "1 SiL3 (Target) pulses", "2 SiL3 (Target) pulses", "3 SiL3 (Target) pulses"};
  Int_t colours[n_spectra] = {kBlue, kRed, kBlack, kMagenta, kSpring};

  TLegend* leg = new TLegend(0.40,0.55,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  for (int i_spectra = 0; i_spectra < n_spectra; ++i_spectra) {
    std::string i_setting = settings[i_spectra];
    std::string plots_file_name = "~/data/results/SiL3/raw_spectra" + i_setting + ".root";
    std::string full_spectrum_name = "RawSpectrum_fromEnergyTime_TimeSlice2000_4000/hRawSpectrum";

    TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
    //    std::string newname = i_setting;
    TH1F* hFoldedSpectrum = (TH1F*) plots_file->Get(full_spectrum_name.c_str());
    if (!hFoldedSpectrum) {
      hFoldedSpectrum = (TH1F*) plots_file->Get("SiL3_ActiveTarget_TimeSlice2000_4000/hRawSpectrum");
    }

    hFoldedSpectrum->Rebin(5);
    //  hFoldedSpectrum->SetLineColor(kBlue);
    std::string histtitle = "SiL3 Dataset, Target Spectrum (2000 ns < t < 4000 ns)";
    hFoldedSpectrum->SetTitle(histtitle.c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 30000);
    hFoldedSpectrum->SetLineColor(colours[i_spectra]);

    std::stringstream axislabel;
    axislabel << "Counts / " << hFoldedSpectrum->GetBinWidth(1) << " keV";
    std::cout << "AE: " << axislabel.str() << std::endl;
    hFoldedSpectrum->SetYTitle(axislabel.str().c_str());
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_spectra];
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");

    if (i_spectra == 0) {
      hFoldedSpectrum->Draw("HIST E");
    }
    else {
      hFoldedSpectrum->Draw("HIST E SAME");
    }
  }
  
  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.50, 0.90, "AlCap Preliminary");

  leg->Draw();
}
