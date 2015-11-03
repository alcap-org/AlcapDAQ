void sis3301_b04_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Heights";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {
    "SIS3301_B04C00", "SIS3301_B04C01","SIS3301_B04C02","SIS3301_B04C03","SIS3301_B04C04", "SIS3301_B04C05","SIS3301_B04C06","SIS3301_B04C07"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw();
    }
  }
}
