#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct RemoveFlatBkgArgs {
  std::string infilename;
  std::string inhistname;
  std::string inflatbkgname;

  std::string outfilename;
  std::string outdirname;
};

void RemoveFlatBkg(const RemoveFlatBkgArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH2F* hEnergyTime = (TH2F*) file->Get(args.inhistname.c_str());
  hEnergyTime->SetName("hInputSpectrum");
  if (!hEnergyTime) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
    return;
  }
  TH2F* hFlatBkg = (TH2F*) file->Get(args.inflatbkgname.c_str());
  hFlatBkg->SetName("hFlatBkg");
  if (!hFlatBkg) {
    std::cout << "Problem getting histogram " << args.inflatbkgname.c_str() << std::endl;
    return;
  }

  // First make sure that the bin widths are the same
  if (hFlatBkg->GetXaxis()->GetBinWidth(1) != hEnergyTime->GetXaxis()->GetBinWidth(1)) {
    std::cout << "Histograms have different x bin widths. Won't be able to subtract..." << std::endl;
    return;
  }
  if (hFlatBkg->GetYaxis()->GetBinWidth(1) != hEnergyTime->GetYaxis()->GetBinWidth(1)) {
    std::cout << "Histograms have different y bin widths. Won't be able to subtract..." << std::endl;
    return;
  }

  // Now make the flat bkg histogram as wide as the original
  // First, get the average number of entries for each row (because this is flat in time)
  /*  const int n_bins_y = hFlatBkg->GetYaxis()->GetNbins();
  double mean[n_bins_y];
  for (int j_bin = 1; j_bin <= n_bins_y; ++j_bin) {
    double& this_mean = mean[j_bin-1];
    this_mean = 0;
    for (int i_bin = 1; i_bin <= hFlatBkg->GetXaxis()->GetNbins(); ++i_bin) {
      this_mean += hFlatBkg->GetBinContent(i_bin, j_bin);
    }
    std::cout << "Total = " << this_mean << ", ";
    if (this_mean > 0) {
      this_mean /= hFlatBkg->GetXaxis()->GetNbins();
    }
    std::cout << "Mean = " << this_mean << std::endl;
  }
  // Then, fill out the full size flat background
  TH2F* hFlatBkg_FullSize = (TH2F*) hEnergyTime->Clone("hFlatBkg_FullSize");
  hFlatBkg_FullSize->Reset();
  for (int j_bin = 1; j_bin <= hFlatBkg_FullSize->GetYaxis()->GetNbins(); ++j_bin) {
    const double& this_mean = mean[j_bin-1];
    std::cout << "This Mean = " << this_mean << std::endl;
    for (int i_bin = 1; i_bin <= hFlatBkg_FullSize->GetXaxis()->GetNbins(); ++i_bin) {
      hFlatBkg_FullSize->SetBinContent(i_bin, j_bin, this_mean);
    }
  }
  
  // Now do the subtractions
  TH2F* hEnergyTime_wFlatBkgRemoval = (TH2F*) hEnergyTime->Clone("hEnergyTime");
  hEnergyTime_wFlatBkgRemoval->Add(hFlatBkg_FullSize, -1);
  */

  /*TH2F* hFlatBkg_FullSize = (TH2F*) hEnergyTime->Clone("hFlatBkg_FullSize");
  hFlatBkg_FullSize->Reset();
  // copy in the half that we've done
  for (int j_bin = 1; j_bin <= hFlatBkg->GetYaxis()->GetNbins(); ++j_bin) {
    for (int i_bin = 1; i_bin <= hFlatBkg->GetXaxis()->GetNbins(); ++i_bin) {
      double bin_x = hFlatBkg->GetXaxis()->GetBinCenter(i_bin);
      double bin_y = hFlatBkg->GetYaxis()->GetBinCenter(j_bin);
      int i_bin_new = hFlatBkg_FullSize->GetXaxis()->FindBin(bin_x);
      int j_bin_new = hFlatBkg_FullSize->GetYaxis()->FindBin(bin_y);
      double original_content = hFlatBkg->GetBinContent(i_bin, j_bin);
      //      if (original_content>0) {
      //	std::cout << "Old (" << i_bin << ", " << j_bin << ") = (" << bin_x << ", " << bin_y << ") ==> (" << i_bin_new << ", " << j_bin_new << ") = " << hFlatBkg_FullSize->GetXaxis()->GetBinCenter(i_bin_new) << ", " << hFlatBkg_FullSize->GetYaxis()->GetBinCenter(j_bin_new) << ": " << original_content << std::endl;
      //      }
      if (original_content>0) {
	hFlatBkg_FullSize->SetBinContent(i_bin_new, j_bin_new, original_content);
      }
    }
  }

  // mirror it over onto the other side
  for (int j_bin = 1; j_bin <= hFlatBkg_FullSize->GetYaxis()->GetNbins(); ++j_bin) {
    for (int i_bin = 1; i_bin <= hFlatBkg_FullSize->GetXaxis()->GetNbins(); ++i_bin) {
      double original_content = hFlatBkg_FullSize->GetBinContent(i_bin, j_bin);
      if (original_content==0) {
	continue;
      }
      double bin_x = hFlatBkg_FullSize->GetXaxis()->GetBinCenter(i_bin);
      double bin_y = hFlatBkg_FullSize->GetYaxis()->GetBinCenter(j_bin);
      int i_bin_new = hFlatBkg_FullSize->GetXaxis()->FindBin(-bin_x);
      int j_bin_new = hFlatBkg_FullSize->GetYaxis()->FindBin(bin_y);
      //      if (original_content>0) {
      //	std::cout << "Old (" << i_bin << ", " << j_bin << ") = (" << bin_x << ", " << bin_y << ") ==> (" << i_bin_new << ", " << j_bin_new << ") = " << hFlatBkg_FullSize->GetXaxis()->GetBinCenter(i_bin_new) << ", " << hFlatBkg_FullSize->GetYaxis()->GetBinCenter(j_bin_new) << ": " << original_content << std::endl;
      //      }
      if (original_content>0) {
	hFlatBkg_FullSize->SetBinContent(i_bin_new, j_bin_new, original_content);
      }
    }
  }
  */
  TH2F* hEnergyTime_wFlatBkgRemoval = (TH2F*) hEnergyTime->Clone("hEnergyTime");
  //  hEnergyTime_wFlatBkgRemoval->Add(hFlatBkg_FullSize, -1);
  hEnergyTime_wFlatBkgRemoval->Add(hFlatBkg, -1);
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hEnergyTime->Write();
  hFlatBkg->Write();
  //  hFlatBkg_FullSize->Write();
  hEnergyTime_wFlatBkgRemoval->Write();
  outfile->Write();
  outfile->Close();
}
