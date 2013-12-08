
#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"
#include "../common/wfd.h"

void wfd_delta_fill_1d(const int run_nr = 0) 
{
  int i;
  char name[256];

  const int n_crates = WFD_NUMBER_OF_CRATES;
  
  TH2D *h2[n_crates];
  TH1D *h1[n_crates];

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
  
  for (i=0; i<n_crates; i++) {

    sprintf(name,"WFDRawDeltaFill%s",wfd_crate_name[i]);
    h2[i] = (TH2D*) get_hist(name, run_nr);
    h1[i] = NULL;

    if ( !h2[i] ) continue;

    h1[i] = h2[i]->ProjectionX();

    int ipad = i+1;   
    c1->cd(ipad);

    h1[i]->Draw();
    if ( h1[i]->GetMaximum() > 10.0 )
      c1->GetPad(ipad)->SetLogy();

    h1[i]->SetXTitle("last_fill_number - first_fill_number");
    h1[i]->SetYTitle("counts");
    //h1[i]->GetXaxis()->SetRangeUser(0.0,2000.0);
    h1[i]->GetXaxis()->SetRangeUser(900.0,1100.0);
    h1[i]->SetLineColor(kBlue);
    h1[i]->SetLineWidth(2);

    c1->GetPad(ipad)->Update();

    TPaletteAxis *palette = (TPaletteAxis*)h2[i]->GetListOfFunctions()->FindObject("palette");
    if ( palette ) {
      palette->SetX2NDC( palette->GetX1NDC()+0.01 );
      palette->Draw();
    }

  }
  
  c1->SaveAs("../png/wfd_delta_fill_1d.png");


}
