#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TRandom3.h"
#include "TLegend.h"

#include <iostream>

// This macro draws 4 random TPIs and TAPs that were exported using ExportPulse in rootana
void DrawTPIsAndTAPs_fromPlotTAP_Energy_wCoincidence(std::string filename, int seed = 0) {
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << filename << std::endl;
    return;
  }

  int n_pulses_to_draw = 4;
  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->Divide(n_pulses_to_draw/2, n_pulses_to_draw/2);

  TDirectory* tpi_dir = file->GetDirectory("ExportPulse/TPIs");
  TList* tpi_hists = tpi_dir->GetListOfKeys();
  int n_max_hists = tpi_hists->GetSize();

  TRandom3 random(seed);

  for (int i_hist = 0; i_hist < n_pulses_to_draw; ++i_hist) {
    int rndm = (random.Rndm())*(n_max_hists/2); // these should be plotted in pairs

    TH1F* hTPIs[2];
    TH1F* hTAPs[2];

    for (int i_pair = 0; i_pair < 2; ++i_pair) {

      c1->cd(i_hist+1);
      std::string pulse_name = tpi_hists->At(rndm*2 + i_pair)->GetName();
      std::string tpi_histname = "ExportPulse/TPIs/" + pulse_name;
      std::cout << "AE: " << rndm*2 + i_pair << " = " <<tpi_histname << std::endl;
      
      hTPIs[i_pair] = (TH1F*) file->Get(tpi_histname.c_str());
      if (!hTPIs[i_pair]) {
	std::cout << "Problem obtaining TPI histogram " << tpi_histname << std::endl;
	return;
      }
      
      std::string tap_histname = "ExportPulse/TAPs/" + pulse_name + "_AP";
      hTAPs[i_pair] = (TH1F*) file->Get(tap_histname.c_str());
      if (!hTAPs[i_pair]) {
	std::cout << "Problem obtaining TAP histogram " << tap_histname << std::endl;
	return;
      }
      hTAPs[i_pair]->SetLineStyle(kDashed);
    }

    double min_time_tpi_1 = hTPIs[0]->GetXaxis()->GetBinLowEdge(1);
    double min_time_tpi_2 = hTPIs[1]->GetXaxis()->GetBinLowEdge(1);
    double min_time = min_time_tpi_2 > min_time_tpi_1 ? min_time_tpi_1 : min_time_tpi_2;

    double max_time_tpi_1 = hTPIs[0]->GetXaxis()->GetBinUpEdge(hTPIs[0]->GetNbinsX());
    double max_time_tpi_2 = hTPIs[1]->GetXaxis()->GetBinUpEdge(hTPIs[1]->GetNbinsX());
    double max_time = max_time_tpi_2 > max_time_tpi_1 ? max_time_tpi_2 : max_time_tpi_1;

    double bin_width = hTPIs[0]->GetXaxis()->GetBinWidth(1);
    int n_bins = (max_time - min_time) / bin_width;

    TH1F* hCombined = new TH1F("hCombined", "", n_bins,min_time,max_time);
    hCombined->SetStats(false);
    hCombined->SetLineColor(kWhite);
    hCombined->SetXTitle(hTPIs[0]->GetXaxis()->GetTitle());
    hCombined->SetYTitle(hTPIs[0]->GetYaxis()->GetTitle());
  
    hCombined->Draw();
    hTPIs[0]->Draw("SAME");
    hTAPs[0]->Draw("SAME");
    hTPIs[1]->Draw("SAME");
    hTPIs[1]->SetLineColor(kRed);
    hTAPs[1]->Draw("SAME");  
    hTAPs[1]->SetLineColor(kRed);

    TLegend *leg = new TLegend(0.4,1.0,0.6,0.8);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(0);
    leg->AddEntry(hTPIs[0], hTPIs[0]->GetTitle(), "l");
    leg->AddEntry(hTPIs[1], hTPIs[1]->GetTitle(), "l");
    leg->Draw();

    double max_y_tpi_1 = hTPIs[0]->GetMaximum();
    double max_y_tpi_2 = hTPIs[1]->GetMaximum();
    double max_y = max_y_tpi_2 > max_y_tpi_1 ? max_y_tpi_2 : max_y_tpi_1;
    double min_y_tpi_1 = hTPIs[0]->GetMinimum();
    double min_y_tpi_2 = hTPIs[1]->GetMinimum();
    double min_y = min_y_tpi_2 > min_y_tpi_1 ? min_y_tpi_1 : min_y_tpi_2;

    hCombined->SetMaximum(max_y*1.1);
    hCombined->SetMinimum(min_y*1.1);
  }
}
