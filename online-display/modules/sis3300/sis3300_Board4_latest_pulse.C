void sis3300_Board4_latest_pulse()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "LatestPulse";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {
    "SIS3300_B4C1", "SIS3300_B4C2","SIS3300_B4C3","SIS3300_B4C4", "SIS3300_B4C5","SIS3300_B4C6","SIS3300_B4C7","SIS3300_B4C8"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw();
    }
  }
}
