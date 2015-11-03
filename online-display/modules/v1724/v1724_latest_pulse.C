void v1724_latest_pulse()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(3,3);

  /*****************************************************************/
  std::string hist_type = "LatestPulse";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {"D400", "D401", "D402", "D403", "D404", "D405", "D406", "D407"}

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      TVirtualPad *pad = AlCapCanvas->cd(iChn+1);
      //AlCapCanvas->cd(0);
      hist->Draw();
      pad->SetLogz();
      pad->SetLogy(0);
      pad->SetLogx(0);
    }
  }
}
