void sis3350_times()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Times";
  const int n_boards = 2;
  const int n_channels = 4;
  std::string bank_names[n_boards*n_channels] = {"S500", "S501", "S502", "S503", "S510", "S511", "S512", "S513"};

  for (int iChn = 0; iChn < n_channels*n_boards; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->GetXaxis()->SetRangeUser(0., 1.2E8);
      hist->Draw();
    }
  }
}
