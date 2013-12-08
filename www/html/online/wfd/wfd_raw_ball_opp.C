
#include <fstream>
#include <stdio.h>
#include <math.h>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TPaletteAxis.h"
#include "TMath.h"

#include "TF1.h"

#include "../common/init.C"
#include "../common/gethist.C"
//#include "/home/mulan/DAQ/analyzer/modules/ZipCodeManipulation.cpp"
#include "../common/ZipCodeManipulation.cpp"
#include "../common/bank2zip.C"

struct s_det {

  int crate;    /* 1-6  */
  int slot;     /* 2-20 */
  int channel;  /* 0-3  */
  int Y;
  int ZIP;
  int SN;       /* Serial number ***/

};

struct s_tile {
  
  float theta_grad;
  float phi_grad;
  float theta;
  float phi;
  
  int PN;      /* pair number  */
  int id_opp;  /* opposite tile id (0-169) */
  
  int Y;

  s_det inner;
  s_det outer;
  
};

const int n_tiles = 170;

s_tile tiles[n_tiles];

void wfd_raw_ball_opp(const int run_nr = 0) 
{
  int i;

  const int n_crates = 6;
  const char *h_names_hits[n_crates] = {"WFDRawHitsB0",
					"WFDRawHitsR1",
					"WFDRawHitsG1",
					"WFDRawHitsR0",
					"WFDRawHitsB1",
					"WFDRawHitsG0" };

  /***
  const char *h_names_coinc[n_crates] = {"WFDRawCoincB0",
					 "WsFDRawCoincR1",
					 "WFDRawCoincG1",
					 "WFDRawCoincR0",
					 "WFDRawCoincB1",
					 "WFDRawCoincG0" };
  ***/

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
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,700,1400);
  c1->Draw();
  c1->cd();

  c1->Divide(1,2,0.001,0.001,10);

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
  TH2D *frame = new TH2D("h2_aux","h2_aux",200,-2.9,2.9,200,-2.9,2.9);
  frame->Fill(0.0,0.0,0.001);
  frame->SetMinimum(0.01);
  frame->SetMaximum(Y_max*0.9);
  frame->Draw("ColZ");
  frame->SetXTitle("#Theta sin(#phi)");
  frame->SetYTitle("#Theta cos(#phi)");

  TGraphErrors *g_DY_Theta = new TGraphErrors();
  g_DY_Theta->SetName("g_DY_Theta");
  g_DY_Theta->SetMarkerStyle(20);
  g_DY_Theta->SetMarkerColor(kBlack);
  g_DY_Theta->SetMarkerSize(1);


  TGraphErrors *g_DY_Theta_1 = new TGraphErrors();
  g_DY_Theta_1->SetName("g_DY_Theta_1");
  g_DY_Theta_1->SetMarkerStyle(20);
  g_DY_Theta_1->SetMarkerColor(kRed);
  g_DY_Theta_1->SetMarkerSize(1);

  TGraphErrors *g_DY_Theta_2 = new TGraphErrors();
  g_DY_Theta_2->SetName("g_DY_Theta_2");
  g_DY_Theta_2->SetMarkerStyle(20);
  g_DY_Theta_2->SetMarkerColor(kGreen);
  g_DY_Theta_2->SetMarkerSize(1);

  TGraphErrors *g_DY_Theta_3 = new TGraphErrors();
  g_DY_Theta_3->SetName("g_DY_Theta_3");
  g_DY_Theta_3->SetMarkerStyle(20);
  g_DY_Theta_3->SetMarkerColor(kBlue);
  g_DY_Theta_3->SetMarkerSize(1);




  TGraphErrors *g_DY_Phi = new TGraphErrors();
  g_DY_Phi->SetName("g_DY_Phi");
  g_DY_Phi->SetMarkerStyle(20);
  g_DY_Phi->SetMarkerColor(kBlack);
  g_DY_Phi->SetMarkerSize(1);

  TGraphErrors *g_DY_Phi_1 = new TGraphErrors();
  g_DY_Phi_1->SetName("g_DY_Phi_1");
  g_DY_Phi_1->SetMarkerStyle(20);
  g_DY_Phi_1->SetMarkerColor(kRed);
  g_DY_Phi_1->SetMarkerSize(1);

  TGraphErrors *g_DY_Phi_2 = new TGraphErrors();
  g_DY_Phi_2->SetName("g_DY_Phi_2");
  g_DY_Phi_2->SetMarkerStyle(20);
  g_DY_Phi_2->SetMarkerColor(kGreen);
  g_DY_Phi_2->SetMarkerSize(1);

  TGraphErrors *g_DY_Phi_3 = new TGraphErrors();
  g_DY_Phi_3->SetName("g_DY_Phi_3");
  g_DY_Phi_3->SetMarkerStyle(20);
  g_DY_Phi_3->SetMarkerColor(kBlue);
  g_DY_Phi_3->SetMarkerSize(1);



  //===========================================================================
  //      *** html output stream ***
  //===========================================================================

  //ofstream *os = new ofstream("../html/wfd_raw_ball_opp.html");

  for (crate=1; crate<=6; crate++)
    for (slot=2; slot<=20; slot++)
      for (chan=0; chan<=3; chan++) {
	
	/*** ZIP code ***/
	zip = bank2zip(crate, slot, chan, run_nr);

	if ( zip == 0 ) continue;

	GetXYZ(zip, xyz);

	if (xyz[0] == 0 && xyz[1] == 0 && xyz[2] == 0) continue;

	/*** Serial Number ***/
	int SN = GetSerialNumber(zip);

	/*** Pair number ***/
	int PN = GetPairNumber(SN);

	/*** id ***/
	int id = PN-1;

	if ( id < 0 || id >= n_tiles ) {
	  cerr << "***ERROR! Bad detector ID: " << id << endl;
	  return;
	}

	double theta=0;
	if (xyz[0] != 0 || xyz[1] != 0) {
	  double arg = xyz[2]/sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]+xyz[2]*xyz[2]);
	  if ( arg >  1.0) arg =  1.0;
	  if ( arg < -1.0) arg = -1.0;
	  theta=acos(arg);
	}

	double phi = atan2(xyz[1],xyz[0]);
	
	cout << crate << " " 
	     << slot << " "
	     << chan << " "
	     << zip << " " 
	     << xyz[0] << " " << xyz[1] << " " << xyz[2] << " "
	     << theta << " " << phi << " "
	     << endl;
	

	Double_t x = -0.375 + 0.25*chan + slot;
	Double_t Y = h1_hits[crate-1]->GetBinContent( h1_hits[crate-1]->FindBin(x) );

	/*** fill array ***/

	tiles[id].theta      = theta;
	tiles[id].phi        = phi;
	tiles[id].theta_grad = theta/TMath::Pi()*180.0;
	tiles[id].phi_grad   = phi/TMath::Pi()*180.0;
	tiles[id].PN         = PN;
	tiles[id].id_opp     = GetOppositePair(PN, PN)-1;
	
	s_det *det = &tiles[id].inner;

	if (SN%2) {
	  det = &tiles[id].outer;
	}


	det->crate = crate;
	det->slot = slot;
	det->channel = chan;
	det->Y = int(Y);
	det->ZIP = zip;
	det->SN = SN;
	
	cout << "csc: " << crate << " " << slot << " " << chan 
	     << " zip: " << zip
	     << " PN: " << PN << " id_opp" << tiles[id].id_opp
	     << endl;

      }
 
  /*** calculate average count ***/
  for (i=0; i<n_tiles; i++) {
    if (tiles[i].outer.Y == 0)
      tiles[i].Y = tiles[i].inner.Y;
    else if (tiles[i].inner.Y == 0)
      tiles[i].Y = tiles[i].outer.Y;
    else
      tiles[i].Y = (tiles[i].outer.Y+tiles[i].inner.Y)/2;
  } 

  /*** find min/max ***/
  double Y_min = 1;
  Y_max = 0;
  for (i=0; i<n_tiles; i++) {
    double Ymean = (tiles[i].Y + tiles[tiles[i].id_opp].Y) / 2.0;
    double DY = 0.0;
    if ( Ymean > 0.0 )
      DY = (tiles[i].Y - tiles[tiles[i].id_opp].Y) / Ymean;
    if (DY < Y_min) Y_min = DY;
    if (DY > Y_max) Y_max = DY;
  } 
 

  /*** plot Delta Y as 2D theta-phi plot ***/
  c1->cd(1);
  frame->Draw();
  for (i=0; i<n_tiles; i++) {

 	Double_t gr_x = tiles[i].theta*sin(tiles[i].phi);
	Double_t gr_y = tiles[i].theta*cos(tiles[i].phi);

	float gr_msiz = 1.5+tiles[i].theta/3.14*1.5;

	sprintf(name,"PN_%d",tiles[i].PN);

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
	double Ymean = (tiles[i].Y + tiles[tiles[i].id_opp].Y) / 2.0;
	double DY = 0.0; 
	if (Ymean > 0.0) 
	  DY = (tiles[i].Y - tiles[tiles[i].id_opp].Y)/Ymean;
	//Double_t DY = tiles[i].Y - tiles[tiles[i].id_opp].Y;
	int color = 10;
	color = 52+50.0*(DY-Y_min)/(Y_max-Y_min);
	if (color > 99) color = 99;

	cout << "color = " << color << " DY=" << DY << endl;
	gr->SetMarkerColor( color );
	gr->Draw("P");
	gr->SetName(name);
	
	int np = g_DY_Theta->GetN();
	g_DY_Theta->SetPoint(np,tiles[i].theta,DY);	  
	g_DY_Theta->SetPointError(np,0,0);
	g_DY_Phi->SetPoint(np,tiles[i].phi,DY);
	g_DY_Phi->SetPointError(np,0,0);


	//if ( tiles[i].theta > 1.45 && tiles[i].theta < 1.55) {
	//if ( tiles[i].theta > 1.25 && tiles[i].theta < 1.35) {
	//if ( tiles[i].theta > 0.5 && tiles[i].theta < 0.7) {
	if ( tiles[i].theta > 0.2 && tiles[i].theta < 0.5) {
	  np = g_DY_Phi_1->GetN();
	  g_DY_Phi_1->SetPoint(np,tiles[i].phi,DY);
	  g_DY_Phi_1->SetPointError(np,0,0);

	  g_DY_Theta_1->SetPoint(np,tiles[i].theta,DY);	  
	  g_DY_Theta_1->SetPointError(np,0,0);
	}
	
	if ( tiles[i].theta > 0.5 && tiles[i].theta < 0.7) {
	  np = g_DY_Phi_2->GetN();
	  g_DY_Phi_2->SetPoint(np,tiles[i].phi,DY);
	  g_DY_Phi_2->SetPointError(np,0,0);

	  g_DY_Theta_2->SetPoint(np,tiles[i].theta,DY);	  
	  g_DY_Theta_2->SetPointError(np,0,0);
	}
	
	if ( tiles[i].theta > 0.7 && tiles[i].theta < 0.85) {
	  np = g_DY_Phi_3->GetN();
	  g_DY_Phi_3->SetPoint(np,tiles[i].phi,DY);
	  g_DY_Phi_3->SetPointError(np,0,0);

	  g_DY_Theta_3->SetPoint(np,tiles[i].theta,DY);	  
	  g_DY_Theta_3->SetPointError(np,0,0);
	}
	
	
	/***
	cout << "pair " << i 
	     << " inner: zip=" << tiles[i].inner.ZIP << " Y=" << tiles[i].inner.Y 
	     << " outer: zip=" << tiles[i].outer.ZIP << " Y=" << tiles[i].outer.Y
	     << " id_opp: " << tiles[i].id_opp
	     << " DY=" << DY << endl;
	***/

 }

  

  /*
  int ii;
  for (ii=0; ii<50; ii++) {
    TGraph *gr_colors = new TGraph();
    gr_colors->SetMarkerStyle(20);
    gr_colors->SetMarkerSize( 1.5 );
    gr_colors->SetPoint(ii,-3.0+6.0*ii/51, 3.0); 
    gr_colors->SetMarkerColor( 52+ii );
    gr_colors->Draw("P");
  }
  */
	



  /*

  TPaletteAxis *paxis = new TPaletteAxis(0.1,0.1,2.8,2.8,frame);
  frame->SetMinimum(0.0);
  frame->SetMaximum(Y_max);
  paxis->Draw();

  */

  c1->GetPad(1)->Update();
  c1->Update();

  TPaletteAxis *palette = (TPaletteAxis*)frame->GetListOfFunctions()->FindObject("palette");
  if ( palette ) {
    palette->SetX2NDC( palette->GetX1NDC()+0.01 );
    palette->Draw();
    palette->SetLabelSize(0.02);
  }



  //==================================================================
  //                 Y(theta), Y(phi)
  //==================================================================
  c1->cd(2);
  c1->GetPad(2)->Divide(1,2,0.001,0.001,10);
  c1->GetPad(2)->GetPad(1)->cd();
  TH1F *h1_frame = c1->GetPad(2)->GetPad(1)->DrawFrame(0.0,Y_min-0.1*fabs(Y_min),3.25,Y_max*1.2);
  h1_frame->SetXTitle("Theta (rad.)");
  h1_frame->SetYTitle("2(Y-Yopposite)/(Y+Yopposite)");
  g_DY_Theta->Draw("P");
  g_DY_Theta_1->Draw("P");
  g_DY_Theta_2->Draw("P");
  g_DY_Theta_3->Draw("P");
  //g_Y_Theta_even->Draw("P");


  c1->GetPad(2)->GetPad(2)->cd();
  h1_frame = c1->GetPad(2)->GetPad(2)->DrawFrame(-3.25,Y_min-0.1*fabs(Y_min),3.25,Y_max*1.2);
  h1_frame->SetXTitle("Phi (rad.)");
  h1_frame->SetYTitle("2(Y-Yopposite)/(Y+Yopposite)");
  //h1_frame->SetTitleSize(0.05,"XY");
  //h1_frame->SetTitleSize(0.05,"Y");
  g_DY_Phi->Draw("P");
  g_DY_Phi_1->Draw("P");
  g_DY_Phi_2->Draw("P");
  g_DY_Phi_3->Draw("P");
  //g_Y_Phi_even->Draw("P");



  //===========================================================================
  //      *** sinus fit to the DY(phi) at ~90 deg. ***
  //===========================================================================

  TF1 *f_sin = new TF1("f_sin","[0]*sin(x+[1])+[2]",-3.2, 3.2);
  f_sin->SetLineColor(kBlue);
  f_sin->SetParameter(0,100);
  f_sin->SetParameter(1,0.1);
  f_sin->SetParameter(2,0.01);
  g_DY_Phi_3->Fit( f_sin );
  f_sin->Draw("Same");
  
  c1->SaveAs("../png/wfd_raw_ball_opp.eps");
  gSystem->Exec("convert -density 100 ../png/wfd_raw_ball_opp.eps ../png/wfd_raw_ball_opp.png");
  //c1->SaveAs("../png/wfd_raw_ball.png");
  //c1->SaveAs("../png/wfd_raw_ball.jpg");

  //===========================================================================
  //      *** close html output ***
  //===========================================================================

  //*os << "</table><br>\n";


  //delete os;

}
