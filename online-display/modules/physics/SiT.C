void SiT()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,1);
  
  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/

  double mu_thr = 110.0; // muon threshold, ch;
  double mu_cutoff = 600.0; // muon upper threshold (cutoff), ch;

  AlCapCanvas->cd(1);

  TPad *pad_left = gPad;
  gPad->Divide(1,2);
  pad_left->cd(1);
  

  TLegend *leg = new TLegend(0.8,0.8,0.99,0.99);

  const int n_channels = 4;
  int colors[n_channels] = {kRed, kBlue, kGreen, kMagenta};
  TH1D *h1_S[n_channels];

  // Number of muons
  double Nmu[n_channels];

  TH1D *h0 = NULL;
  unsigned int i=0;
  double max = 0;
  double N_mu_tot = 0.0;

  TText *text;
	     
  for (unsigned int ich=1; ich<=4; ++ich)
    {      
      h1_S[i] = (TH1D*)get_histogram( Form("SIS3300_B3C%i", ich), "Heights" );
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
      h0->SetMaximum( 1.1*max );
      h0->GetXaxis()->SetRangeUser(0.0,500.0);
     }

  leg->Draw();

  // draw muon threhold
  TGraph *gr_thr = new TGraph();
  gr_thr->SetPoint(0,mu_thr,0.0);
  gr_thr->SetPoint(1,mu_thr,1.1*max);

  gr_thr->SetLineStyle(2);
  gr_thr->SetLineColor( kBlack );
  gr_thr->Draw("L");

  // draw muon cutoff
  TGraph *gr_cutoff = new TGraph();
  gr_cutoff->SetPoint(0,mu_cutoff,0.0);
  gr_cutoff->SetPoint(1,mu_cutoff,1.1*max);

  gr_cutoff->SetLineStyle(2);
  gr_cutoff->SetLineColor( kBlack );
  gr_cutoff->Draw("L");

  // TDC data
  TH1D *h1_TDC = (TH1D*) getObject( "h1_M1290_hits" );
  TH1D *h1_TDC_10 = (TH1D*) getObject( "h1_M1290_hits_10" );
  //if ( ! h1_TDC ) return;
  //AlCapCanvas->cd(2);
  pad_left->cd(1);
  //h1_TDC->Draw();
  
  // Number of protons
  double Np = h1_TDC->GetBinContent( 7 ) * 10; // 10 - prescale factor 
  AlCapCanvas->cd(2);
  TPaveText *pt = new TPaveText(0.1,0.1,0.9,0.9,"NDC");

  double muSc_Hi = h1_TDC->GetBinContent( 3 );
  double muSc_Lo = h1_TDC->GetBinContent( 8 );

  // convert counts to rates
  int Nblocks = h1_TDC->GetBinContent( 65 );
  double block_length = 0.098; // sec
  double Ip = Np / (Nblocks * block_length);
  for (unsigned int i=0; i<n_channels; ++i)
    {
      // muon rate will be in kHz now      
      Nmu[i] = Nmu[i] / (Nblocks * block_length) / 1000.0; 
    }
  pt->AddText( Form("Ip = %.2f mA",Ip/1.0e5) );    
  pt->AddText( Form("Np = %.2e",Np) );    

  muSc_Hi /= (Nblocks * block_length) / 1000.0;
  muSc_Lo /= (Nblocks * block_length) / 1000.0;
  
  pt->AddText( Form("f_{#muScHi} = %.2f kHz",muSc_Hi) );    
  pt->AddText( Form("f_{#muScLo} = %.2f kHz",muSc_Lo) );    

  double N_mu_tot_rate = 0.0;
  for (unsigned int i=0; i<n_channels; ++i)
    {
      text = pt->AddText( Form("f_{#mu} = %.2f kHz",Nmu[i]) );
      text->SetTextColor( colors[i] );
      N_mu_tot_rate += Nmu[i];
    }
  pt->AddText( Form("f_{#mu} = %.2f kHz",N_mu_tot_rate) );    

  // normalize on 2.10 mA p current
  double N_mu_tot_rate_norm = N_mu_tot_rate/Ip*2.10*1.0e5;
  pt->AddText( Form("f_{#mu,norm} = %.2f kHz",N_mu_tot_rate_norm) );    


  pt->AddText( Form("N_{#mu} = %.2e ",N_mu_tot) );    


  // Number of analyzed blocks
  pt->AddText( Form("Nblocks = %i",Nblocks) );


  // Hits
  pad_left->cd(2);
  TH2D *h2 = new TH2D("h2_S_hits","muons in SiT (beam view)",2,0.5,2.5, 2,0.5,2.5);
  h2->SetBinContent(1,2,Nmu[0]);
  h2->SetBinContent(1,1,Nmu[1]);
  h2->SetBinContent(2,1,Nmu[2]);
  h2->SetBinContent(2,2,Nmu[3]);
  h2->Draw("BoxText");
  h2->SetMarkerSize(2.6);

  // Beam position 
  TH1D *h1_x = h2->ProjectionX("h1_SiT_x");
  double x = h1_x->GetMean();
  TH1D *h1_y = h2->ProjectionY("h1_SiT_y");
  double y = h1_y->GetMean();

  pt->AddText( Form("<x> = %.2f",x) );
  pt->AddText( Form("<y> = %.2f",y) );



  AlCapCanvas->cd(2);  
  pt->Draw();


}
