
#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"
#include "../common/wfd.h"

void wfd_coinc_vs_fill_1d_sng(const int run_nr = 0,
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

  sprintf( name, "WFDRawCoincVsFill%s",wfd_crate_name[crate-1]);  
  TH2D *h2 = (TH2D*) get_hist(name, run_nr);
  
  for (slot=slot1; slot<=slot2; slot++) {

    for (chan=0; chan<4; chan++) {
      
      ipad++;

      sprintf( name, "WFDRawCoincVsFill%s_slot_%02d_ch_%1d",wfd_crate_name[crate-1],slot,chan); 
      int bin = h2->GetYaxis()->FindBin( 1.625 + 0.25*chan + slot - 2 );

      TH1D *h1 = h2->ProjectionX(name,bin,bin);

      sprintf(name,"crate %d slot %02d chan. %d", crate, slot, chan);
      h1->SetTitle(name);
    
      c1->cd(ipad);

      h1->Draw();

      //h1->GetXaxis()->SetRangeUser(0.0,3000.0);
      //h1->GetXaxis()->SetRangeUser(950.0,1050.0);
      h1->GetXaxis()->SetRangeUser(0.0,1010.0);
      h1->SetLineColor(kBlue);
      //h1->SetFillColor(kYellow);
      h1->SetLineWidth(2);

      h1->SetXTitle("fill number");
      h1->SetYTitle("number of coincidences");
    
      /*
      if ( h1->GetMaximum() > 100 ) {
	h1->SetMinimum(0.9);
	c1->GetPad(ipad)->SetLogy();
      }
      */

      //if ( h2[i]->GetMaximum() > 10 )
      //c1->GetPad(ipad)->SetLogz();

    }

  }

  c1->SaveAs("../png/wfd_coinc_vs_fill_1d_sng.png");


}
