void caen_uh_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(1,2);

  // gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Heights";
  const int n_channels = 2;
  //std::string bank_names[n_channels] = {"CaUH", "CbUH", "CcUH", "CdUH", "CeUH", "CfUH", "CgUH", "ChUH"};
  std::string bank_names[n_channels] = {"CaUH", "CbUH"};

  for (int iChn = 0; iChn < n_channels; iChn++) 
    {
      TH1* hist = get_histogram(bank_names[iChn], hist_type);
      if (hist) 
	{
	  //AlCapCanvas->cd(iChn+1);
	  TVirtualPad *pad = AlCapCanvas->cd(iChn+1);
	  hist->Draw();
	  pad->SetLogy(1);
	  pad->SetLogx(0);
	}
    }
}
