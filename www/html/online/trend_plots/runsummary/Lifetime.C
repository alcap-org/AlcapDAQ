
#include "TVirtualPad.h"
#include "TF1.h"
#include "TPaveText.h"

#include "../common/init.C"
#include "../common/gethist.C"


void Lifetime(const int run_nr = 0) 
{

  TH1D        *h1; 
  TF1         *f_fit;
  TVirtualPad *pad;
  TPaveText   *pt;
  char        str[1204];
  char        name[1204];
  const float clock = 450.8e6; /*** clock frequency, MHz ***/
  int i;

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(1);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,1800);
  c1->Draw();
  c1->cd();

  c1->Divide(1,3,0.0001,0.0001,10);


  h1 = (TH1D*) get_hist("Lifetime", run_nr);
  
  if ( !h1 ) { 
    cerr << "***ERROR! Histogram Lifetime does not exist. Exiting..." << endl;
    return;
  }


  /*** Number of events analyzed by the analyzer for normalization ***/
  TH1D *h1_Events = (TH1D*) get_hist("h1_Events", run_nr);

  int Nevents   = 0;
  int Nanalyzed = 0;
  if ( h1_Events ) {
    Nanalyzed = int(h1_Events->GetBinContent(1));
    Nevents   = int(h1_Events->GetBinContent(2));
  }
  Double_t eps_analyzed = 0.0;
  if ( Nevents > 0 ) {
    eps_analyzed = float(Nanalyzed)/float(Nevents);
  }

  

  pad = c1->GetPad(1);
  pad->cd();

  h1->Draw();
  //h1->SetStats(kFALSE);
  h1->SetLineColor(kBlue);
  h1->SetLineWidth(2);
  h1->SetXTitle("time counter");
  h1->SetYTitle("counts");
  h1->GetXaxis()->SetRangeUser(0.0,9800.0);


  if ( h1->GetMaximum() > 10.0 ) {
    pad->SetLogy();
  }
  
  //double fitStart=350;
  double fitStart=250;

  /*** fit function ***/
  //  f_fit = new TF1("f_fit","[0]*exp(-x/[1])+[2]",150.0,9500.0);
  f_fit = new TF1("f_fit","[0]*exp(-x/(990.6*(1+([1]/1000000))))+[2]",
		       fitStart,9500.0);

  bool pileup=0;
  if(h1->GetEntries()>0.1E9){
    pileup=1;
    f_fit = new TF1("f_fit","[0]*exp(-x/(990.6*(1+([1]/1000000))))+[2]+[3]*exp(-(x*2)/(990.6*(1+([1]/1000000))))",
		    fitStart,9500.0);
  f_fit->SetParName(3,"P");
  f_fit->SetParameter(3,0.0);
  }

  //f_fit[i] = new TF1(name,"[0]+[1]+[2]",300.0,9500.0);
  f_fit->SetParameter(0,h1->GetBinContent(1));
  f_fit->SetParameter(1,0.0);
  f_fit->SetParameter(2,10.0);

  f_fit->SetParName(0,"N");
  f_fit->SetParName(1,"R");
  f_fit->SetParName(2,"B");

  //f_fit[i]->SetParLimits(0, 0.0, 1.0e32);
  //f_fit[i]->SetParLimits(1, 1.0, 1.0e32);
  //f_fit[i]->SetParLimits(2, 0.0, 1.0e32);
  
  //f_fit[i]->FixParameter(1, 0.0);
  //f_fit[i]->FixParameter(2, 0.0);
  
  h1->Fit(f_fit,"NWR");
  h1->Fit(f_fit,"NR");
  h1->Fit(f_fit,"R");

  f_fit->SetLineColor(kRed);
  f_fit->Draw("Same");

  Double_t N = f_fit->GetParameter(0);
  Double_t B = f_fit->GetParameter(2);
  Double_t t = f_fit->GetParameter(1);
  Double_t P = 0;
  Double_t PFrac = 0;
  if(pileup){
    P = f_fit->GetParameter(3);
    PFrac=P/N;
  }
  Double_t NB = 0.0;
  if ( B != 0 ) {
    NB = N / B;
  } 

  
  
  //int bin_min = h1->FindBin(f_fit->GetXmin());
  int bin_min = h1->FindBin(250.0);
  int bin_max = h1->FindBin(f_fit->GetXmax());
  float Nmuons = h1->Integral(bin_min,bin_max)-B*(bin_max-bin_min);
  float Nmuons_tot = 0.0;
  if ( eps_analyzed > 0.0 ) {
    Nmuons_tot = Nmuons / eps_analyzed;
  }
  /*** bug in online analyzer ***/
  if (eps_analyzed > 0.99)
    eps_analyzed = 0.10;

  pt = new TPaveText(0.593,0.350,0.994,0.650,"NDC");
  sprintf(str,"N/B = %.0f",NB);
  pt->AddText(str);
  sprintf(str,"R = %4.2f ppm",t);
  pt->AddText(str);
  //sprintf(str,"#chi^{2} = %.1f/%d", f_fit->GetChisquare(),bin_max-bin_min);
  //pt->AddText(str);
  //sprintf(str,"Nmuons = %.1e", f_fit->GetParameter(0)*f_fit->GetParameter(1)*(f_fit->Eval(f_fit->GetXmin())-f_fit->Eval(f_fit->GetXmax())));
  sprintf(str,"Ngoodmuons = %.1e",Nmuons);
  pt->AddText(str);
  sprintf(str,"%.1fo/o events analyzed",eps_analyzed*100.0);
  pt->AddText(str);  
  sprintf(str,"Ngoodmuonstot = %.1e",Nmuons_tot);
  pt->AddText(str);
  if(pileup){
    sprintf(str,"Pileup Frac = %.1e",PFrac);
    pt->AddText(str);
  }
  pt->SetBorderSize(1);
  pt->Draw();


  //==============================================================
  //        *** deviation from fit ***
  //==============================================================
  TH1D *h1_err = new TH1D( *h1 );
  for (int i=1; i<=h1_err->GetNbinsX(); i++) {
    Double_t t = h1_err->GetXaxis()->GetBinCenter(i);
    if ( t<f_fit->GetXmin() || t> f_fit->GetXmax() ) {
      h1_err->SetBinContent(i,0);
      h1_err->SetBinError(i,0);
    } else {
      Double_t y = h1_err->GetBinContent(i);
      /*** modified by VT on 25-July-2007 ***/
      Double_t dy = y - f_fit->Eval(t);
      Double_t dy_err = 0.0;
      if ( y>0 ) {
	Double_t sigma = sqrt(y);
	dy = dy / sigma;
	//dy = dy / y;
	dy_err = 1.0;	
      }
      h1_err->SetBinContent(i,dy);
      h1_err->SetBinError(i,dy_err);
      //if ( y>0 ) {
      //h1_err->SetBinContent(i,(y - f_fit->Eval(t))/y);      
      //h1_err->SetBinError(i,y_err/y);
      //}
    }
  }

  pad = c1->GetPad(2);
    
  pad->cd();
  h1_err->SetMarkerStyle(21);
  h1_err->SetMarkerColor(kBlue);
  h1_err->SetMarkerSize(0.2);
  h1_err->Draw("PE");
  //h1->SetStats(kFALSE);
  h1_err->SetLineColor(20);
  h1_err->SetLineWidth(1);
  h1_err->SetXTitle("time counter");
  h1_err->SetYTitle("residual (sigma)");
  h1_err->SetTitle("Residual");
  pad->SetGridy();


  c1->Update();

  //==============================================================
  //              *** distribution of errors ***
  //==============================================================
  pad = c1->GetPad(3);    
  pad->cd();

  //--> this is no very useful, since the binning is to coarse;

  //gStyle->SetStatStyle(1111);
  TH1D *h1_sigma = new TH1D("h1_sigma","error distribution",20.0,-3.0,3.0);
  h1_sigma->SetXTitle("error (sigma)");
  h1_sigma->SetYTitle("counts");
  h1_sigma->SetStats(kTRUE);
  for (int i=1; i<=h1_err->GetNbinsX(); i++) {
 
    Double_t Y = h1_err->GetBinContent(i);
    Double_t Y_err = h1_err->GetBinError(i);
    if ( Y_err != 0.0 )
      h1_sigma->Fill( Y );

  }
  h1_sigma->Draw();
  h1_sigma->SetLineColor(kBlue);
  h1_sigma->SetLineWidth(2);
  pad->SetGridx();
  gStyle->SetOptStat();  
  //c1->GetPad(1)->Modified();
  //c1->GetPad(1)->Update();
  //c1->Update();


  c1->SaveAs("../png/Lifetime.png");

  if(run_nr>900000){
    sprintf(name, "/mnt/raid/online_root/Lifetime_Lifetime_%06d.root", run_nr);
    TFile *f=new TFile(name, "RECREATE");
    h1->SetDirectory(f);
    h1_err->SetName("resid");
    h1_err->SetDirectory(f);
    f->Write();
    f->Close();
  }

  //==============================================================
  //        *** HTML output ***
  //==============================================================

  /***
  ofstream *os = new ofstream("../html/Lifetime_by_ring.html");

  *os << "number of events analyzed: n = " << Nevents << "<br>\n";

  *os << "<hr><br>\n"
      << "<h2>Fit parameters for various rings</h2>\n"
      << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "  <td align=\"center\"><b> Ring </b></td>\n"
      << "  <td align=\"center\"><b> N </b></td>\n"
      << "  <td align=\"center\"><b> N/nEvents </b></td>\n"
      << "  <td align=\"center\"><b> B   </b></td>\n"
      << "  <td align=\"center\"><b> N/B </b></td>\n"
      << "  <td align=\"center\"><b> tau (mks) </b></td>\n"
      << "</tr>";

  for (i=0; i<=n_rings; i++) {
    *os << "<tr>\n";

    if ( i < n_rings )
      *os << "  <td align=\"center\"><b> " << i+1  << " </b></td>\n";
    else
      *os << "  <td align=\"center\"><b> All </b></td>\n";

    *os << "  <td align=\"center\"><b> " << N[i]   << " </b></td>\n"
	<< "  <td align=\"center\"><b> " << (Nevents ? N[i]/Nevents : 0)  << " </b></td>\n"
	<< "  <td align=\"center\"><b> " << B[i]    << " </b></td>\n"
	<< "  <td align=\"center\"><b> " << NB[i]   << " </b></td>\n"
	<< "  <td align=\"center\"><b> " << t[i] / clock *1.0e6   << " </b></td>\n"
	<< "</tr>\n"
      ;
    
  }

  *os << "</table><br>\n";

  *os << "The Lifetime has been evaluated assuming " <<  clock << " Hz of WFD clock frequency.<br>\n";

  delete os;
  ***/

}
