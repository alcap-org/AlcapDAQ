void sis3301_Board6_times()
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
  const int n_channels = 8;
  std::string bank_names[n_channels] = {
    "SIS3301_B6C1", "SIS3301_B6C2","SIS3301_B6C3","SIS3301_B6C4", "SIS3301_B6C5","SIS3301_B6C6","SIS3301_B6C7","SIS3301_B6C8"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->GetXaxis()->SetRangeUser(0., 1.2E8);
      hist->Draw();

      TLine* line = new TLine(1.12E8, 0, 1.12E8, hist->GetMaximum());
      line->SetLineColor(kRed);
      line->SetLineWidth(2);
      line->Draw("LSAME");
    }
  }
}
