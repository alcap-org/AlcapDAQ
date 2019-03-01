void Si16b_FinalPlot_XRaySpectrum() {

  std::string plots_file_name = "~/data/results_old/Si16b_passive/plots.root";
  std::string full_spectrum_name = "hXRaySpectrum_GeLoGain";
  std::string norm_file_name = "/home/edmonds/data/results_old/Si16b_passive/normalisation.root";
  std::string norm_ws_name = "ws_timecut";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH1D* hXRaySpectrum = ((TH2F*) plots_file->Get(full_spectrum_name.c_str()))->ProjectionY();

  TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
  RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
  
  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");

  hXRaySpectrum->Rebin(4);
  //  hXRaySpectrum->SetLineColor(kBlue);
  hXRaySpectrum->SetTitle("Si16b Dataset, Full X-Ray Spectrum");
  hXRaySpectrum->SetStats(false);
  hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);

  std::stringstream axislabel;
  axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
  hXRaySpectrum->SetYTitle(axislabel.str().c_str());
  hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hXRaySpectrum->Draw("HIST E");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.30, 0.70, "AlCap Preliminary");

  double xray_energy = 400.177;
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


  TPad* inset_pad = new TPad("inset", "inset", 0.47, 0.15, 0.92, 0.59);
  inset_pad->SetFillStyle(0);
  inset_pad->Draw();
  inset_pad->cd();

  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle("Si 2p-1s X-Ray");
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);
  Eframe->Draw();

}
