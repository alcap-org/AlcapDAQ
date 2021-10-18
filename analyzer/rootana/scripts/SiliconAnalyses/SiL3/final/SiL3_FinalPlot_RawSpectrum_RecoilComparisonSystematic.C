#include "../../../XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_RawSpectrum_RecoilComparisonSystematic(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  numbers_file << "% SiL3_FinalPlot_RawSpectrum_RecoilComparisonSystematic.C" << std::endl;
  
  TCanvas* c_FoldedSpectrum = new TCanvas("c_FoldedSpectrum", "c_FoldedSpectrum");
  c_FoldedSpectrum->SetLogy();
  
  const int n_spectra = 3;
  std::string settings[n_spectra] = {"allRecoil", "lowAllRecoil", "highAllRecoil"};
  std::string leglabels[n_spectra] = {"combined recoil", "low", "high"};
  Int_t colours[n_spectra] = {kBlue, kRed, kMagenta};

  //   const int n_spectra = 4;
  // std::string settings[n_spectra] = {"allRecoil", "lowAllRecoil", "highAllRecoil", "noRecoil"};
  // std::string leglabels[n_spectra] = {"combined recoil", "low", "high", "none"};
  // Int_t colours[n_spectra] = {kBlue, kRed, kMagenta, kBlack};


  TLegend* leg = new TLegend(0.40,0.55,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  std::string plots_file_name = "~/data/results/SiL3/raw_spectra_geq2TgtPulse_newPP20us_1.root";
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  for (int i_spectra = 0; i_spectra < n_spectra; ++i_spectra) {
    std::string i_setting = settings[i_spectra];
    std::string dirname = "SiL3_ActiveTarget_TimeSlice3000_4000_" + i_setting;
    std::string full_spectrum_name = dirname + "/hRawSpectrum";

    //    std::string newname = i_setting;
    TH1F* hFoldedSpectrum = (TH1F*) plots_file->Get(full_spectrum_name.c_str());
    if (!hFoldedSpectrum) {
      std::cout << "Can't find histogram with name \"" << full_spectrum_name << "\"" << std::endl;
      return;
    }
    std::string treename = dirname + "/cuttree";
    TTree* cuttree = (TTree*) plots_file->Get(treename.c_str());
    double recoil_fraction = 0;
    cuttree->SetBranchAddress("recoil_fraction", &recoil_fraction);
    cuttree->GetEntry(0);

    hFoldedSpectrum->Rebin(5);
    //  hFoldedSpectrum->SetLineColor(kBlue);
    std::string histtitle = "SiL3 Dataset, Target Spectrum (3000 ns < t < 4000 ns), Recoil Effects";
    hFoldedSpectrum->SetTitle(histtitle.c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 30);
    hFoldedSpectrum->GetXaxis()->SetTitleOffset(0.9);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(0.9);
    hFoldedSpectrum->SetLineColor(colours[i_spectra]);

    std::stringstream axislabel;
    axislabel << "Counts / " << hFoldedSpectrum->GetBinWidth(1) << " keV";
    std::cout << "AE: " << axislabel.str() << std::endl;
    hFoldedSpectrum->SetYTitle(axislabel.str().c_str());
    //    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_spectra];
    if (recoil_fraction>0) {
      leglabel << " (" << std::fixed << std::setprecision(1) << recoil_fraction*100 << "%)";

      if (i_spectra > 0) {
	numbers_file << std::fixed << std::setprecision(1) << "\\newcommand\\" << i_setting << "{$" << recoil_fraction*100 << "\\%$}" << std::endl;
      }
    }
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");

    alcaphistogram(hFoldedSpectrum);
    hFoldedSpectrum->SetLineWidth(1);

    if (i_spectra == 0) {
      hFoldedSpectrum->Draw("HIST E");
      alcapPreliminary(hFoldedSpectrum);
    }
    else {
      hFoldedSpectrum->Draw("HIST E SAME");
    }
  }
  
  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.50, 0.90, "AlCap Preliminary");

  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_RawSpectrum_RecoilComparisonSystematic";
    
    std::string pdfname = savename + ".pdf";
    c_FoldedSpectrum->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_FoldedSpectrum->SaveAs(pngname.c_str());
  }
  numbers_file << std::endl;
}
