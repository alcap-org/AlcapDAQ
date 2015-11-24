void sis3300_Board1_shapes()
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
  const int n_channels = 8;
  std::string bank_names[n_channels] = {
    "SIS3300_B1C1", "SIS3300_B1C2","SIS3300_B1C3","SIS3300_B1C4", "SIS3300_B1C5","SIS3300_B1C6","SIS3300_B1C7","SIS3300_B1C8"};

  for (int iChn = 0; iChn < n_channels; iChn++) {
    //TH1* hist = get_histogram(bank_names[iChn], hist_type);
    TH1* hist =  (TH1*) getObject(Form("h2_pulses_SIS3300_B1C%i",iChn+1));
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->Draw("COLZ");
      gPad->SetLogz();
    }
  }
}
