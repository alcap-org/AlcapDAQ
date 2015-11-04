void sis3350_shapes()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Shapes";
  const int n_boards = 2;
  const int n_channels = 4;
  std::string bank_names[n_boards*n_channels] = {"SIS3350_B1C1", "SIS3350_B1C2","SIS3350_B1C3","SIS3350_B1C4",
						 "SIS3350_B2C1","SIS3350_B2C2","SIS3350_B2C3","SIS3350_B2C4"};

  for (int iChn = 0; iChn < n_channels*n_boards; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw("COLZ");
    }
  }
}
