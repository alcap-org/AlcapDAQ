#include "../unfold/Si16b_Unfold_ResponseMatrix.C"
#include "../unfold/Si16b_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_UnfoldingBinWidths(std::string infilename, std::string outfilename,
					 std::string countfilename, std::string counttreename,
					 std::string pid, std::string particle_name,
					 std::string sigma, std::string layer_coinc,
					 std::string timecut, std::string inbinw) {

  // Number of bin widths
  const int n_bin_widths = 1;
  int bin_widths[n_bin_widths] = {100};
  std::string BinWs[n_bin_widths] = {"100keV"};

  for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
    std::string binw = BinWs[i_bin_width];

    std::string outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_BinW" + binw;
    // convert previously unfolded spectrum to correct binning
    TFile* infile = new TFile(infilename.c_str(), "READ");
    std::string centralhistname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + inbinw + "/hNormalisedSpectrum";
    std::string systhistname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_BinW" + binw + "/hNormalisedSpectrum";

    TH1F* hCentralHist = (TH1F*) infile->Get(centralhistname.c_str());
    TH1F* hSystHist = (TH1F*) infile->Get(systhistname.c_str());
    TH1F* hSystHistNewBin = (TH1F*) hCentralHist->Clone("hNormalisedSpectrum");
    for (int i_bin = 1; i_bin <= hSystHistNewBin->GetNbinsX(); ++i_bin) {
      double E = hSystHistNewBin->GetBinCenter(i_bin);
      double new_bin_content = hSystHist->GetBinContent(hSystHist->FindBin(E));
      double new_bin_error = hSystHist->GetBinError(hSystHist->FindBin(E));
      hSystHistNewBin->SetBinContent(i_bin, new_bin_content);
      hSystHistNewBin->SetBinError(i_bin, new_bin_error);
    }
    TFile* outfile = new TFile(outfilename.c_str(), "UPDATE");
    TDirectory* outdir = outfile->mkdir(outdirname.c_str());
    outdir->cd();
    hSystHistNewBin->Write();
    outfile->Write();
    outfile->Close();
    infile->Close();
    
    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.central_val_histname = centralhistname;
    args.rebin_factor = 1; // need to rebin the 500 keV to be 1 MeV to see any change in rates
    args.central_rebin_factor = args.rebin_factor;
    args.syst_val_filename = outfilename;
    args.syst_val_histname = outdirname + "/hNormalisedSpectrum";
    args.outfilename = outfilename;
    args.outdirname = outdirname + "_SystPlot";
    Systematic_MakePlot(args);
  }
}
