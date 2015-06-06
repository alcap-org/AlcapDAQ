void v1724_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(3,3);

  // gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Heights";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {"D400", "D401", "D402", "D403", "D404", "D405", "D406", "D407"};

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
