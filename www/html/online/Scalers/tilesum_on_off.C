
#include "TVirtualPad.h"
#include "TPaveText.h"

#include "../common/init.C"
#include "../common/gethist.C"


void tilesum_on_off(const int run_nr = 0) 
{

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  //gStyle->SetOptStat(11);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,1000);
  c1->Draw();
  c1->cd();

  c1->Divide(1,3,0.001,0.001,10);

  //=========================================================
  //             *** retrive histograms ***
  //=========================================================

  TH1D *h1_sc_tilesum     = (TH1D*) get_hist("sc_tilesum"    , run_nr);
  TH1D *h1_sc_tilesum_off = (TH1D*) get_hist("sc_tilesum_off", run_nr);


  //=========================================================
  //         *** convert scaler to delta_scaler ***
  //=========================================================


  TH1D *h1_tilesum     = new TH1D("h1_tilesum"    ,"Tilesum, Beam on",    1000, 0.5, 1.005e6);
  TH1D *h1_tilesum_off = new TH1D("h1_tilesum_off","Tilesum, Beam off",1000, 0.5, 1.0005e3);
  TH1D *h1_ratio       = new TH1D("h1_ratio", "Tilesum beam on/off ratio",300, 0.5, 1.05e3);

  
  
  TVirtualPad *pad;
  if ( h1_tilesum && h1_tilesum_off ) {

    int i;
    for (i=2; i<=h1_sc_tilesum->GetNbinsX(); i++) {
      Double_t dY = h1_sc_tilesum->GetBinContent(i) - h1_sc_tilesum->GetBinContent(i-1);
      if ( dY > 0)
	h1_tilesum->Fill(dY);

      Double_t dY_off = h1_sc_tilesum_off->GetBinContent(i) - h1_sc_tilesum_off->GetBinContent(i-1);
      if ( dY_off > 0.0 ) 
	h1_tilesum_off->Fill(dY_off);

      Double_t x = 0;
      if ( dY_off > 0.0 ) {
	x = dY / dY_off;
      }
      if ( x > 0 )
	h1_ratio->Fill(x);

    }

    for (i=2; i<=h1_sc_tilesum_off->GetNbinsX(); i++) {
      Double_t dY = h1_sc_tilesum_off->GetBinContent(i) - h1_sc_tilesum_off->GetBinContent(i-1);
      h1_tilesum_off->Fill(dY);
    }

    pad = c1->GetPad(1);
    pad->cd();
    h1_sc_tilesum->Draw();
    h1_tilesum->SetLineColor(kBlue);
    h1_tilesum->SetLineWidth(2);
    if ( h1_tilesum->GetMaximum() > 50 ) 
      pad->SetLogy();

    pad->Update();

    pad = c1->GetPad(2);
    pad->cd();
    h1_tilesum_off->Draw();
    h1_tilesum_off->SetLineColor(kBlue);
    h1_tilesum_off->SetLineWidth(2);
    if ( h1_tilesum_off->GetMaximum() > 50 ) 
      pad->SetLogy();
    
    pad = c1->GetPad(3);
    pad->cd();
    h1_ratio->Draw();
    h1_ratio->SetLineColor(kBlue);
    h1_ratio->SetLineWidth(2);
    if ( h1_ratio->GetMaximum() > 50 ) 
      pad->SetLogy();
    

  }



  c1->SaveAs("../png/tilesum_on_off.png");


}
