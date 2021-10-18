void Si16b_FinalPlot_UnfoldedCovarianceMatrix() {

  gStyle->SetPaintTextFormat("4.0f");

  int rebin_factor = 1;
  std::stringstream axistitle;
  TLegend* leg = new TLegend(0.55,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TCanvas* c = new TCanvas();
  //  c->SetLogy();

  
  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_covError_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  
  std::string unfold_name = "ResponseMatrix_proton_TCutG_2sig_layerCoinc500ns_tGT0ns/unfold";
  RooUnfoldBayes* unfold = (RooUnfoldBayes*) infile->Get(unfold_name.c_str());
  //  auto covMatrix = unfold->GetMeasuredCov();
  auto covMatrix = unfold->Ereco();
  covMatrix.Draw("COLZ TEXT");
  //  leg->Draw();
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
