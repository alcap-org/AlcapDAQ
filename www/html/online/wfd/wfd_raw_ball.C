
#include <fstream>
#include <stdio.h>
#include <math.h>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TPaletteAxis.h"
#include "TMath.h"

#include "../common/init.C"
#include "../common/gethist.C"
//#include "/home/mulan/DAQ/analyzer/modules/ZipCodeManipulation.cpp"
#include "../common/ZipCodeManipulation.cpp"
#include "../common/bank2zip.C"



void wfd_raw_ball(const int run_nr = 0) 
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

  /*** Graphs: yield as function of theta, phi ***/
  TGraphErrors *g_Y_Theta_odd = new TGraphErrors();
  g_Y_Theta_odd->SetName("g_Y_Theta_odd");
  g_Y_Theta_odd->SetMarkerStyle(20);
  g_Y_Theta_odd->SetMarkerColor(kBlack);
  g_Y_Theta_odd->SetMarkerSize(1);

  TGraphErrors *g_Y_Theta_even = new TGraphErrors();
  g_Y_Theta_even->SetName("g_Y_Theta_even");
  g_Y_Theta_even->SetMarkerStyle(21);
  g_Y_Theta_even->SetMarkerColor(kBlue);
  g_Y_Theta_even->SetMarkerSize(1);

  TGraphErrors *g_Y_Phi_odd = new TGraphErrors();
  g_Y_Phi_odd->SetName("g_Y_Phi_odd");
  g_Y_Phi_odd->SetMarkerStyle(20);
  g_Y_Phi_odd->SetMarkerColor(kBlack);
  g_Y_Phi_odd->SetMarkerSize(1);
  
  TGraphErrors *g_Y_Phi_even = new TGraphErrors();
  g_Y_Phi_even->SetName("g_Y_Phi_even");
  g_Y_Phi_even->SetMarkerStyle(20);
  g_Y_Phi_even->SetMarkerColor(kBlue);
  g_Y_Phi_even->SetMarkerSize(1);

  //===========================================================================
  //      *** html output stream ***
  //===========================================================================

  ofstream *os = new ofstream("../html/wfd_raw_ball.html");

  *os << "<hr><br>\n"
      << "<h2>Counting rates of various detectors</h2>\n"
      << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "  <td align=\"center\"><b> crate </b></td>\n"
      << "  <td align=\"center\"><b> slot </b></td>\n"
      << "  <td align=\"center\"><b> ch   </b></td>\n"
      << "  <td align=\"center\"><b> zip </b></td>\n"
      << "  <td align=\"center\"><b> theta </b></td>\n"
      << "  <td align=\"center\"><b> phi </b></td>\n"
      << "  <td align=\"center\"><b> theta*sin(phi) </b></td>\n"
      << "  <td align=\"center\"><b> theta*cos(phi) </b></td>\n"
      << "  <td align=\"center\"><b> counts </b></td>\n"
      << "</tr>";

  
  

  for (crate=1; crate<=6; crate++)
    for (slot=2; slot<=20; slot++)
      for (chan=0; chan<=3; chan++) {
	
	zip = bank2zip(crate, slot, chan, run_nr);

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
	
	cout << crate << " " 
	     << slot << " "
	     << chan << " "
	     << zip << " " 
	     << xyz[0] << " " << xyz[1] << " " << xyz[2] << " "
	     << theta << " " << phi << " "
	     << endl;
	

	Double_t gr_x = theta*sin(phi);
	Double_t gr_y = theta*cos(phi);

	float gr_msiz = 1.5+theta/3.14*1.5;

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

	int np;
	if ( zip%2 ) {
	  np = g_Y_Theta_odd->GetN();
	  g_Y_Theta_odd->SetPoint(np,theta,Y);	  
	  g_Y_Theta_odd->SetPointError(np,0,Y_err);
	  g_Y_Phi_odd->SetPoint(np,phi,Y);
	  g_Y_Phi_odd->SetPointError(np,0,Y_err);
	} else {
	  np = g_Y_Theta_even->GetN();
	  g_Y_Theta_even->SetPoint(np,theta,Y);
	  g_Y_Phi_even->SetPoint(np,phi,Y);	  
	}

	*os << "<tr>\n"
	    << "  <td align=\"center\"><b> " << crate  << " </b></td>\n"
	    << "  <td align=\"center\"><b> " << slot   << " </b></td>\n"
	    << "  <td align=\"center\"><b> " << chan   << " </b></td>\n"
	    << "  <td align=\"center\"><b> " << zip    << " </b></td>\n";
	os->precision(2);
	*os << "  <td align=\"right\"><b> " << fixed << theta/TMath::Pi()*180.0  << " </b></td>\n"
	    << "  <td align=\"right\"><b> " << phi/TMath::Pi()*180.0  << " </b></td>\n"
	    << "  <td align=\"center\"><b> " << gr_x  << " </b></td>\n"
	    << "  <td align=\"center\"><b> " << gr_y  << " </b></td>\n";
	if ( int(Y) == 0) {
	  *os << "  <td align=\"right\"><b><font color=\"#FF0000\"> " << int(Y)  << "</font> </b></td>\n";
	} else {
	  *os << "  <td align=\"right\"><b> " << int(Y)  << " </b></td>\n";
	}
	*os << "</tr>\n";
	

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
  TH1F *h1_frame = c1->GetPad(2)->GetPad(1)->DrawFrame(0.0,-0.1,3.25,Y_max*1.2);
  h1_frame->SetXTitle("Theta (rad.)");
  h1_frame->SetYTitle("counts");
  g_Y_Theta_odd->Draw("P");
  g_Y_Theta_even->Draw("P");


  c1->GetPad(2)->GetPad(2)->cd();
  h1_frame = c1->GetPad(2)->GetPad(2)->DrawFrame(-3.25,-0.1,3.25,Y_max*1.2);
  h1_frame->SetXTitle("Phi (rad.)");
  h1_frame->SetYTitle("counts");
  g_Y_Phi_odd->Draw("P");
  g_Y_Phi_even->Draw("P");

  
  c1->SaveAs("../png/wfd_raw_ball.eps");
  gSystem->Exec("convert -density 100 ../png/wfd_raw_ball.eps ../png/wfd_raw_ball.png");
  //c1->SaveAs("../png/wfd_raw_ball.png");
  //c1->SaveAs("../png/wfd_raw_ball.jpg");

  //===========================================================================
  //      *** close html output ***
  //===========================================================================

  *os << "</table><br>\n";


  delete os;

}
