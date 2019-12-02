void SiL3_FinalPlot_ResponseMatrix() {

  std::string infilename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  //  std::string particle = "Proton";
  //  std::string particle = "Deuteron";
  std::string particle = "Combined";
  std::string inhistname = particle + "Escape_TimeSlice2000_4000/hResponseMatrix";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

  TCanvas* c_Response = new TCanvas("c_Response", "c_Response");

  std::string histtitle;
  histtitle = "SiL3 MC, " + particle + " Escape Response Matrix";
  hResponseMatrix->SetTitle(histtitle.c_str());
  hResponseMatrix->SetStats(false);
  //  gStyle->SetOptFit(1);
  hResponseMatrix->GetYaxis()->SetTitleOffset(1.3);
  hResponseMatrix->SetXTitle("Observed Energy [keV]");
  hResponseMatrix->SetYTitle("True Energy [keV]");
  hResponseMatrix->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");

  TCanvas* c_MeasAndTrue = new TCanvas("c_MeasAndTrue", "c_MeasAndTrue");
  TH1D* hTrue = hResponseMatrix->ProjectionY("hTrue");
  hTrue->SetLineColor(kMagenta);
  hTrue->SetLineWidth(2);
  hTrue->SetStats(false);
  TH1D* hMeas = hResponseMatrix->ProjectionX("hMeas");
  hMeas->SetLineColor(kCyan);
  hMeas->SetLineWidth(2);
  hMeas->SetStats(false);

  int rebin_factor = 5;
  hMeas->Rebin(rebin_factor);
  hTrue->Rebin(rebin_factor);

  hMeas->Draw("HIST");
  hTrue->Draw("HIST SAME");

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hMeas, "MC Meas", "l");
  leg->AddEntry(hTrue, "MC Truth", "l");
  leg->Draw();
}
