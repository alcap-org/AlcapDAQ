
#include "TVirtualPad.h"
#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"
#include "../common/wfd.h"
#include "../common/bank2zip.C"
#include "../common/ZipCodeManipulation.cpp"


void wfd_raw_time_SN(const int run_nr = 0) 
{
  const int n_crates = WFD_NUMBER_OF_CRATES;
  TH2D *h2_raw_time  [n_crates];
  TH2D *h2_coinc_time[n_crates];
  char name[256], title[256];
  TPaletteAxis *palette;

  int i;
  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(0);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,800);
  c1->Draw();
  c1->cd();

  c1->Divide(2,1,0.001,0.001,10);

  //=========================================================
  //                  *** get histograms ***
  //=========================================================

  for (i=0; i<WFD_NUMBER_OF_CRATES; i++) {

    sprintf(name,"WFDRawTime%s",wfd_crate_name[i]);
    h2_raw_time[i] = (TH2D*) get_hist(name, run_nr);
    
    sprintf(name,"WFDCoincTime%s",wfd_crate_name[i]);
    h2_coinc_time[i] = (TH2D*) get_hist(name, run_nr);    
    
  }
  
  //=========================================================
  //           *** create time vs. SN matrixes ***
  //=========================================================

  int nbins_x =  500;
  float x_min =    0.0;
  float x_max = 1000.0;

  int nbins_y = 350;
  float y_min = 0.5;
  float y_max = 0.5 + nbins_y;
 
  if ( h2_raw_time[0] ) {
    nbins_x = h2_raw_time[0]->GetNbinsX();
    x_min = h2_raw_time[0]->GetXaxis()->GetXmin();
    x_max = h2_raw_time[0]->GetXaxis()->GetXmax();
  }

  TH2D *h2_raw_time_SN = new TH2D("h2_raw_time_SN","Raw time",nbins_x, x_min, x_max, nbins_y, y_min, y_max);
  h2_raw_time_SN->SetXTitle("time counter");
  h2_raw_time_SN->SetYTitle("SN");

  if ( h2_coinc_time[0] ) {
    nbins_x = h2_coinc_time[0]->GetNbinsX();
    x_min = h2_coinc_time[0]->GetXaxis()->GetXmin();
    x_max = h2_coinc_time[0]->GetXaxis()->GetXmax();
  }

  TH2D *h2_coinc_time_SN = new TH2D("h2_coinc_time_SN","Coincidence time",nbins_x, x_min, x_max, nbins_y, y_min, y_max);
  h2_coinc_time_SN->SetXTitle("time counter");
  h2_coinc_time_SN->SetYTitle("SN");

  
  //============================================================================
  //               *** Build raw  time vs. SN ***
  //============================================================================


  
  for (i=0; i<WFD_NUMBER_OF_CRATES; i++) {

    int j;
    for (j=1; j<=h2_raw_time[i]->GetNbinsY(); j++) {

      int y = int(h2_raw_time[i]->GetYaxis()->GetBinCenter(j));

      int crate = wfd_crate_number[i];
      int slot    = 2 + j/4;
      int channel = j - (slot-2)*4 - 1;
      
      int zip = bank2zip(crate, slot, channel, run_nr);

      if ( zip == 0 )
	continue;

      int SN = GetSerialNumber(zip);
      
      int ix;
      for (ix=1; ix<h2_raw_time[i]->GetNbinsX(); ix++) {
	h2_raw_time_SN->SetCellContent(ix,SN,h2_raw_time[i]->GetCellContent(ix,j));
      }
      
    }
    
  }

  c1->cd(1);
  h2_raw_time_SN->Draw("ColZ");
  h2_raw_time_SN->GetXaxis()->SetRangeUser(-0.5,10200.0);
  if ( h2_raw_time_SN->GetMaximum() > 100.0 )
    c1->GetPad(1)->SetLogz();

  c1->GetPad(1)->Update();

  palette = (TPaletteAxis*)h2_raw_time_SN->GetListOfFunctions()->FindObject("palette");
  if ( palette ) {
    palette->SetX2NDC( palette->GetX1NDC()+0.01 );
    palette->SetLabelSize(0.03);
    palette->Draw();
  }


  //============================================================================
  //               *** Build coinc time vs. SN ***
  //============================================================================


  
  for (i=0; i<WFD_NUMBER_OF_CRATES; i++) {

    int j;
    for (j=1; j<=h2_coinc_time[i]->GetNbinsY(); j++) {

      int y = int(h2_coinc_time[i]->GetYaxis()->GetBinCenter(j));

      int crate = wfd_crate_number[i];
      int slot    = 2 + j/4;
      int channel = j - (slot-2)*4 - 1;
      
      int zip = bank2zip(crate, slot, channel, run_nr);

      if ( zip == 0 )
	continue;

      int SN = GetSerialNumber(zip);
      
      int ix;
      for (ix=1; ix<h2_coinc_time[i]->GetNbinsX(); ix++) {
	h2_coinc_time_SN->SetCellContent(ix,SN,h2_coinc_time[i]->GetCellContent(ix,j));
      }
      
    }
    
  }

  c1->cd(2);
  h2_coinc_time_SN->Draw("ColZ");
  h2_coinc_time_SN->GetXaxis()->SetRangeUser(-0.5,10200.0);
  if ( h2_coinc_time_SN->GetMaximum() > 100.0 )
    c1->GetPad(2)->SetLogz();

  c1->GetPad(2)->Update();

  palette = (TPaletteAxis*)h2_coinc_time_SN->GetListOfFunctions()->FindObject("palette");
  if ( palette ) {
    palette->SetX2NDC( palette->GetX1NDC()+0.01 );
    palette->SetLabelSize(0.03);
    palette->Draw();
  }



  c1->SaveAs("../png/wfd_raw_time_SN.png");

}
