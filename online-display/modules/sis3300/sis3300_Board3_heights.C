void sis3300_Board3_heights()
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
    "SIS3300_B3C1", "SIS3300_B3C2","SIS3300_B3C3","SIS3300_B3C4","SIS3300_B3C5", "SIS3300_B3C6","SIS3300_B3C7","SIS3300_B3C8"}
						 

  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw();
    }
  }
}
