
#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"

void wfd_raw_fill(const int run_nr = 0) 
{
  int i;

  const int n_hists = 6;
  const char *h_names[n_hists] = { "WFDRawFillR0",
				   "WFDRawFillR1",
				   "WFDRawFillG0",
				   "WFDRawFillG1",
				   "WFDRawFillB0",
				   "WFDRawFillB1" };
  
  TH2D *h2[n_hists];

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,1000);
  c1->Draw();
  c1->cd();

  c1->Divide(2,3,0.001,0.001,10);

  //=========================================================
  //          *** receive and plot histograms ***
  //=========================================================
  
  for (i=0; i<n_hists; i++) {

    h2[i] = (TH2D*) get_hist(h_names[i], run_nr);

    if ( !h2[i] ) continue;

    int ipad = i+1;   
    c1->cd(ipad);

    h2[i]->Draw("ColZ");
    
    if ( h2[i]->GetMaximum() > 100 ) {
      h2[i]->SetMinimum(0.4);
      c1->GetPad(ipad)->SetLogz();
    }

    c1->GetPad(ipad)->Update();
    h2[i]->SetXTitle("fill counter");
    h2[i]->SetYTitle("(slot-1)*4 + chan.");
    //h2[i]->GetXaxis()->SetRangeUser(0.0,2000.0);
    //h2[i]->GetXaxis()->SetRangeUser(-0.5,7000.0);
    //h2[i]->GetXaxis()->SetRangeUser(-0.5,50.0);
    h2[i]->GetXaxis()->SetRangeUser(4950.0,5050.0);

    TPaletteAxis *palette = (TPaletteAxis*)h2[i]->GetListOfFunctions()->FindObject("palette");
    if ( palette ) {
      palette->SetX2NDC( palette->GetX1NDC()+0.01 );
      palette->Draw();
    }

  }
  
  c1->SaveAs("../png/wfd_raw_fill.png");


}
