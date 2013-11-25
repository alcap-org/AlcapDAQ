void thin_right_fast_silicon_times()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);

  gROOT->ProcessLine(".L modules/common/get_histogram.C+");
  /*****************************************************************/
  std::string hist_type = "Times";
  const int n_detectors = 4;
  std::string det_names[n_detectors] = {"SiR1-1-fast", "SiR1-2-fast", "SiR1-3-fast", "SiR1-4-fast"};

  for (int iDet = 0; iDet < n_detectors; iDet++) {
    TH1* hist = get_histogram(det_names[iDet], hist_type);
    if (hist) {
      AlCapCanvas->cd(iDet+1);
      hist->Draw();
    }
  }
}
