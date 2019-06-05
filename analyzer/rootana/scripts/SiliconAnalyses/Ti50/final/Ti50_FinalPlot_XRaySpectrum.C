void Ti50_FinalPlot_XRaySpectrum() {

  
  std::string plots_file_name = "~/data/results/Ti50/plots.root";
  std::string ge_channel = "GeHiGain";
  std::string full_spectrum_name = ge_channel + "/hEnergyTime";
  std::string norm_file_name = "/home/edmonds/data/results/Ti50/normalisation.root";
  std::string norm_ws_name = "XRaySpectrum_" + ge_channel + "_noTimeCut/ws";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH1D* hXRaySpectrum = ((TH2F*) plots_file->Get(full_spectrum_name.c_str()))->ProjectionY();

  TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
  RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
  
  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");

  hXRaySpectrum->Rebin(4);
  //  hXRaySpectrum->SetLineColor(kBlue);
  hXRaySpectrum->SetTitle("Ti50 Dataset, Full X-Ray Spectrum");
  hXRaySpectrum->SetStats(false);
  hXRaySpectrum->GetXaxis()->SetRangeUser(182, 1200);

  std::stringstream axislabel;
  axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
  hXRaySpectrum->SetYTitle(axislabel.str().c_str());
  hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hXRaySpectrum->Draw("HIST E");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.60, 0.40, "AlCap Preliminary");

  double xray_energy = 931;
  int bin_2p1s = hXRaySpectrum->FindBin(xray_energy);
  double arrow_y_end = hXRaySpectrum->GetBinContent(bin_2p1s)*1.05;
  double arrow_y_start = arrow_y_end*1.20;
  TArrow* arrow = new TArrow(xray_energy, arrow_y_start, xray_energy, arrow_y_end, 0.01);
  arrow->SetLineColor(kRed);
  arrow->SetLineWidth(2);
  arrow->Draw();


  latex->SetTextAlign(22);
  latex->SetTextSize(0.04);
  latex->DrawLatex(xray_energy, arrow_y_start*1.30, "2p-1s");
  
  std::stringstream text;
  text.str("");
  text << "(" << xray_energy << " keV)";
  latex->SetTextSize(0.034);
  latex->DrawLatex(xray_energy, arrow_y_start*1.12, text.str().c_str());


  TPad* inset_pad = new TPad("inset", "inset", 0.47, 0.45, 0.92, 0.89);
  inset_pad->SetFillStyle(0);
  inset_pad->Draw();
  inset_pad->cd();

  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle("Ti 2p-1s X-Ray");
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe, RooFit::Components("xraypeak_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  (ws->pdf("sum"))->plotOn(Eframe, RooFit::Components("bkgpeak_pdf"), RooFit::LineStyle(kDashed));
  (ws->pdf("sum"))->plotOn(Eframe, RooFit::Components("pol1_bkg"), RooFit::LineStyle(kDashed));

  Eframe->Draw();

}
