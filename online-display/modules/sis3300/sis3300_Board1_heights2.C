void sis3300_Board1_heights2()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  std::string hist_type = "Heights";
  const int n_channels = 8;
  std::string bank_names[n_channels] = {
    "SIS3300_B1C1", "SIS3300_B1C2","SIS3300_B1C3","SIS3300_B1C4","SIS3300_B1C5", "SIS3300_B1C6","SIS3300_B1C7","SIS3300_B1C8"}

  int colors[n_channels] = {kRed, kBlue, kGreen, kMagenta, kCyan, kBlack, kOrange, kGray};

  AlCapCanvas->Divide(1,2);

  // counts
  TH1D *h1_N = new TH1D("h1_N","counts",8,0.5,8.5);
  h1_N->SetXTitle("Channel Nr");
  h1_N->SetLineWidth(3);

  AlCapCanvas->cd(1);

  gPad->SetLogy();

  double max = 0;
  TH1 *h0 = NULL;

  TLegend *leg = new TLegend(0.8,0.8,0.99,0.99);
  
  for (int iChn = 0; iChn < n_channels; iChn++) 
    {
      TH1* hist = get_histogram(bank_names[iChn], hist_type);
      if (hist) 
	{
	  if ( !h0 )
	    {
	      h0 = hist;
	      h0->Draw();
	    }
	  hist->Draw("Same");
	  hist->SetLineColor( colors[iChn] );
	  hist->GetXaxis()->SetRangeUser(0.0,3000.0);
	  if ( hist->GetMaximum() > max ) max = hist->GetMaximum();
	  leg->AddEntry( hist, Form("ch %i",iChn+1), "L");
	  h1_N->SetBinContent( iChn+1, hist->Integral());
	}
    }
  if ( h0 && max > 0 )
    {
      h0->SetMaximum( max );
    }

  leg->Draw();

  AlCapCanvas->cd(2);
  h1_N->Draw();
}
