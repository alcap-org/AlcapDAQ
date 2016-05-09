void SiT()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  
  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/

  double mu_thr = 60.0; // muon threshold, ch;
  double mu_cutoff = 250.0; // muon upper threshold (cutoff), ch;

  AlCapCanvas->cd(1);
 

  TLegend *leg = new TLegend(0.8,0.8,0.99,0.99);

  const int n_channels = 4;
  int colors[n_channels] = {kRed, kBlue, kGreen, kMagenta};
  TH1D *h1_S[n_channels];
  TH1D *h1_F[n_channels];

  // Number of muons
  double Nmu[n_channels];

  TH1D *h0 = NULL;
  TH1D *h0f = NULL;
  unsigned int i=0;
  double max = 0;
  double N_mu_tot = 0.0;

  TText *text;
	     
  for (unsigned int ich=1; ich<=4; ++ich)
    {      
      h1_S[i] = (TH1D*)get_histogram( Form("SIS3300_B1C%i", ich), "Heights" );
      if ( !h1_S[i] ) continue;
      
      if ( ! h0 ) 
	{
	  h0 = h1_S[i];
	  h0->Draw();
	}
      h1_S[i]->SetLineColor( colors[i] );
      h1_S[i]->SetLineWidth( 2 );
      h1_S[i]->Draw("Same");
      
      Int_t thr_bin = h1_S[i]->GetXaxis()->FindBin( mu_thr );
      Int_t cutoff_bin = h1_S[i]->GetXaxis()->FindBin( mu_cutoff );
      Int_t nbins = h1_S[i]->GetNbinsX();
      if ( cutoff_bin >= nbins ) cutoff_bin = nbins-1;
      
      Nmu[i] = h1_S[i]->Integral(thr_bin,cutoff_bin);
      N_mu_tot += Nmu[i];
      
      
      if ( h1_S[i]->GetMaximum() > max ) max = h1_S[i]->GetMaximum();
      
      leg->AddEntry( h1_S[i], Form("ch %i",ich+1), "L");
      
      i++;
    }
  
  if ( h0 && max > 0 )
    {
      h0->SetMaximum( max );
      h0->GetXaxis()->SetRangeUser(0.0,300.0);
      h0->SetTitle("beam SiT-Heights, Slow");
      gPad->SetLogy();
    }
  
  leg->Draw();
  
  // draw muon threhold
  TGraph *gr_thr = new TGraph();
  gr_thr->SetPoint(0,mu_thr,0.0);
  gr_thr->SetPoint(1,mu_thr,1.*max);
  
  gr_thr->SetLineStyle(2);
  gr_thr->SetLineColor( kBlack );
  gr_thr->Draw("L");
  
  // draw muon cutoff
  TGraph *gr_cutoff = new TGraph();
  gr_cutoff->SetPoint(0,mu_cutoff,0.0);
  gr_cutoff->SetPoint(1,mu_cutoff,1.*max);
  
  gr_cutoff->SetLineStyle(2);
  gr_cutoff->SetLineColor( kBlack );
  gr_cutoff->Draw("L");
  
  // Hits
  AlCapCanvas->cd(4);
  TH2D *h2 = new TH2D("h2_S_hits","muons in SiT (beam view)",2,0.5,2.5, 2,0.5,2.5);
  h2->SetBinContent(1,2,Nmu[0]);
  h2->SetBinContent(1,1,Nmu[1]);
  h2->SetBinContent(2,1,Nmu[2]);
  h2->SetBinContent(2,2,Nmu[3]);
  h2->SetMarkerSize(2.6);
  h2->Draw("BoxText");
  
  AlCapCanvas->cd(2);
  for (unsigned int ich=1; ich<=4; ++ich)
    {      
      int ii=ich-1;
      h1_S[ii] = (TH1D*)get_histogram( Form("SIS3300_B1C%d", ich), "Times" );
      h1_S[ii]->SetLineColor( colors[ii] );
      h1_S[ii]->SetLineWidth( 1 );
      h1_S[ii]->SetTitle("beam SiT-Times, Slow");

      if(ich == 1) {
	h1_S[ii]->Draw("");
      }
      else {
	h1_S[ii]->Draw("same");
      }
    }  


  
  AlCapCanvas->cd(3);
  std::string bank_names[n_channels] = {"D500", "D501", "D502", "D503"};
  for (int iChn = 0; iChn < n_channels; iChn++) {
    TH1* hist = get_histogram(bank_names[iChn], "Heights");
    if (hist) {
      if(iChn) {
	hist->DrawClone("same");
      }
      else {
	hist->DrawClone("");
      }
    }
  }



}
