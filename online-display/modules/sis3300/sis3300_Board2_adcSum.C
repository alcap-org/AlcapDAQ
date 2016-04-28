void sis3300_Board2_adcSum()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "adcSum";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {
    "SIS3300_B2C1", "SIS3300_B2C2","SIS3300_B2C3","SIS3300_B2C4","SIS3300_B2C5", "SIS3300_B2C6","SIS3300_B2C7","SIS3300_B2C8"}
						 

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw();
    }
  }
}
