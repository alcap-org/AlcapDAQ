void thin_right_slow_silicon_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);

  gROOT->ProcessLine(".L modules/common/get_histogram.C+");
  /*****************************************************************/
  std::string hist_type = "Heights";
  const int n_detectors = 4;
  std::string det_names[n_detectors] = {"SiR1-1-slow", "SiR1-2-slow", "SiR1-3-slow", "SiR1-4-slow"};

  for (int iDet = 0; iDet < n_detectors; iDet++) {
    TH1* hist = get_histogram(det_names[iDet], hist_type);
    if (hist) {
      AlCapCanvas->cd(iDet+1);
      hist->Draw();
    }
  }
}
