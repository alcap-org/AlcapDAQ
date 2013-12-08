
#include <fstream>

#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"
#include "../common/wfd.h"

void afterpulses(const int run_nr = 0) 
{

  int i;
  const int n_crates = WFD_NUMBER_OF_CRATES;
  char name[256];
  
  TH2D *h2[n_crates];

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);
  gStyle->SetPadGridY(1);

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
  
  for (i=0; i<n_crates; i++) {

    sprintf(name,"h2_block_mult_%s",wfd_crate_name[i]);
    h2[i] = (TH2D*) get_hist(name, run_nr);

    if ( !h2[i] ) continue;

    int ipad = i+1;   
    c1->cd(ipad);

    h2[i]->Draw("ColZ");
    //h2[i]->SetXTitle("defnan area");
    //h2[i]->SetYTitle("slot");
    c1->GetPad(ipad)->Update();
    

    TPaletteAxis *palette = (TPaletteAxis*)h2[i]->GetListOfFunctions()->FindObject("palette");
    if ( palette ) {
      palette->SetX2NDC( palette->GetX1NDC()+0.01 );
      palette->Draw();
    }

    if ( h2[i]->GetMaximum() > 10 ) {
      h2[i]->SetMinimum( 0.9 );
      c1->GetPad(ipad)->SetLogz();
    }

  }
  
  c1->SaveAs("../png/afterpulses.png");

}
