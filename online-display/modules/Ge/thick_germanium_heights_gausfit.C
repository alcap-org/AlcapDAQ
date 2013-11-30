void thick_germanium_heights_gausfit()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,3);

  gROOT->ProcessLine(".L modules/common/get_histogram.C+");
  /*****************************************************************/
  std::string hist_type = "Heights";
  const int n_detectors = 2;
  std::string det_names[n_detectors] = {"Ge-fast", "Ge-slow"};
  double mean = 0;
  double rms = 0;

  gStyle->SetOptFit(1111);

  for (int iDet = 0; iDet < n_detectors; iDet++) {
    TH1* hist = get_histogram(det_names[iDet], hist_type);
    mean = hist->GetMean();
    rms = hist->GetRMS();

    AlCapCanvas->cd(iDet+1);

    if (hist->Integral()!=0) {
      TF1* gaus = new TF1("gaus","gaus",mean-rms,mean+rms);
      hist->Fit("gaus","Q");
      hist->GetXaxis()->SetRange(mean-3*rms,mean+3*rms);

      hist->Draw();
      gaus->Draw("same");
    }
  }
}
