#include "scripts/XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_FullXRaySpectrum() {

  std::string plots_file_name = "~/data/results/SiL3/plots_geq1TgtPulse.root";
  std::string ge_channel = "GeLoGain";
  std::string full_spectrum_name = ge_channel + "/hEnergyTime";
  std::string norm_file_name = "/home/edmonds/data/results/SiL3/normalisation_geq1TgtPulse.root";
  std::string norm_ws_name = "XRaySpectrum_" + ge_channel + "_noTimeCut/ws";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH1D* hXRaySpectrum = ((TH2F*) plots_file->Get(full_spectrum_name.c_str()))->ProjectionY();

  TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
  RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
  
  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");

  c_XRaySpectrum->SetLogy();
  hXRaySpectrum->Rebin(4);
  //  hXRaySpectrum->SetLineColor(kBlue);
  std::string histtitle = "SiL3 Dataset, Full X-Ray Spectrum (" + ge_channel + ")";
  hXRaySpectrum->SetTitle(histtitle.c_str());
  hXRaySpectrum->SetStats(false);
  hXRaySpectrum->GetXaxis()->SetRangeUser(250, 1200);
  hXRaySpectrum->SetMaximum(3e4);
  
  std::stringstream axislabel;
  axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
  hXRaySpectrum->SetYTitle(axislabel.str().c_str());
  hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hXRaySpectrum->Draw("HIST E");

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.50, 0.70, "AlCap Preliminary");
  //  latex->SetTextAngle(45);
  latex->DrawLatexNDC(0.15, 0.20, "AlCap Preliminary");

  XRay xray;
  xray.material = "Si";
  int min_transition = 2;
  int max_transition = 2;
  std::stringstream transition;
  for (int i_transition = min_transition; i_transition <= max_transition; ++i_transition) {
    transition.str("");
    transition << i_transition << "p-1s";
    xray.transition = transition.str();
    if (FillXRayInfo(xray, ge_channel) == 1) { // if we return an error code
      std::cout << "Error: Problem getting X-ray information" << std::endl;
      return;
    }

    double arrow_length = 2500;
    int i_transition_bin = hXRaySpectrum->FindBin(xray.energy);
    double arrow_y_end = hXRaySpectrum->GetBinContent(i_transition_bin)+100;
    double arrow_y_start = arrow_y_end+arrow_length;
    TArrow* arrow = new TArrow(xray.energy, arrow_y_start, xray.energy, arrow_y_end, 0.01);
    arrow->SetLineColor(kRed);
    arrow->SetLineWidth(2);
    arrow->Draw();

    std::stringstream text;
    text.str("");
    text << "#splitline{     " << transition.str() << "}{(" << xray.energy << " keV)}";
    latex->SetTextAlign(21);
    latex->SetTextSize(0.03);
    latex->DrawLatex(xray.energy, arrow_y_start+100, text.str().c_str());
  }


  TPad* inset_pad = new TPad("inset", "inset", 0.47, 0.45, 0.92, 0.90);
  inset_pad->SetFillStyle(0);
  inset_pad->Draw();
  inset_pad->cd();

  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle("Si 2p-1s X-Ray");
  Eframe->GetYaxis()->SetTitle("Count / 0.5 keV");
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);

  Eframe->Draw();
}
