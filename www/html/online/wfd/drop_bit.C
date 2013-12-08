/*
  macro: drop_bit.C
  it was my attempt to monitor the "drop bit problem",
  unsuccessful, although;
*/

#include "../common/init.C"
#include "../common/gethist.C"

void drop_bit(const int run_nr = 0,
	      const int crate = 1,
	      const int slot1 = 2,
	      const int slot2 = 2) 
{
  char name[256];

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================

  int nslots = slot2 - slot1 + 1;
  
  TCanvas *c1;
  if (nslots == 1) 
      c1 = new TCanvas("c1","c1",0,0,2400,600);    
  else
    c1 = new TCanvas("c1","c1",0,0,800,200*nslots);
  c1->Draw();
  c1->cd();

  c1->Divide(4,nslots,0.001,0.001,10);

  //=========================================================
  //          *** receive and plot histograms ***
  //=========================================================

  int ipad = 0;
  int slot, chan;
  for (slot=slot1; slot<=slot2; slot++) {

    for (chan=0; chan<4; chan++) {

      ipad++;

      sprintf( name, "h2_pulse_c_%1d_s_%02d_ch_%1d",crate,slot,chan );
      TH2D *h2 = (TH2D*) get_hist(name, run_nr);
      
      if ( !h2 ) continue;

      sprintf(name, "h2_ADC_c_%1d_s_%02d_ch_%1d",crate,slot,chan);
      TH1D *h1 = h2->ProjectionY();
      h1->SetName( name );
      h1->SetLineColor(kBlue);
      h1->SetLineWidth(2);
      h1->SetXTitle("ADC");
      h1->SetYTitle("counts");
      h1->SetFillColor(kYellow);

      c1->cd(ipad);

      h1->Draw();
      //c1->GetPad(ipad)->Update();
      //h1->GetXaxis()->SetRangeUser(0,100);

      /*** build smooth histogram ***/
      TH1D *h1_smooth = new TH1D( *h1 );
      sprintf(name, "h2_ADC_c_%1d_s_%02d_ch_%1d_smooth",crate,slot,chan);
      h1_smooth->SetName( name );
      h1_smooth->Reset();
      h1_smooth->SetLineColor(kRed);
      h1_smooth->SetLineWidth(1);
      h1_smooth->SetFillStyle(0);

      int i;
      for (i=2; i<h1->GetNbinsX(); i++) {
	Double_t y1 = h1->GetBinContent(i-1);
 	Double_t y2 = h1->GetBinContent(i  );
 	Double_t y3 = h1->GetBinContent(i+1);

	Double_t y_sm = (y1+(y1+y3)*0.1)/1.2;
	h1_smooth->SetBinContent(i,y_sm);
      }
      h1_smooth->Draw("Same");
      
      /*** histogram difference ***/
      TH1D *h1_diff = new TH1D( *h1 );
      sprintf(name, "h2_ADC_c_%1d_s_%02d_ch_%1d_diff",crate,slot,chan);
      h1_diff->SetName( name );
      h1_diff->Add( h1_smooth, -1.0 );

      h1_diff->Draw();

      

      //if ( h1->GetMaximum() > 100 ) {
      //c1->GetPad(ipad)->SetLogy();
      //}

    }
  }

  //c1->SaveAs("../png/wfd_ADC.png");

}
