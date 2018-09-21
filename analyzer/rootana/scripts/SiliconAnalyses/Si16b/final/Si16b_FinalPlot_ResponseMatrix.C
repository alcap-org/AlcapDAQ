void Si16b_FinalPlot_ResponseMatrix() {

  std::string infilename = "~/data/results/Si16b_passive/unfolds.root";
  std::string inhistname = "hResponseMatrix_proton";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

  TCanvas* c_Response = new TCanvas("c_Response", "c_Response");

  hResponseMatrix->SetTitle("Si16b MC, Right Arm, Proton Response Matrix");
  hResponseMatrix->SetStats(false);
  //  gStyle->SetOptFit(1);
  hResponseMatrix->GetYaxis()->SetTitleOffset(1.3);
  hResponseMatrix->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
