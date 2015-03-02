#include <iostream>
#include <algorithm>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"

struct Arm {
  std::string armname;
  TH2F* selected_band;
} LeftArm, RightArm;

void ProtonBandGraphicalCut(std::string identifier, std::string filename, std::string baseplotname, std::string output_location);

void DataAndMC() {

  // Set up the arms
  LeftArm.armname = "SiL";
  RightArm.armname = "SiR";

  // Data
  std::string version = "v92";
  std::string location = "~/data/out/"+version;
  std::string filename = location+"/total.root";
  std::string savelocation = location+"/plots";
  std::string baseplotname = "TME_Al50_EvdE/all_particles/ARM_EvdE";
  ProtonBandGraphicalCut("data", filename, baseplotname, savelocation);

}

void ProtonBandGraphicalCut(std::string identifier, std::string filename, std::string baseplotname, std::string output_location) {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::vector<Arm> arms;
  arms.push_back(LeftArm);
  arms.push_back(RightArm);

  TFile* file = new TFile(filename.c_str(), "READ");
  std::string outfilename = "result_Al50_"+identifier+".root";
  TFile* output_file = new TFile(outfilename.c_str(), "RECREATE");

  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string plotname = baseplotname;
    plotname.replace(baseplotname.find("ARM"), 3, i_arm->armname);
    i_arm->selected_band = (TH2F*) (file->Get(plotname.c_str()))->Clone();
  
    // straight line cut to remove electrons
    double x_1 = 0, y_1 = 2000, x_2 = 4000, y_2 = 0;
    double gradient = (y_2 - y_1) / (x_2 - x_1);
    double offset = y_1;
    TF1* electron_spot_cut = new TF1("electron_spot_cut", "[0]*x + [1]", 0, 15000);
    electron_spot_cut->SetLineColor(kBlue);
    electron_spot_cut->SetParameter(0, gradient);
    electron_spot_cut->SetParameter(1, offset);

    double punch_through_yoffset = 300;
    TF1* punch_through_cut = new TF1("punch_through_cut", "[0]", 0, 25000);
    punch_through_cut->SetLineColor(kBlue);
    punch_through_cut->SetParameter(0, punch_through_yoffset);


    // Cut to remove the remaining deuteron band
    TF1* deuteron_cut = new TF1("deuteron_cut", "[0]*TMath::Exp([1]*x) + [2]", 0, 25000);
    deuteron_cut->SetLineColor(kBlue);
    deuteron_cut->SetParameter(0, 4500);
    deuteron_cut->SetParameter(1, -0.0004);
    deuteron_cut->SetParameter(2, 500);
    //    evde_hists[i_arm]->Fit(deuteron_cut, "R");

    int n_bins_x = i_arm->selected_band->GetNbinsX();
    int n_bins_y = i_arm->selected_band->GetNbinsY();
    for (int i_bin = 1; i_bin <= n_bins_x; ++i_bin) {
      for (int j_bin = 1; j_bin <= n_bins_y; ++j_bin) {

	double x_coord = i_arm->selected_band->GetXaxis()->GetBinCenter(i_bin);
	double y_coord = i_arm->selected_band->GetYaxis()->GetBinCenter(j_bin);
	
	if (i_arm->selected_band->GetBinContent(i_bin, j_bin) < 20 ||
	    y_coord < electron_spot_cut->Eval(x_coord) || 
	    y_coord < punch_through_cut->Eval(x_coord) ||
	    y_coord > deuteron_cut->Eval(x_coord) ) {

	  i_arm->selected_band->SetBinContent(i_bin, j_bin, 0);
	}
      }
    }
    i_arm->selected_band->ResetStats();    
    i_arm->selected_band->Draw("COLZ");
    //    electron_spot_cut->Draw("LSAME");
    //    punch_through_cut->Draw("LSAME");
    //    deuteron_cut->Draw("LSAME");

    std::string output_plotname = output_location+"/"+i_arm->armname+"_EvdE_SelectedBand";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    //    c1->SaveAs(pdfname.c_str());
    //    c1->SaveAs(pngname.c_str());

    TH1D* hProjection = i_arm->selected_band->ProjectionX();
    //    hProjection->Rebin(10);
    hProjection->Draw("HIST E");
    output_plotname = output_location+"/"+i_arm->armname+"_EvdE_SelectedBand_ProjectionX";
    pdfname = output_plotname+".pdf";
    pngname = output_plotname+".png";
    //    c1->SaveAs(pdfname.c_str());
    //    c1->SaveAs(pngname.c_str());

    i_arm->selected_band->Write();
    hProjection->Write();

    double energy_range_low = 4000;
    double energy_range_high = 8000;
    int bin_low = hProjection->FindBin(energy_range_low);
    int bin_high = hProjection->FindBin(energy_range_high);
    std::cout << i_arm->armname << " Proton Integral (" << energy_range_low << " - " << energy_range_high << " keV) = " << hProjection->Integral(bin_low, bin_high) << std::endl;

    // Now want to do what I did in the MC (i.e. go through each energy bin and get a mean and rms for what I think is the stopped protons
    double n_bins = i_arm->selected_band->GetNbinsX();

    // Loop through the energy bins
    std::string outfilename = "pid-cuts-" + i_arm->armname + ".txt";
    std::ofstream output(outfilename.c_str(), std::ofstream::out);
    output << "energy/D:p_stop_mean/D:p_stop_rms/D:proton_mean/D:proton_rms/D:deuteron_mean/D:deuteron_rms/D:triton_mean/D:triton_rms/D:alpha_mean/D:\
alpha_rms/D";
    int n_entries_covered = 0;
    for (int i_bin = 1; i_bin <= n_bins; ++i_bin) {
      double energy = i_arm->selected_band->GetXaxis()->GetBinLowEdge(i_bin);
      TH1D* hProjection = i_arm->selected_band->ProjectionY("_py", i_bin, i_bin);

      double mean = hProjection->GetMean();
      double rms = hProjection->GetRMS();

      int integral_low = hProjection->FindBin(mean - 2*rms);
      int integral_high = hProjection->FindBin(mean + 2*rms);
      n_entries_covered += hProjection->Integral(integral_low, integral_high);

      output << energy << " " << mean << " " << rms << " 0 0 0 0 0 0 0 0" << std::endl;
    }
    std::cout << i_arm->armname << ": " << n_entries_covered << " / " << i_arm->selected_band->GetEntries() << " " << n_entries_covered / i_arm->selected_band->GetEntries() << std::endl;
  }
}
