#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TLegend.h"

#include <iostream>
#include <sstream>

void SiLSpectra(TFile* file, TFile* out_file) {

  const int n_sil1_channels = 16;

  int bin_width = 25; // the bin width we want (keV)

  std::stringstream canvasname, histname, histtitle;
  for (int i_sil1_channel = 0; i_sil1_channel < n_sil1_channels; ++i_sil1_channel) {
    int si_channel_number = i_sil1_channel+1;

    canvasname.str("");
    canvasname << "c_SiL1-" << si_channel_number;
    TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());

    histname.str("");
    histname << "PlotTAP_StoppedMuonDiscrepancy/hSiL1-" << si_channel_number << "-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Energy_NoCuts";
    TH1F* hNoCuts = (TH1F*) (file->Get(histname.str().c_str())->Clone());
    hNoCuts->SetDirectory(0);
    if (!hNoCuts) {
      std::cout << "Problem getting histogram " << histname.str() << std::endl;
      return;
    }
    hNoCuts->SetStats(false);
    hNoCuts->Rebin(bin_width);
    histtitle.str("");
    histtitle << "SiL1-" << si_channel_number << "-S";
    hNoCuts->SetTitle(histtitle.str().c_str());

    histname.str("");
    histname << "PlotTAP_StoppedMuonDiscrepancy/hSiL1-" << si_channel_number << "-S_Energy_wNoSiL3Coincidence";
    TH1F* hCuts = (TH1F*) (file->Get(histname.str().c_str())->Clone());
    hCuts->SetDirectory(0);
    if (!hCuts) {
      std::cout << "Problem getting histogram " << histname.str() << std::endl;
      return;
    }
    hCuts->Rebin(bin_width);
    hCuts->SetLineColor(kRed);
    histtitle.str("");
    histtitle << "SiL1-" << si_channel_number << "-S (with no SiL3 coincidence)";
    hCuts->SetTitle(histtitle.str().c_str());

    hNoCuts->Draw();
    hCuts->Draw("SAME");

    TLegend *leg = new TLegend(0.5,0.8,0.7,0.6);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(0);
    leg->AddEntry(hNoCuts, hNoCuts->GetTitle(), "l");
    leg->AddEntry(hCuts, hCuts->GetTitle(), "l");
    leg->Draw();

    c->SetLogy();
    c->Write();
    out_file->Write();
  }

  canvasname.str("");
  canvasname << "c_SiL3-S";
  TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());

  histname.str("");
  histname << "PlotTAP_StoppedMuonDiscrepancy/hSiL3-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Energy_NoCuts";
  TH1F* hNoCuts = (TH1F*) (file->Get(histname.str().c_str())->Clone());
  hNoCuts->SetDirectory(0);
  if (!hNoCuts) {
    std::cout << "Problem getting histogram " << histname.str() << std::endl;
    return;
  }
  hNoCuts->SetStats(false);
  hNoCuts->Rebin(bin_width);
  histtitle.str("");
  histtitle << "SiL3-S";
  hNoCuts->SetTitle(histtitle.str().c_str());
  
  histname.str("");
  histname << "PlotTAP_StoppedMuonDiscrepancy/hSiL3_Energy_wSiL1Coincidence";
  TH1F* hCuts = (TH1F*) (file->Get(histname.str().c_str())->Clone());
  hCuts->SetDirectory(0);
  if (!hCuts) {
    std::cout << "Problem getting histogram " << histname.str() << std::endl;
    return;
  }
  hCuts->Rebin(bin_width);
  hCuts->SetLineColor(kRed);
  histtitle.str("");
  histtitle << "SiL3-S (with any SiL1-X-S coincidence)";
  hCuts->SetTitle(histtitle.str().c_str());

  hNoCuts->Draw();
  hCuts->Draw("SAME");
  
  TLegend *leg = new TLegend(0.5,0.8,0.7,0.6);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);
  leg->AddEntry(hNoCuts, hNoCuts->GetTitle(), "l");
  leg->AddEntry(hCuts, hCuts->GetTitle(), "l");
  leg->Draw();
  
  c->SetLogy();
  c->Write();
  out_file->Write();
}

