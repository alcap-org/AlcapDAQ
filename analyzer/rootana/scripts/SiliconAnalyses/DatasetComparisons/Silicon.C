#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TH1.h"
#include "TTree.h"
#include "TF1.h"

#include <iostream>

struct SiAnalysis {

  std::string dataset;
  std::string analysis;

  std::string filename;
  std::string histname;
  int rebin_factor;
  Int_t colour;

  std::string normfilename;
  std::string counttree;
  double capture_fraction;

  double n_stopped_muons;
  double n_captured_muons;

  TH1F* spectrum;
};

void Silicon() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string SW_filename = "~/data/results/SiL3/SW-plots.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->Draw("APE");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,26000);
  SW_gre->GetXaxis()->SetTitle("Energy [keV]");
  SW_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture");
  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  SiAnalysis SiL3_target;
  SiL3_target.dataset = "SiL3";
  SiL3_target.analysis = "active target";
  SiL3_target.filename = "~/data/results/SiL3/unfold.root";
  SiL3_target.histname = "TimeCut_TimeSlice2000_4000/hCorrectedSpectrum"; //"ProtonEscapeCorrection_TimeSlice2000_4000/hUnfoldedSpectrum",
  SiL3_target.rebin_factor = 1;
  SiL3_target.colour = kGray;
  SiL3_target.normfilename = "~/data/results/SiL3/normalisation.root";
  SiL3_target.counttree = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  SiL3_target.capture_fraction = 0.658;

  SiAnalysis Si16b_right_protons;
  Si16b_right_protons.dataset = "Si16b";
  Si16b_right_protons.analysis = "SiR protons";
  Si16b_right_protons.filename = "~/data/results/Si16b/unfold.root";
  Si16b_right_protons.histname = "ResponseMatrix_proton_combined/hUnfoldedSpectrum";
  Si16b_right_protons.rebin_factor = 1;
  Si16b_right_protons.colour = kRed;
  Si16b_right_protons.normfilename = "~/data/results/Si16b/normalisation.root";
  Si16b_right_protons.counttree = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Si16b_right_protons.capture_fraction = 0.658;

  SiAnalysis Si16b_right_pdta;
  Si16b_right_pdta.dataset = "Si16b";
  Si16b_right_pdta.analysis = "SiR p+d+t+#alpha";
  Si16b_right_pdta.filename = "~/data/results/Si16b/unfold.root";
  Si16b_right_pdta.histname = "AllParticles/hUnfoldedSpectrum",
  Si16b_right_pdta.rebin_factor = 1;
  Si16b_right_pdta.colour = kMagenta;
  Si16b_right_pdta.normfilename = "~/data/results/Si16b/normalisation.root";
  Si16b_right_pdta.counttree = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Si16b_right_pdta.capture_fraction = 0.658;

  SiAnalysis Si16b_right_inclusive;
  Si16b_right_inclusive.dataset = "Si16b";
  Si16b_right_inclusive.analysis = "SiR2 inclusive";
  Si16b_right_inclusive.filename = "~/data/results/Si16b/unfold.root";
  Si16b_right_inclusive.histname = "GeomAcceptance_SiR2/hCorrectedSpectrum",
  Si16b_right_inclusive.rebin_factor = 5;
  Si16b_right_inclusive.colour = kCyan;
  Si16b_right_inclusive.normfilename = "~/data/results/Si16b/normalisation.root";
  Si16b_right_inclusive.counttree = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Si16b_right_inclusive.capture_fraction = 0.658;
  
  SiAnalysis Si16b_left_inclusive;
  Si16b_left_inclusive.dataset = "Si16b";
  Si16b_left_inclusive.analysis = "SiL3 inclusive";
  Si16b_left_inclusive.filename = "~/data/results/Si16b/unfold.root";
  Si16b_left_inclusive.histname = "GeomAcceptance_SiL3/hCorrectedSpectrum";
  Si16b_left_inclusive.rebin_factor = 5;
  Si16b_left_inclusive.colour = kBlue;
  Si16b_left_inclusive.normfilename = "~/data/results/Si16b/normalisation.root";
  Si16b_left_inclusive.counttree = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Si16b_left_inclusive.capture_fraction = 0.658;

  std::vector<SiAnalysis> siAnalyses;
  siAnalyses.push_back(SiL3_target);
  siAnalyses.push_back(Si16b_right_protons);
  siAnalyses.push_back(Si16b_right_pdta);
  siAnalyses.push_back(Si16b_right_inclusive);
  siAnalyses.push_back(Si16b_left_inclusive);
  
  for (std::vector<SiAnalysis>::iterator i_siAnalysis = siAnalyses.begin(); i_siAnalysis != siAnalyses.end(); ++ i_siAnalysis) {

    TFile* norm_file = new TFile(i_siAnalysis->normfilename.c_str(), "READ");
    if (!norm_file) {
      std::cout << "Error: Problem getting normalisation file " << i_siAnalysis->normfilename << std::endl;
      return;
    }
    TTree* counttree = (TTree*) norm_file->Get(i_siAnalysis->counttree.c_str());
    if (!counttree) {
      std::cout << "Error: Problem getting counttree " << i_siAnalysis->counttree << std::endl;
      return;
    }
    double n_stopped_muons = 0;
    counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
    counttree->GetEntry(0);
    i_siAnalysis->n_stopped_muons = n_stopped_muons;
    i_siAnalysis->n_captured_muons = i_siAnalysis->n_stopped_muons * i_siAnalysis->capture_fraction;
    std::cout << "AE: n_stopped_muons = " << i_siAnalysis->n_stopped_muons << std::endl;
    std::cout << "AE: n_captured_muons = " << i_siAnalysis->n_captured_muons << std::endl;

    TFile* file = new TFile(i_siAnalysis->filename.c_str(), "READ");
    if (!file) {
      std::cout << "Error: Problem getting hist file " << i_siAnalysis->filename.c_str() << std::endl;
      return;
    }
    i_siAnalysis->spectrum = (TH1F*) file->Get(i_siAnalysis->histname.c_str());
    if (!i_siAnalysis->spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_siAnalysis->histname << std::endl;
      return;
    }
    i_siAnalysis->spectrum->Sumw2();
    i_siAnalysis->spectrum->Scale(1.0 / (i_siAnalysis->spectrum->GetXaxis()->GetBinWidth(1)) ); // per keV
    i_siAnalysis->spectrum->Rebin(i_siAnalysis->rebin_factor);
    i_siAnalysis->spectrum->Scale(1.0 / i_siAnalysis->rebin_factor); // rescale to per keV
    i_siAnalysis->spectrum->Scale(1.0 / i_siAnalysis->n_captured_muons); // now scale to per captured muon per keV

    i_siAnalysis->spectrum->SetStats(false);
    i_siAnalysis->spectrum->SetLineWidth(1);
    i_siAnalysis->spectrum->SetLineColor(i_siAnalysis->colour);

    i_siAnalysis->spectrum->Draw("HIST E SAMES");

    std::string leglabel;
    leglabel = i_siAnalysis->dataset + " (" + i_siAnalysis->analysis + ")";
    leg->AddEntry(i_siAnalysis->spectrum, leglabel.c_str(), "l");
  }

  
  const int n_ranges = 5;
  double min_energies[n_ranges] = {1400,  10000, 5000,   3500, 4000};
  double max_energies[n_ranges] = {26000, 26000, 10000, 10000, 8000};

  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];
    bool start_integral = false;
    double SW_integral = 0;
    for (int i_element = 0; i_element < SW_gre->GetN(); ++i_element) {
      if (*(SW_gre->GetX()+i_element) > min_energy) {
	start_integral = true;
      }
      if (start_integral) {
	double previous_x = *(SW_gre->GetX()+i_element-1);
	double this_x = *(SW_gre->GetX()+i_element);
	double width_x = this_x - previous_x;
	SW_integral += *(SW_gre->GetY()+i_element) * width_x;
      }
      if (*(SW_gre->GetX()+i_element) > max_energy) {
	break;
      }
    }
    std::cout << "S-W Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SW_integral << std::endl;

    for (std::vector<SiAnalysis>::iterator i_siAnalysis = siAnalyses.begin(); i_siAnalysis != siAnalyses.end(); ++ i_siAnalysis) {
      int min_energy_bin = i_siAnalysis->spectrum->GetXaxis()->FindBin(min_energy);
      int max_energy_bin = i_siAnalysis->spectrum->GetXaxis()->FindBin(max_energy) - 1;
      std::cout << "AlCap, " << i_siAnalysis->dataset << ", " << i_siAnalysis->analysis << ": Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << i_siAnalysis->spectrum->Integral(min_energy_bin, max_energy_bin, "width") << std::endl;
    }
  }

  leg->Draw();
}
