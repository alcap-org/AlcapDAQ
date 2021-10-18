void Si16b_FinalPlot_UnfoldedCorrelationMatrix() {

  gStyle->SetPaintTextFormat("4.2f");

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
  TMatrixD covMatrix = unfold->Ereco();
  TMatrixD corrMatrix = covMatrix;
  for (int i_bin = 0; i_bin < covMatrix.GetNrows(); ++i_bin) {
    for (int j_bin = 0; j_bin < covMatrix.GetNcols(); ++j_bin) {
      //      std::cout << "(i=" << i_bin << ", j=" << j_bin << ") = " << covMatrix(i_bin, j_bin) << std::endl;
      //      std::cout << "\t(i=" << i_bin << ", i=" << i_bin << ") = " << covMatrix(i_bin, i_bin) << std::endl;
      //      std::cout << "\t(j=" << j_bin << ", j=" << j_bin << ") = " << covMatrix(j_bin, j_bin) << std::endl;
      if (corrMatrix(i_bin,i_bin) != 0 && corrMatrix(j_bin,j_bin) != 0) {
	(corrMatrix)(i_bin, j_bin) = (covMatrix)(i_bin, j_bin)/(std::sqrt((covMatrix)(i_bin, i_bin)) * std::sqrt((covMatrix)(j_bin, j_bin)));
	//	std::cout << "\t(i=" << i_bin << ", j=" << j_bin << ") = " << corrMatrix(i_bin, j_bin) << std::endl;
      }
    }
  }
  TH2D* hist = new TH2D(corrMatrix);
  hist->SetMinimum(-1.0);
  hist->SetMaximum(1.0);
  hist->Draw("COLZ TEXT");

  
  //  leg->Draw();
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
