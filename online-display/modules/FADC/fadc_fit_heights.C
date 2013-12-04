void fadc_fit_heights(std::string fadc_id,std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptFit(1111);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L common/get_histogram.C"); // get_histogram() is called here
  /*****************************************************************/
  const int n_channels = 8;
  std::string bank_names[n_channels] = {"Na", "Nb", "Nc", "Nd", "Ne", "Nf", "Ng", "Nh"};
  std::string name;

  double mean = 0;
  double rms = 0;

  for (int iChn = 0; iChn < n_channels; iChn++) {
    name=bank_names[iChn]+fadc_id;
    TH1* hist = get_histogram(name, hist_type);
    mean = hist->GetMean();
    rms = hist->GetRMS();

      AlCapCanvas->cd(iChn+1);

    if (hist->Integral()!=0) {
      TF1* gaus = new TF1("gaus","gaus",mean-rms,mean+rms);
      hist->Fit("gaus","Q");
      hist->GetXaxis()->SetRange(mean-2*rms,mean+2*rms);
      hist->Draw();
    }
  }
}

