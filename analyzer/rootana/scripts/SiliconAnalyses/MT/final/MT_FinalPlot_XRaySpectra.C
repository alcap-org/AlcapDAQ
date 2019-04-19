#include "scripts/XRayAnalysis/XRayUtils.h"

void MT_FinalPlot_XRaySpectra() {

  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");
  
  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};
  Int_t colours[n_settings] = {kBlue, kRed};
  std::string leglabels[n_settings] = {"SF = 1.05", "SF = 1.025"};

  TLegend* leg = new TLegend(0.40,0.25,0.80,0.7);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    
    std::string plots_file_name = "~/data/results/MT/plots_" + setting + ".root";
    std::string ge_channel = "GeHiGain";
    std::string full_spectrum_name = ge_channel + "/hEnergyTime";

    TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
    std::string newname = setting;
    TH1D* hXRaySpectrum = ((TH2F*) plots_file->Get(full_spectrum_name.c_str()))->ProjectionY(newname.c_str());


    hXRaySpectrum->Rebin(4);
    //  hXRaySpectrum->SetLineColor(kBlue);
    std::string histtitle = "MT Dataset, Full X-Ray Spectrum (" + ge_channel + ")";
    hXRaySpectrum->SetTitle(histtitle.c_str());
    hXRaySpectrum->SetStats(false);
    hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);
    hXRaySpectrum->SetLineColor(colours[i_setting]);

    std::stringstream axislabel;
    axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
    hXRaySpectrum->SetYTitle(axislabel.str().c_str());
    hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);   
    
    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_setting];
    leg->AddEntry(hXRaySpectrum, leglabel.str().c_str(), "l");

    if (i_setting == 0) {
      hXRaySpectrum->Draw("HIST E");
    }
    else {
      hXRaySpectrum->Draw("HIST E SAME");
    }
  }
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.50, 0.75, "AlCap Preliminary");

  leg->Draw();
}
