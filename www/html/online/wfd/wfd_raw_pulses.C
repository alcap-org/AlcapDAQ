

#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"

void wfd_raw_pulses(const int run_nr = 0,
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

      sprintf( name, "h2_pulse_c_%1d_s_%02d_ch_%1d",crate,slot,chan);
      TH2D *h2 = (TH2D*) get_hist(name, run_nr);
      
      if ( !h2 ) continue;

      c1->cd(ipad);

      h2->Draw("ColZ");
      h2->SetXTitle("clock tick");
      h2->SetYTitle("ADC");
      c1->GetPad(ipad)->Update();

      TPaletteAxis *palette = (TPaletteAxis*)h2->GetListOfFunctions()->FindObject("palette");
      if ( palette ) {
	palette->SetX2NDC( palette->GetX1NDC()+0.01 );
	palette->Draw();
      }

      if ( h2->GetMaximum() > 100 ) {
	c1->GetPad(ipad)->SetLogz();
      }

    }
  }

  c1->SaveAs("../png/wfd_raw_pulses.png");

}
