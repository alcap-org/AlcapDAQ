
#include "TGraph.h"

#include "../common/mysql.h"
#include "../common/init.C"
#include "../common/aux.C"

void DAQ_rates(const int run_1=1, const int run_2=1, const int rollback = 0)
{
  
  char         str[4096];
  TH1F         *frame;
  TVirtualPad  *pad;

  const float segment_length = 135.0e-3; // segment length, ms;
  //const float segment_length = 135.0e-4; // segment length, ms;
  
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
    sprintf(crun_query,"run >= %d and run <=%d and starttime > 0 and stoptime > 0",run_1,run_2);
  }

  //=========================================================
  //            *** Build Query ***
  //=========================================================

  sprintf(str,
	  "select run,time_to_sec(stoptime)-time_to_sec(starttime),nsegments,size from Runlog "
	  //"left join RunInfo using (run) "
	  "where "
	  "%s "
	  //"and quality=\"Y\" "
	  //"and tau is not null "
	  //"and starttime is not null "
	  //"and stoptime is not null "
	  "and nsegments is not null "
	  "and size is not null "
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
  
  if (nrows < 1) return;

  struct s_query {
    int    run_nr;
    float  time;
    int    nsegments;
    float  size;
  };

  s_query *query = new s_query[nrows];

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    //***
    for (int j=0; j<nfields; j++) {
      printf("%s ", row->GetField(j));
    }
    printf("\n");
    //***/

    query[i].run_nr    = atoi( row->GetField(0) );
    query[i].time      = atof( row->GetField(1) );
    query[i].nsegments = atof( row->GetField(2) );
    query[i].size      = atof( row->GetField(3) ) / 1048576.0; /*** in MB/s now ***/

    delete row;
  }

  //=========================================================
  //       *** Fill graphs, histos, etc. ***
  //=========================================================

  float msize = 0.7;

  TGraph *g_deadtime = new TGraph;
  g_deadtime->SetName("g_deadtime");
  g_deadtime->SetTitle("deadtime vs. RunNumber");
  g_deadtime->SetMarkerStyle(21);
  g_deadtime->SetMarkerColor(kBlue);
  g_deadtime->SetMarkerSize(msize);


  TGraph *g_datarate = new TGraph;
  g_datarate->SetName("g_datarate");
  g_datarate->SetTitle("data rate vs. RunNumber");
  g_datarate->SetMarkerStyle(21);
  g_datarate->SetMarkerColor(kBlue);
  g_datarate->SetMarkerSize(msize);


  for( int i=0; i<nrows; i++ ) {

    float deadtime = 0.0;
    if ( query[i].time > 0 ) {
      deadtime = (query[i].time - segment_length*query[i].nsegments) / query[i].time * 100.0;
    }

    g_deadtime->SetPoint(g_deadtime->GetN(),query[i].run_nr, deadtime);

    float datarate = 0.0;
    if ( query[i].time > 0 ) {
      datarate = query[i].size /  query[i].time;
    }
    g_datarate->SetPoint(g_datarate->GetN(),query[i].run_nr, datarate);
    
  }
  
  //=========================================================
  //                   *** Create Canvas ***
  //=========================================================
 
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,800);
  c1->Draw();
  
  c1->Divide(1,2);

  Double_t xmin_aux, xmax_aux;
  Double_t xmin, xmax;
  Double_t ymin, ymax;
  

  //=========================================================
  //           *** Plot deadtime vs. RunNr. ***
  //=========================================================

  get_min_max(g_deadtime,xmin,xmax,ymin,ymax);

  if ( xmin == xmax ) {
    xmin -= 0.5;
    xmax += 0.5;
  }

  pad = c1->GetPad(1);
  pad->cd();

  ymin = 0.0;
  ymax = ymax + fabs(ymax)*0.01;

  if (ymax < 30.0)
    ymax = 30.0;

  /*** fix limits ***/
  ymin = 0.0;
  ymax = 45.0;
  
  frame = pad->DrawFrame(xmin,ymin,xmax,ymax,"DAQ dead time");
  frame->SetXTitle("run number");
  frame->SetYTitle("dead time (%)");
  
  g_deadtime->Draw("P");

  pad->SetGridy(1);
  
  //=========================================================
  //           *** Plot datarate vs. RunNr. ***
  //=========================================================

  get_min_max(g_datarate,xmin_aux,xmax_aux,ymin,ymax);

  pad = c1->GetPad(2);
  pad->cd();

  //ymin = ymin - fabs(ymin)*0.01;
  ymin = 0;
  ymax = ymax + fabs(ymax)*0.05;

  /*** fix limits ***/
  ymin = 0.0;
  ymax = 45.0;


  frame = pad->DrawFrame(xmin,ymin,xmax,ymax,"data rate");
  frame->SetXTitle("run number");
  frame->SetYTitle("data rate (Mb/s)");

  pad->SetGridy(1);
  g_datarate->Draw("P");

  
  
  delete res;
  delete db;
  delete query;

  c1->SaveAs("../png/DAQ_rates.png");
  
}


