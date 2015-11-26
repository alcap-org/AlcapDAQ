void entrance_counter()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  
  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/

 
  // TDC data
  TH1D *h1_TDC = (TH1D*) getObject( "h1_M1290_hits" );
  // Number of protons
  double Np = h1_TDC->GetBinContent( 7 ) * 10; // 10 - prescale factor 
  TPaveText *pt = new TPaveText(0.1,0.1,0.9,0.9,"NDC");

  double muSc_Hi = h1_TDC->GetBinContent( 3 );
  double muSc_Lo = h1_TDC->GetBinContent( 9 );

  // convert counts to rates
  int Nblocks = h1_TDC->GetBinContent( 65 );
  double block_length = 0.098; // sec
  double Ip = Np / (Nblocks * block_length);

  pt->AddText( Form("Ip = %.2f mA",Ip/1.0e5) );    
  pt->AddText( Form("Np = %.2e",Np) );    

  muSc_Hi /= (Nblocks * block_length) * 1000.0;
  muSc_Lo /= (Nblocks * block_length) * 1000.0;
  
  pt->AddText( Form("f_{#muScHi} = %.2f kHz",muSc_Hi) );    
  pt->AddText( Form("f_{#muScLo} = %.2f kHz",muSc_Lo) );    

  // normalize on 2.10 mA p current
  double N_mu_tot_rate_norm = muSc_Hi/Ip*2.10*1.0e5;
  pt->AddText( Form("f_{#mu,norm} = %.2f kHz",N_mu_tot_rate_norm) );    

  // Number of analyzed blocks
  pt->AddText( Form("Nblocks = %i",Nblocks) );

  pt->Draw();

}
