
#include "TGraph.h"
#include "TMath.h"
#include "TLegend.h"

#include "../common/mysql.h"
#include "../common/init.C"
#include "../common/aux.C"

void beam_ball(const int run_1=1, const int run_2=1, const int rollback = 0)
{
  
  char         str[4096];
  TH1F         *frame;
  TVirtualPad  *pad;
  TLegend  *leg;

  //=========================================================
  //             *** initialize style ***
  //=========================================================
  init();
  gStyle->SetOptStat(11);


  //=========================================================
  //            *** Connect to MySQL server ***
  //=========================================================

  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) return;

  //=========================================================
  //            *** Build run selector ***
  //=========================================================

  char crun_query[256];

  if ( rollback > 0 ) {   
    /*** Get the latest time stamp from Runlog table ***/
    TSQLResult *res = db->Query("select max(starttime) as starttime from Runlog");
    TSQLRow *row = res->Next();
    sprintf(crun_query,"stoptime > '%s' - INTERVAL %d hour and starttime > 0",row->GetField(0),rollback);
    delete row;
    delete res;
  } else {
    sprintf(crun_query,"r.run >= %d and r.run <=%d and starttime > 0 and stoptime > 0",run_1,run_2);
  }

  //=========================================================
  //            *** Build Query ***
  //=========================================================

  sprintf(str,
	  "select r.run,ball_theta_mean,ball_phi_mean,ball_X_mean,ball_Y_mean,ball_Z_mean from RunInfo "
	  "left join Runlog as r using (run) "
	  "where "
	  "%s "
	  "and ball_theta_mean is not null "
	  "and ball_phi_mean is not null "
	  "and ball_X_mean is not null "
	  "and ball_Y_mean is not null "
	  "and ball_Z_mean is not null "
	  ,crun_query);

  printf("query: %s\n",str);

  //=========================================================
  //            *** Query ***
  //=========================================================

  TSQLResult *res = db->Query(str);

  if ( !res ) return;

  //=========================================================
  //       *** Process Query, fill s_query array ***
  //=========================================================

  int nrows   = res->GetRowCount();
  int nfields = res->GetFieldCount();
  
  if (nrows < 1) return;

  struct s_query {
    int    run_nr;
    float  theta;
    float  phi;
    float  X;
    float  Y;
    float  Z;
  };

  s_query *query = new s_query[nrows];

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    /***
	for (int j=0; j<nfields; j++) {
	printf("%s ", row->GetField(j));
	}
	printf("\n");
    ***/

    query[i].run_nr  = atoi( row->GetField(0) );
    query[i].theta   = atof( row->GetField(1) ) / TMath::Pi()*180.0;
    query[i].phi     = atof( row->GetField(2) ) / TMath::Pi()*180.0;
    query[i].X     = atof( row->GetField(3) );
    query[i].Y     = atof( row->GetField(4) );
    query[i].Z     = atof( row->GetField(5) );
 
    delete row;
  }

  //=========================================================
  //       *** Fill graphs, histos, etc. ***
  //=========================================================

  float msize = 0.5;

  TGraph *g_theta = new TGraph;
  g_theta->SetName("g_theta");
  g_theta->SetTitle("Ball <#Theta> vs. RunNumber");
  g_theta->SetMarkerStyle(21);
  g_theta->SetMarkerColor(kBlue);
  g_theta->SetMarkerSize(msize);


  TGraph *g_phi = new TGraph;
  g_phi->SetName("g_phi");
  g_phi->SetTitle("<#phi> vs. RunNumber");
  g_phi->SetMarkerStyle(21);
  g_phi->SetMarkerColor(kBlue);
  g_phi->SetMarkerSize(msize);


  TGraph *g_X = new TGraph;
  g_X->SetName("g_X");
  g_X->SetTitle("<#X> vs. RunNumber");
  g_X->SetMarkerStyle(21);
  g_X->SetMarkerColor(kBlack);
  g_X->SetMarkerSize(msize);

  TGraph *g_Y = new TGraph;
  g_Y->SetName("g_Y");
  g_Y->SetTitle("<#Y> vs. RunNumber");
  g_Y->SetMarkerStyle(21);
  g_Y->SetMarkerColor(kBlue);
  g_Y->SetMarkerSize(msize);

  TGraph *g_Z = new TGraph;
  g_Z->SetName("g_Z");
  g_Z->SetTitle("<#Z> vs. RunNumber");
  g_Z->SetMarkerStyle(21);
  g_Z->SetMarkerColor(kRed);
  g_Z->SetMarkerSize(msize);



  for( int i=0; i<nrows; i++ ) {

    g_theta->SetPoint(g_theta->GetN(), query[i].run_nr, query[i].theta);
    g_phi->SetPoint  (g_phi->GetN(),   query[i].run_nr, query[i].phi);

    g_X->SetPoint  (g_X->GetN(),   query[i].run_nr, query[i].X);
    g_Y->SetPoint  (g_Y->GetN(),   query[i].run_nr, query[i].Y);
    g_Z->SetPoint  (g_Z->GetN(),   query[i].run_nr, query[i].Z);
    
  }
  
  //=========================================================
  //                   *** Create Canvas ***
  //=========================================================
 
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,1000);
  c1->Draw();
  
  c1->Divide(1,4);

  Double_t xmin_aux, xmax_aux;
  Double_t xmin, xmax;
  Double_t ymin, ymax;
  


  //=========================================================
  //           *** Plot X vs. RunNr. ***
  //=========================================================

  get_min_max(g_X,xmin,xmax,ymin,ymax);
  double plot_min=ymin;
  double plot_max=ymax;
  get_min_max(g_Y,xmin,xmax,ymin,ymax);
  if(ymin<plot_min) plot_min=ymin;
  if(ymax>plot_max) plot_max=ymax;
  get_min_max(g_Z,xmin,xmax,ymin,ymax);
  if(ymin<plot_min) plot_min=ymin;
  if(ymax>plot_max) plot_max=ymax;


  if ( xmin == xmax ) {
    xmin -= 0.5;
    xmax += 0.5;
  }


  plot_min = plot_min - 0.5;//fabs(ymin)*0.01;
  plot_max = plot_max + (plot_max-plot_min)*0.3 + 0.5;//fabs(ymax)*0.01;

  pad = c1->GetPad(1);
  pad->cd();

  frame = pad->DrawFrame(xmin,plot_min,xmax,plot_max,"<XYZ> over ball");
  frame->SetXTitle("run number");
  frame->SetYTitle("(cm)");
  
  g_X->Draw("P");
  g_Y->Draw("Psame");
  g_Z->Draw("Psame");

  leg = new TLegend(0.8,0.75,0.9,0.99,"Coordinate:", "NDC");
  leg->AddEntry(g_X,"    X","P");
  leg->AddEntry(g_Y,"    Y","P");
  leg->AddEntry(g_Z,"    Z","P");

  leg->SetBorderSize(1);
  leg->SetFillStyle(0);

  leg->Draw();


  pad->SetGridy(1);
  

  pad = c1->GetPad(2);
  pad->cd();

  plot_min=-0.5;
  plot_max=0.5;
  frame = pad->DrawFrame(xmin,plot_min,xmax,plot_max,"<XYZ> over ball (ZOOMED)");
  frame->SetXTitle("run number");
  frame->SetYTitle("(cm)");

  g_X->Draw("P");
  g_Y->Draw("Psame");
  g_Z->Draw("Psame");

  leg = new TLegend(0.8,0.75,0.9,0.99,"Coordinate:", "NDC");
  leg->AddEntry(g_X,"    X","P");
  leg->AddEntry(g_Y,"    Y","P");
  leg->AddEntry(g_Z,"    Z","P");

  leg->SetBorderSize(1);
  leg->SetFillStyle(0);

  leg->Draw();

  pad->SetGridy(1);



  //=========================================================
  //           *** Plot Theta vs. RunNr. ***
  //=========================================================

  get_min_max(g_theta,xmin,xmax,ymin,ymax);

  if ( xmin == xmax ) {
    xmin -= 0.5;
    xmax += 0.5;
  }

  pad = c1->GetPad(3);
  pad->cd();

  ymin = ymin - 0.5;//fabs(ymin)*0.01;
  ymax = ymax + 0.5;//fabs(ymax)*0.01;

  frame = pad->DrawFrame(xmin,ymin,xmax,ymax,"<#Theta> over ball");
  frame->SetXTitle("run number");
  frame->SetYTitle("<#Theta> (deg.)");
  
  g_theta->Draw("P");

  pad->SetGridy(1);
  
  //=========================================================
  //           *** Plot Phi vs. RunNr. ***
  //=========================================================

  get_min_max(g_phi,xmin_aux,xmax_aux,ymin,ymax);

  pad = c1->GetPad(4);
  pad->cd();

  ymin = ymin - 0.5;//fabs(ymin)*0.05;
  ymax = ymax + 0.5;//fabs(ymax)*0.05;

  frame = pad->DrawFrame(xmin,ymin,xmax,ymax,"<#phi> over ball");
  frame->SetXTitle("run number");
  frame->SetYTitle("<#phi> (deg.)");

  pad->SetGridy(1);
  g_phi->Draw("P");

  
  
  delete res;
  delete db;
  delete query;

  c1->SaveAs("../png/beam_ball.png");
  
}


