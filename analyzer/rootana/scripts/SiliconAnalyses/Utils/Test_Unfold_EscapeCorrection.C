void Test_Unfold_EscapeCorrection() {

  TFile* data_file = new TFile("~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test.root", "READ");
  std::string data_histname = "DecayElectron_TimeSlice2000_4000_allRecoil/hCorrectedSpectrum";
  TH1F* hData = (TH1F*) data_file->Get(data_histname.c_str());
  hData->Rebin(2);
  
  TFile* mc_file = new TFile("~/data/mc/SiL3/combinedCorr_1M_Geom-P1_combined-flat_Thresh0-1MeV__MeV.root", "READ");
  std::string respname = "SiL_middle_layer_response";
  RooUnfoldResponse* resp = (RooUnfoldResponse*) mc_file->Get(respname.c_str());
  TH2D* hResp = (TH2D*) resp->Hresponse();

  TF1* truth = new TF1("truth", "[0]*TMath::Exp(-x/[1])", 3, 30);
  truth->SetParameters(1e7, 4.6);
  //  TH1F* hTruth = (TH1F*) truth->GetHistogram();

  TVectorF truthVec(hData->GetNbinsX());
  for (int i_bin = 1; i_bin <= hData->GetNbinsX(); ++i_bin) {
    double E = hData->GetBinCenter(i_bin);
    truthVec[i_bin-1] = truth->Eval(E);
    std::cout << "Truth = " << truthVec[i_bin-1] << std::endl;
  }

  //  std::cout << hResp->GetNbinsX() << " " << hResp->GetNbinsY() << std::endl;
  TMatrixF respMatrix(hResp->GetNbinsX(), hResp->GetNbinsY());
  for (int i_bin = 0; i_bin < hResp->GetNbinsX(); ++i_bin) {
    for (int j_bin = 0; j_bin < hResp->GetNbinsY(); ++j_bin) {
      //      std::cout << hResp->GetBinContent(i_bin, j_bin) << std::endl;
      respMatrix(i_bin, j_bin) = hResp->GetBinContent(i_bin+1, j_bin+1)/hResp->GetEntries();
      //      std::cout << "Resp = " << respMatrix(i_bin, j_bin) << std::endl;
    }
  }
  
  TVectorF fold = respMatrix * truthVec;
  TH1F* hFold = (TH1F*) hData->Clone("hFold");
  hFold->Reset();
  for (int i_bin = 0; i_bin < hData->GetNbinsX(); ++i_bin) {
    //    std::cout << fold[i_bin] << std::endl;
    hFold->SetBinContent(i_bin+1, fold[i_bin]);
  }
  hFold->SetLineColor(kRed);
  
  TCanvas* c = new TCanvas();
  c->SetLogy();
  hData->Draw();
  hFold->Draw("SAME");
}
