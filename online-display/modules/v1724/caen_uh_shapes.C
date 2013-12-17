void caen_uh_shapes()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,1);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Shapes";
  const int n_channels = 2;
  //std::string bank_names[n_channels] = {"CaUH", "CbUH", "CcUH", "CdUH", "CeUH", "CfUH", "CgUH", "ChUH"};
  std::string bank_names[n_channels] = {"CaUH", "CbUH"};//, "CbUH", "CcUH", "CdUH", "CeUH", "CfUH", "CgUH", "ChUH"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      TVirtualPad *pad = AlCapCanvas->cd(iChn+1);
      //AlCapCanvas->cd(0);
      hist->Draw("COLZ");
      pad->SetLogz();
      pad->SetLogy(0);
      pad->SetLogx(0);
    }
  }
}
