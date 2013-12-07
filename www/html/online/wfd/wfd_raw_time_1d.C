

#include "../common/init.C"
#include "../common/gethist.C"

void wfd_raw_time_1d(const int run_nr = 0) 
{
  int i;

  const int n_hists = 6;
  const char *h_names[n_hists] = { "WFDRawTimeR0",
				   "WFDRawTimeR1",
				   "WFDRawTimeG0",
				   "WFDRawTimeG1",
				   "WFDRawTimeB0",
				   "WFDRawTimeB1" };
  
  TH2D *h2[n_hists];

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();

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

    //h2[i]->Draw("ColZ");
    TH1D *h1 = h2[i]->ProjectionX();
    h1->SetLineColor(kRed);
    h1->Draw();
    h1->SetXTitle("time counter");
    h1->SetYTitle("counts");
    //h1->SetFillColor(kYellow);
    h1->GetXaxis()->SetRangeUser(-0.5,15000.0);

    if ( h1->GetMaximum() > 1 ) {
      c1->GetPad(ipad)->SetLogy();
    }

  }
  
  c1->SaveAs("../png/wfd_raw_time_1d.png");


}
