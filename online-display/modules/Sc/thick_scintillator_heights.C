void thick_scintillator_heights()
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
  const int n_detectors = 5;
  std::string det_names[n_detectors] = {"muSc", "muSc-A", "ScL", "ScR", "ScGe"};

  for (int iDet = 0; iDet < n_detectors; iDet++) {
    TH1* hist = get_histogram(det_names[iDet], hist_type);
    if (hist) {
      AlCapCanvas->cd(iDet+1);
      hist->Draw();
    }
  }
}
