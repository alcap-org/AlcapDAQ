
#include <fstream>

#include "../common/init.C"
#include "../common/gethist.C"

void wfd_raw_hits(const int run_nr = 0) 
{
  int i;

  const int n_hists = 6;
  const char *h_names_hits[n_hists] = { "WFDRawHitsR0",
					"WFDRawHitsR1",
					"WFDRawHitsG0",
					"WFDRawHitsG1",
					"WFDRawHitsB0",
					"WFDRawHitsB1" };

  const char *h_names_coinc[n_hists] = { "WFDRawCoincR0",
					 "WFDRawCoincR1",
					 "WFDRawCoincG0",
					 "WFDRawCoincG1",
					 "WFDRawCoincB0",
					 "WFDRawCoincB1" };
  
  TH1D *h1_hits [n_hists];
  TH1D *h1_coinc[n_hists];
  TH1D *h1_diff [n_hists];
  char name [256];
  char title[256];

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
  //c1->Divide(2,3);

  //=========================================================
  //          *** receive and plot histograms ***
  //=========================================================
  
  for (i=0; i<n_hists; i++) {

    h1_hits[i] = (TH1D*) get_hist(h_names_hits[i], run_nr);
    if ( !h1_hits[i] ) continue;

    h1_diff[i] = new TH1D( *h1_hits[i] );
    sprintf(name,"%s_diff",h1_hits[i]->GetName());
    h1_diff[i]->SetName(name);    

    h1_coinc[i] = (TH1D*) get_hist(h_names_coinc[i], run_nr);
    if ( !h1_coinc[i] ) continue;

    int ipad = i+1;

    c1->cd(ipad);

    c1->GetPad(ipad)->SetGridx(1);

    h1_hits [i]->SetLineColor(kBlack);
    h1_coinc[i]->SetLineColor(kBlue);
    h1_diff [i]->SetLineColor(kRed);

    h1_hits [i]->SetLineWidth(4);
    h1_coinc[i]->SetLineWidth(2);
    h1_diff [i]->SetLineWidth(2);
    h1_diff [i]->SetFillColor(kYellow);
    //h1_diff [i]->SetFillColor(kGreen);
    //h1_coinc [i]->SetFillColor(kGreen);
    //h1_coinc [i]->SetFillColor(17);
    //h1_hits  [i]->SetFillColor(18);
    //h1_coinc [i]->SetFillColor(18);
    h1_hits  [i]->SetFillColor(19);

    if ( h1_hits[i]->GetMaximum() > 2 ) {
      ;//h1_hits [i]->SetMinimum(0.9);
      ;//c1->GetPad(ipad)->SetLogy();
    } else {
      h1_hits [i]->SetMinimum(0.0);
      h1_hits [i]->SetMaximum(2.0);
    }

    //h1_hits [i]->GetXaxis()->SetRangeUser(0.0,12.0);


    h1_hits [i]->Draw();
    h1_hits [i]->SetXTitle("slot number");
    h1_hits [i]->SetYTitle("counts");

    h1_coinc[i]->Draw("Same");

    h1_diff [i]->Add(h1_coinc[i],-1);
    h1_diff [i]->Draw("Same");



    c1->GetPad(ipad)->Update();

  }
  
  c1->SaveAs("../png/wfd_raw_hits.png");

  //===========================================================================
  //      *** make comments to the plot as html output ***
  //===========================================================================

  ofstream *os = new ofstream("../html/wfd_raw_hits.html");
  *os << "<b>Black histogram: total number of islands in each channel (inclusive spectrum)<br>"
      << "<font color=\"#0000FF\"> Blue histogram: number of coincidences</font><br>"
      << "<font color=\"#FF0000\"> Red histogram: difference between black and blue</font><br>"
      << "</b>";


  delete os;

}
