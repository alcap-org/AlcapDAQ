void dt5720_adcSum()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "adcSum";
  const int n_channels = 4;
  std::string bank_names[n_channels] = {"D500", "D501", "D502", "D503"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw();
    }
  }
}
