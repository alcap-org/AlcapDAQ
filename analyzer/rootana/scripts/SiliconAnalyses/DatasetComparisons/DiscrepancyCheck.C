void DiscrepancyCheck() {

  TFile* unfolded_file = new TFile("/data/ssd3/R15bAnalysis/hists/andy.unfolded.al50.oldrespmatrix.root", "READ");

  TH1F* hSiL = (TH1F*) unfolded_file->Get("hproton_SiL");
  hSiL->SetLineColor(kBlue);
  TH1F* hSiR = (TH1F*) unfolded_file->Get("hproton_SiR");
  hSiR->SetLineColor(kRed);

  TFile* combined_file = new TFile("/data/ssd3/R15bAnalysis/hists/final-al50-plots.root", "READ");
  TGraphErrors* grCombined = (TGraphErrors*) combined_file->Get("gCombined-al50-proton");
  grCombined->SetLineColor(kBlack);
  grCombined->SetMarkerColor(kBlack);

  grCombined->Draw("APE");
  hSiR->Draw("HIST E SAME");
  hSiL->Draw("HIST E SAME");
}
