
#include <fstream>
#include <stdio.h>
#include <math.h>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TPaletteAxis.h"
#include "TMath.h"
#include "TPaveText.h"

#include "../common/init.C"
#include "../common/gethist.C"
#include "/home/mulan/DAQ/analyzer/modules/ZipCodeManipulation.cpp"
#include "../common/bank2zip.C"



void wfd_raw_ball_2(const int run_nr = 0) 
{
  int i;

  const int n_crates = 6;
  const char *h_names_hits[n_crates] = {"WFDRawHitsB0",
					"WFDRawHitsR1",
					"WFDRawHitsG1",
					"WFDRawHitsR0",
					"WFDRawHitsB1",
					"WFDRawHitsG0" };

  const int crates[n_crates] = {5,2,4,1,6,3};

  TH1D *h1_hits [n_crates];
  TH1D *h1_coinc[n_crates];
  TH1D *h1_diff [n_crates];
  char name [256];
  char title[256];

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(0);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetOptTitle(0);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,900,450);
  c1->Draw();
  c1->cd();

  c1->Divide(2,1,0.001,0.001,10);

  //=========================================================
  //          *** receive and plot histograms ***
  //=========================================================
  
  for (i=0; i<n_crates; i++) {

    h1_hits [i] = (TH1D*) get_hist(h_names_hits[i], run_nr);
    //h1_coinc[i] = (TH1D*) get_hist(h_names_coinc[i], run_nr);
    if ( !h1_hits[i] ) return;
  }

  c1->cd(1);


  //*** find maximum ***//
  double Y_max = 0.0;
  double xyz[3];
  int crate, slot, chan, zip;
  for (crate=1; crate<=6; crate++)
    for (slot=2; slot<=20; slot++)
      for (chan=0; chan<=3; chan++) {

	zip = bank2zip(crate, slot, chan, run_nr);

	if ( zip == 0 ) continue;

	GetXYZ(zip, xyz);

	if (xyz[0] == 0 && xyz[1] == 0 && xyz[2] == 0) continue;

	Double_t x = -0.375 + 0.25*chan + slot;
	Double_t Y = h1_hits[crate-1]->GetBinContent( h1_hits[crate-1]->FindBin(x)  );
	if ( Y_max < Y ) {
	  Y_max = Y;
	}	
      }

  //TH1 *frame = c1->DrawFrame(-3.14,-3.14,3.14,3.14);
  const float theta_max = 3.14/2;
  TH2D *frame = new TH2D("h2_aux","h2_aux",200,-theta_max,theta_max,200,-theta_max,theta_max);
  frame->Fill(0.0,0.0,0.001);
  frame->SetMinimum(0.01);
  frame->SetMaximum(Y_max*0.9);
  frame->Draw("ColZ");
  frame->SetXTitle("#Theta sin(#phi)");
  frame->SetYTitle("#Theta cos(#phi)");

  c1->cd(2);
  frame->Draw("ColZ");

  //===========================================================================
  //      *** html output stream ***
  //===========================================================================

  for (crate=1; crate<=6; crate++)
    for (slot=2; slot<=20; slot++)
      for (chan=0; chan<=3; chan++) {
	
	zip = bank2zip(crate, slot, chan);

	if ( zip == 0 ) continue;

	GetXYZ(zip, xyz);

	if (xyz[0] == 0 && xyz[1] == 0 && xyz[2] == 0) continue;

	double theta=0;
	if (xyz[0] != 0 || xyz[1] != 0) {
	  double arg = xyz[2]/sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]+xyz[2]*xyz[2]);
	  if ( arg >  1.0) arg =  1.0;
	  if ( arg < -1.0) arg = -1.0;
	  theta=acos(arg);
	}

	double phi = atan2(xyz[1],xyz[0]);
	
	c1->cd(1);
	if ( theta > TMath::Pi()/2 ) {
	  
	  theta = TMath::Pi() - theta;
	  phi += TMath::Pi();
	  c1->cd(2);
	  
	} 

	cout << crate << " " 
	     << slot << " "
	     << chan << " "
	     << zip << " " 
	     << xyz[0] << " " << xyz[1] << " " << xyz[2] << " "
	     << theta << " " << phi << " "
	     << endl;
	

	Double_t gr_x = theta*sin(phi);
	Double_t gr_y = theta*cos(phi);

	float gr_msiz = 2.2+theta/TMath::Pi()*0.8;

	if ( chan%2 ) {
	  gr_msiz/=2;
	}

	sprintf(name,"c%ds%02dch%d_zip_%d",crate,slot,chan,zip);

	/*** Detector position ***/
	TGraph *gr_marker = new TGraph();
	gr_marker->SetPoint(0,gr_x,gr_y);
	gr_marker->SetMarkerStyle(24);
	gr_marker->SetMarkerSize( gr_msiz );
	gr_marker->SetMarkerColor( kBlack );
	gr_marker->Draw("P");
	gr_marker->SetName(name);

	TGraph *gr = new TGraph();
	gr->SetPoint(0,gr_x,gr_y);
	gr->SetMarkerStyle(20);
	gr->SetMarkerSize( gr_msiz );
	//gr->SetMarkerSize(2.0+sin(theta/2)/3.14*2.5);
	Double_t x = -0.375 + 0.25*chan + slot;
	Double_t Y = h1_hits[crate-1]->GetBinContent( h1_hits[crate-1]->FindBin(x) );
	int color = 10;
	if ( Y_max > 0 && Y > 0 ) {
	  color = 52+50.0*Y/Y_max;
	  //color = 52+50*log(Y)/log(Y_max);
	  if (color > 99) color = 99;
	}
	cout << "color = " << color << " Y=" << Y << endl;
	gr->SetMarkerColor( color );
	gr->Draw("P");
	gr->SetName(name);


	Double_t Y_err = 0.0;
	if (Y > 0)
	  Y_err = sqrt(Y);

      }
  
  c1->GetPad(1)->Update();
  c1->Update();

  TPaletteAxis *palette = (TPaletteAxis*)frame->GetListOfFunctions()->FindObject("palette");
  if ( palette ) {
    palette->SetX2NDC( palette->GetX1NDC()+0.01 );
    palette->Draw();
    palette->SetLabelSize(0.02);
  }

  /*** print lables ***/
  TPaveText *pt;
  c1->cd(1);
  pt = new TPaveText(0.1065,0.9294,0.4624,0.9878,"NDC");
  pt->AddText("downstream");
  pt->SetBorderSize(1.0);
  pt->Draw();

  c1->cd(2);
  pt = new TPaveText(0.1065,0.9294,0.4624,0.9878,"NDC");
  pt->AddText("upstream");
  pt->SetBorderSize(1.0);
  pt->Draw();


  c1->SaveAs("../png/wfd_raw_ball_2.eps");
  gSystem->Exec("convert -density 100 ../png/wfd_raw_ball_2.eps ../png/wfd_raw_ball_2.png");

}
