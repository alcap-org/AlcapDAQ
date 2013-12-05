void fadc_projY(std::string fadc_id,std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas.
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L common/get_histogram.C"); // get_histogram() is called here
  /*****************************************************************/
  const int n_channels = 8;
  std::string bank_names[n_channels] = {"Na", "Nb", "Nc", "Nd", "Ne", "Nf", "Ng", "Nh"};
  std::string name;

  for (int iChn = 0; iChn < n_channels; iChn++) {
    name=bank_names[iChn]+fadc_id;
    TH2* hist = get_histogram_2d(name, hist_type);
    
    if (hist) {
      AlCapCanvas->cd(iChn+1);
      hist->ProjectionY()->Draw();
    }
  }
}
