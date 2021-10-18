void Si16b_FinalPlot_DeadLayerCorrection() {

  TCanvas* c = new TCanvas();
  
  int rebin_factor = 5;

  std::string filename = "/data/R15bMC/respMatrix_100M_Si16b_proton_100keVBins.root";
  std::string responsename = "SiR_three_layer_response";

 
  TFile* file = new TFile(filename.c_str(), "READ");
  RooUnfoldResponse* response = (RooUnfoldResponse*) file->Get(responsename.c_str());
  TH1D* hMeasured = (TH1D*) response->Hmeasured();
  hMeasured->Rebin(rebin_factor);
  hMeasured->SetLineColor(kRed);
  hMeasured->Draw();  

  std::string noDL_filename = "~/g4sim/alcap/respMatrix_100M_Si16b_proton_100keVBins_incDLEDep.root";
  std::string noDL_responsename = "SiR_three_layer_response";

  TFile* noDL_file = new TFile(noDL_filename.c_str(), "READ");
  RooUnfoldResponse* noDL_response = (RooUnfoldResponse*) noDL_file->Get(noDL_responsename.c_str());
  TH1D* noDL_hMeasured = (TH1D*) noDL_response->Hmeasured();
  noDL_hMeasured->Rebin(rebin_factor);
  noDL_hMeasured->SetLineColor(kBlue);
  noDL_hMeasured->Draw("SAME");

  TCanvas* cR = new TCanvas();
  TH1D* hRatio = (TH1D*) noDL_hMeasured->Clone("hRatio");
  hRatio->Divide(hMeasured);
  hRatio->SetMinimum(0.90);
  hRatio->SetMaximum(1.10);
  hRatio->SetLineColor(kBlack);
  hRatio->Draw("HIST");
}

