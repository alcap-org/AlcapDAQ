
#include "TGraph.h"

#include "../common/mysql.h"
#include "../common/init.C"

void tau(const int run_1=1, const int run_2=1, const int rollback = 0)
{
  
  char         str[4096];
  TH1F         *frame;
  TVirtualPad  *pad;

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
	  "select r.run,tau,N,B from Runlog as r "
	  "left join RunInfo using (run) "
	  "where "
	  "%s "
	  //"and quality=\"Y\" "
	  "and tau is not null "
	  "and N>0 and B>0 and tau>0"
	  //"and starttime is not null "
	  ,crun_query);

  printf("query: %s\n",str);

  //=========================================================
  //            *** Query ***
  //=========================================================

  TSQLResult *res = db->Query(str);

  if ( !res ) return;

  //=========================================================
  //       *** Process Query, fill s_queery array ***
  //=========================================================

  int nrows   = res->GetRowCount();
  int nfields = res->GetFieldCount();
  
  struct s_query {
    int    run_nr;
    float  tau;
    float  N;
    float  B;
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

    query[i].run_nr    = atoi(row->GetField(0));
    query[i].tau       = atof(row->GetField(1));
    query[i].N         = atof(row->GetField(2));
    query[i].B         = atof(row->GetField(3));


    delete row;
  }

  //=========================================================
  //       *** Fill graphs, histos, etc. ***
  //=========================================================

  float msize = 0.7;

  TGraph *g_tau = new TGraph;
  g_tau->SetName("g_tau");
  g_tau->SetTitle("#tau vs. RunNumber");
  g_tau->SetMarkerStyle(21);
  g_tau->SetMarkerColor(kBlue);
  g_tau->SetMarkerSize(msize);

  TGraph *g_NB = new TGraph;
  g_NB->SetName("g_NB");
  g_NB->SetTitle("N/B vs. RunNumber");
  g_NB->SetMarkerStyle(21);
  g_NB->SetMarkerColor(kBlue);
  g_NB->SetMarkerSize(msize);


  Double_t xmin    =  1.0e32;
  Double_t xmax    = -1.0e32;
  Double_t tau_min =  1.0e32;
  Double_t tau_max = -1.0e32;
  Double_t NB_min  =  1.0e32;
  Double_t NB_max  = -1.0e32;

  for( int i=0; i<nrows; i++ ) {

    if ( query[i].run_nr < xmin )
      xmin = query[i].run_nr;

    if ( query[i].run_nr > xmax )
      xmax = query[i].run_nr;

    if ( query[i].tau < tau_min )
      tau_min = query[i].tau;

    if ( query[i].tau > tau_max )
      tau_max = query[i].tau;
      
    g_tau->SetPoint(g_tau->GetN(),query[i].run_nr,  query[i].tau);

    float NB = 0.0;
    if ( query[i].B > 0 ) {
      NB = query[i].N / query[i].B;
    }

    if ( NB < NB_min )
      NB_min = NB;

    if ( NB > NB_max )
      NB_max = NB;

    g_NB->SetPoint(g_NB->GetN(),query[i].run_nr, NB);

  }
  
  //=========================================================
  //                   *** Create Canvas ***
  //=========================================================
 
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,800);
  c1->Draw();
  
  c1->Divide(1,2);

  
  if ( xmax < xmin ) {
    xmin = 0;
    xmax = 1;
    tau_min = 0;
    tau_max = 2000;
  }

  

  //=========================================================
  //                *** Plot tau vs. RunNr. ***
  //=========================================================

  pad = c1->GetPad(1);
  pad->cd();

  tau_min = tau_min - fabs(tau_min)*0.01;
  tau_max = tau_max + fabs(tau_max)*0.01;

  /*** fix limits ***/
  tau_min = 2170;
  tau_max = 2220;

  frame = pad->DrawFrame(xmin,tau_min,xmax,tau_max,"muon lifetime from fit");
  frame->SetXTitle("run number");
  frame->SetYTitle("#tau (clock ticks)");

  pad->SetGridy();
  g_tau->Draw("P");

  
  //=========================================================
  //                *** Plot N/B vs. RunNr. ***
  //=========================================================

  pad = c1->GetPad(2);
  pad->cd();

  NB_min = NB_min - fabs(NB_min)*0.1;
  NB_max = NB_max + fabs(NB_max)*0.1;

  /*** fix limits ***/
  NB_min = 400;
  NB_max = 1200;

  frame = pad->DrawFrame(xmin,NB_min,xmax,NB_max,"N/B vs. run number");
  frame->SetXTitle("run number");
  frame->SetYTitle("N/B");

  g_NB->Draw("P");
  pad->SetGridy();

  
  delete res;
  delete db;
  delete query;

  c1->SaveAs("../png/tau.png");
  
}


