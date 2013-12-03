void fadc_81_times()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  gROOT->ProcessLine(".L modules/common/get_histogram.C+");
  /*****************************************************************/
  std::string hist_type = "Times";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {"Na81", "Nb81", "Nc81", "Nd81", "Ne81", "Nf81", "Ng81", "Nh81"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw();
    }
  }
}
