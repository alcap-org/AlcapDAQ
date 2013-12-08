#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"

void plot_histo(const int run_nr, const char *hname) 
{
  
  TH1 *h;

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  //gStyle->SetOptStat(11);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,600);
  c1->Draw();
  c1->cd();

  //=========================================================
  //          *** receive and plot histograms ***
  //=========================================================
  
  h = (TH1*) get_hist(hname, run_nr);

  if ( h ) {

    if ( h->InheritsFrom("TH2") ) {
      c1->SetRightMargin(0.1);
      h->Draw("ColZ");
    } else{
      h->SetLineColor(kBlue);
      h->SetLineWidth(2);
      h->SetFillColor(kYellow);
      h->Draw();
    }
    
    c1->Update();

    TPaletteAxis *palette = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");
    if ( palette ) {
      palette->SetX2NDC( palette->GetX1NDC()+0.01 );
      palette->Draw();
    }

  }
  
  c1->SaveAs("../png/plot_histo.png");


}
