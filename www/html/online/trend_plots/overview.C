/*
  Vanya overestimate the number of good muons in his muexpert program.
  I added a factor 0.7 to account for that.
  This factor has to be removed later when Vanya fixes muexpert.
*/


#include<fstream>
#include<iomanip>


#include "TBox.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TLegend.h"

#include "../common/mysql.h"
#include "../common/init.C"

void overview()
{


  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) return;
  
  /***
  TSQLResult *res = db->Query("select count(r.run), sum(N*2200/40*0.8/1e9*maxserial/nsegment)*0.7*1.2,"
			      "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
			      "to_days(starttime)-to_days('2006-08-01') as date "
			      "from Runlog as r "
			      "left join RunInfo using (run) "
			      "left join RunScalers using (run) "
			      "where "
			      "tau between 1900 and 2400 "
			      "and quality='Y' "
			      "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
			      "and N is not null "
			      "and maxserial is not null "
			      "and nsegment  is not null "
			      "group by date");
  ***/

  /***
  TSQLResult *res = db->Query("select count(r.run), sum(N*2200/40*0.8/1e9*nsegments/nsegment)*0.7*1.25,"
			      "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
			      "to_days(starttime)-to_days('2006-08-01') as date "
			      "from Runlog as r "
			      "left join RunInfo using (run) "
			      "left join RunScalers using (run) "
			      "where "
			      "tau between 1900 and 2400 "
			      "and quality='Y' "
			      "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
			      "and N is not null "
			      "and maxserial is not null "
			      "and nsegment  is not null "
			      "and nsegments  is not null "
			      "group by date");
  ***/

  TSQLResult *res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
			      "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
			      "to_days(starttime)-to_days('2007-06-13') as date "
			      "from Runlog as r "
			      "left join RunInfo using (run) "
			      "left join RunScalers using (run) "
			      "where "
			      "tau between 1900 and 2400 "
			      "and quality='Y' "
			      "and time>10 and starttime > 0 and stoptime > 0 and r.run>61377 "
			      "and N is not null "
			      //"and maxserial is not null "
			      //"and nsegment  is not null "
			      "and nevents_analyzed>0 "
			      "and nsegments  is not null "
			      "group by date");

  int nrows = res->GetRowCount();
  int nfields = res->GetFieldCount();
  
  struct s_query {
    int   nruns;
    float n_muons;
    float hours;
    int   day;
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

    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    printf("i=%d day: %d nruns: %d, n_muons: %f, n_hours: %f\n",
	   i,
	   query[i].day,
	   query[i].nruns,
	   query[i].n_muons,
	   query[i].hours);

    delete row;
  }
  double day0 = query[0].day;
  double day1 = query[nrows-1].day;
  int nday = int(day1-day0+1.1);

  TH1D *hmu = new TH1D("hmu","Number of collected good #mu per day, Giga#mu",nday,day0-0.5,day1+0.5);
  hmu->GetXaxis()->SetTitle("Days since June 13, 2007");
  hmu->GetYaxis()->SetTitle("10^{9} #mu");
  
  TH1D *smu = new TH1D("smu","Total number of collected good #mu, Giga#mu",nday,day0-0.5,day1+0.5);
  smu->GetXaxis()->SetTitle("Days since June 13, 2007");
  smu->GetYaxis()->SetTitle("10^{9} #mu");
  
  TH1D *htime = new TH1D("htime","Data taking hours per day",nday,day0-0.5,day1+0.5);
  htime->GetXaxis()->SetTitle("Days since June 13, 2007");
  htime->GetYaxis()->SetTitle("Hours");
  
  TH1D *stime = new TH1D("stime","Total data taking hours",nday,day0-0.5,day1+0.5);
  stime->GetXaxis()->SetTitle("Days since June 13, 2007");
  stime->GetYaxis()->SetTitle("Hours");

  double sum_mu   = 0;
  double sum_time = 0;

  for (int i=0; i<nrows; i++) {

    hmu  -> Fill( query[i].day, query[i].n_muons);
    htime-> Fill( query[i].day, query[i].hours);
    
    sum_mu += query[i].n_muons;
    sum_time += query[i].hours;
	 
    smu->Fill(query[i].day, sum_mu);
    stime->Fill(query[i].day,sum_time);
    
  }
  
  TH1D *h[4] = { smu, stime, hmu, htime };
  double ymin[4], ymax[4];
  for( int i=0; i<4; i++ ) {
    h[i]->SetTitleOffset(0.9,"x");
    h[i]->SetTitleSize(0.05,"x");
    h[i]->SetTitleOffset(1.2,"y");
    h[i]->SetTitleSize(0.055,"y");
    h[i]->SetLabelOffset(0.01,"x");
    h[i]->SetLabelSize(0.05,"x");
    h[i]->SetLabelOffset(0.01,"y");
    h[i]->SetLabelSize(0.05,"y");
    h[i]->SetTitleSize(0.12,"t");
    ymin[i] = 0;
    ymax[i] = h[i]->GetMaximum()*1.1;
    h[i]->SetMinimum(0.0);
    h[i]->SetMaximum((ymax[i]>0)?ymax[i]:1.0);
  }


  TBox *b = new TBox();
  b->SetFillColor(21);
  b->SetFillStyle(1001);
  b->SetLineWidth(0);
  
  TStyle *plain = gROOT->GetStyle("Plain");
  plain->SetTitleBorderSize(0);
  plain->SetPadLeftMargin(0.12);
  plain->SetPadBottomMargin(0.12);
  plain->SetPadRightMargin(0.01);
  plain->SetOptStat(0);
  gROOT->SetStyle("Plain");

  //=========================================================
  //             *** initialize style ***
  //=========================================================
  init();
  gStyle->SetGridColor(20);

  
  TCanvas *c1 = new TCanvas("cmain","Summer-2007 run overview",800,600);
  
  c1->cd();
  
  c1->Divide(2,2);

  c1->cd(1);

  /*** remove zeros ***/
  for (int i=2; i<=smu->GetNbinsX(); i++) {
    if ( smu->GetBinContent(i) == 0.0 )
      smu->SetBinContent(i, smu->GetBinContent(i-1) );    
  }

  for (int i=2; i<=stime->GetNbinsX(); i++) {
    if ( stime->GetBinContent(i) == 0.0 )
      stime->SetBinContent(i, stime->GetBinContent(i-1) );
  }
  
  smu->SetMarkerColor(2);
  smu->SetLineColor(2);
  smu->SetLineWidth(4);
  smu->SetMarkerStyle(1);
  smu->Draw("l");
  for( int i=int(day0+0.1); i<int(day0+nday+0.1); i++ ) if( i%7 == 1 ) b->DrawBox(i-0.5,ymin[0],i+0.5,ymax[0]);
  smu->Draw("l,same");
  
  c1->cd(2);
 
  stime->SetMarkerColor(2);
  stime->SetLineColor(2);
  stime->SetLineWidth(4);
  stime->SetMarkerStyle(1);
  stime->Draw("l");
  for( int i=int(day0+0.1); i<int(day0+nday+0.1); i++ ) if( i%7 == 1 ) b->DrawBox(i-0.5,ymin[1],i+0.5,ymax[1]);
  stime->Draw("l,same");
  
  c1->cd(3);
  
  hmu->SetMarkerColor(2);
  hmu->SetLineColor(2);
  hmu->SetLineWidth(2);
  hmu->SetMarkerStyle(1);
  hmu->SetBarWidth(0.6);
  hmu->SetBarOffset(0.2);
  hmu->SetFillStyle(1001);
  hmu->SetFillColor(2);
  hmu->Draw("bar");
  for( int i=int(day0+0.1); i<int(day0+nday+0.1); i++ ) if( i%7 == 1 ) b->DrawBox(i-0.5,ymin[2],i+0.5,ymax[2]);
  hmu->Draw("bar,same");
  
  c1->cd(4);
  
  htime->SetMarkerColor(4);
  htime->SetLineColor(4);
  htime->SetLineWidth(2);
  htime->SetMarkerStyle(1);
  htime->SetBarWidth(0.6);
  htime->SetBarOffset(0.2);
  htime->SetFillStyle(1001);
  htime->SetFillColor(4);
  htime->Draw("bar");
  for( int i=int(day0+0.1); i<int(day0+nday+0.1); i++ ) if( i%7 == 1 ) b->DrawBox(i-0.5,ymin[3],i+0.5,ymax[3]);
  htime->Draw("bar,same");
  
  
  delete res;
  delete query;



  //=======================================================================
  //                   *** Target magnet right ***
  //========================================================================

  /***
  res = db->Query("select count(r.run), sum(N*2200/40*0.8/1e9*maxserial/nsegment)*0.7,"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2006-08-01') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='right' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
		  "group by date");
  ***/

  /***
  res = db->Query("select count(r.run), sum(N*2200/40*0.8/1e9*nsegments/nsegment)*0.7*1.25,"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2006-08-01') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='right' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
		  "and nsegments  is not null "
		  "group by date");
  ***/
  
  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='right' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>61377 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");

  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_right = new TGraph();
  g_smu_target_right->SetName("g_smu_target_right");
  g_smu_target_right->SetLineWidth(2);
  g_smu_target_right->SetLineColor(kMagenta);
  
  TGraph *g_stime_target_right = new TGraph();
  g_stime_target_right->SetName("g_stime_target_right");
  g_stime_target_right->SetLineWidth(2);
  g_stime_target_right->SetLineColor(kMagenta);
  
  Double_t sum_mu_target_right   = 0;
  Double_t sum_time_target_right = 0;

  int day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));


    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_right   -> SetPoint(j, j, sum_mu_target_right);
      g_stime_target_right -> SetPoint(j, j, sum_time_target_right);      
    }

    sum_mu_target_right   += query[i].n_muons;
    sum_time_target_right += query[i].hours;

    //g_smu_target_right   -> SetPoint(g_smu_target_right->GetN(),   query[i].day, sum_mu_target_right);
    //g_stime_target_right -> SetPoint(g_stime_target_right->GetN(), query[i].day, sum_time_target_right);

    g_smu_target_right   -> SetPoint(query[i].day, query[i].day, sum_mu_target_right);
    g_stime_target_right -> SetPoint(query[i].day, query[i].day, sum_time_target_right);

    day_old = query[i].day;

    printf("day: %d mu_right: %f\n",query[i].day,sum_mu_target_right);


    delete row;
  }

  g_smu_target_right   -> SetPoint(g_smu_target_right->GetN(),   day1, sum_mu_target_right);
  g_stime_target_right -> SetPoint(g_stime_target_right->GetN(), day1, sum_time_target_right);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_right->Draw("L");
  
  c1->cd(2);
  g_stime_target_right->Draw("L");

  c1->GetPad(1)->SetGridy(1);
  c1->GetPad(2)->SetGridy(1);
  c1->GetPad(3)->SetGridy(1);
  c1->GetPad(4)->SetGridy(1);
  




  //=======================================================================
  //                   *** Target magnet left ***
  //========================================================================

  /***
  res = db->Query("select count(r.run), sum(N*2200/40*0.8/1e9*maxserial/nsegment)*0.7,"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2006-08-01') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='left' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
		  "group by date");
  ***/

  /***
  res = db->Query("select count(r.run), sum(N*2200/40*0.8/1e9*nsegments/nsegment)*0.7*1.25,"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2006-08-01') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='left' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
		  "and nsegments  is not null "
		  "group by date");
  ***/

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='left' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>20468 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_left = new TGraph();
  g_smu_target_left->SetName("g_smu_target_left");
  g_smu_target_left->SetLineWidth(2);
  g_smu_target_left->SetLineColor(3);
  
  TGraph *g_stime_target_left = new TGraph();
  g_stime_target_left->SetName("g_stime_target_left");
  g_stime_target_left->SetLineWidth(2);
  g_stime_target_left->SetLineColor(3);
  
  Double_t sum_mu_target_left   = 0;
  Double_t sum_time_target_left = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_left   -> SetPoint(j, j, sum_mu_target_left);
      g_stime_target_left -> SetPoint(j, j, sum_time_target_left);      
    }
    
    sum_mu_target_left   += query[i].n_muons;
    sum_time_target_left += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_left   -> SetPoint(query[i].day, query[i].day, sum_mu_target_left);
    g_stime_target_left -> SetPoint(query[i].day, query[i].day, sum_time_target_left);


    day_old = query[i].day;

    printf("day: %d mu_left: %f \n",query[i].day,sum_mu_target_left);

    delete row;
  }

  g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   day1, sum_mu_target_left);
  g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), day1, sum_time_target_left);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_left->Draw("L");
  
  c1->cd(2);
  g_stime_target_left->Draw("L");




  //=======================================================================
  //                   *** Target magnet up ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='up' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>62415 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_up = new TGraph();
  g_smu_target_up->SetName("g_smu_target_up");
  g_smu_target_up->SetLineWidth(2);
  g_smu_target_up->SetLineColor(kBlue);
  
  TGraph *g_stime_target_up = new TGraph();
  g_stime_target_up->SetName("g_stime_target_up");
  g_stime_target_up->SetLineWidth(2);
  g_stime_target_up->SetLineColor(kBlue);
  
  Double_t sum_mu_target_up   = 0;
  Double_t sum_time_target_up = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_up   -> SetPoint(j, j, sum_mu_target_up);
      g_stime_target_up -> SetPoint(j, j, sum_time_target_up);      
    }
    
    sum_mu_target_up   += query[i].n_muons;
    sum_time_target_up += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_up   -> SetPoint(query[i].day, query[i].day, sum_mu_target_up);
    g_stime_target_up -> SetPoint(query[i].day, query[i].day, sum_time_target_up);


    day_old = query[i].day;

    printf("day: %d mu_up: %f \n",query[i].day,sum_mu_target_up);

    delete row;
  }

  g_smu_target_up   -> SetPoint(g_smu_target_up->GetN(),   day1, sum_mu_target_up);
  g_stime_target_up -> SetPoint(g_stime_target_up->GetN(), day1, sum_time_target_up);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_up->Draw("L");
  
  c1->cd(2);
  g_stime_target_up->Draw("L");





  //=======================================================================
  //                   *** Target magnet down ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='down' "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>62415 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_down = new TGraph();
  g_smu_target_down->SetName("g_smu_target_down");
  g_smu_target_down->SetLineWidth(2);
  g_smu_target_down->SetLineColor(kCyan);
  
  TGraph *g_stime_target_down = new TGraph();
  g_stime_target_down->SetName("g_stime_target_down");
  g_stime_target_down->SetLineWidth(2);
  g_stime_target_down->SetLineColor(kCyan);
  
  Double_t sum_mu_target_down   = 0;
  Double_t sum_time_target_down = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_down   -> SetPoint(j, j, sum_mu_target_down);
      g_stime_target_down -> SetPoint(j, j, sum_time_target_down);      
    }
    
    sum_mu_target_down   += query[i].n_muons;
    sum_time_target_down += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_down   -> SetPoint(query[i].day, query[i].day, sum_mu_target_down);
    g_stime_target_down -> SetPoint(query[i].day, query[i].day, sum_time_target_down);


    day_old = query[i].day;

    printf("day: %d mu_down: %f \n",query[i].day,sum_mu_target_down);

    delete row;
  }

  g_smu_target_down   -> SetPoint(g_smu_target_down->GetN(),   day1, sum_mu_target_down);
  g_stime_target_down -> SetPoint(g_stime_target_down->GetN(), day1, sum_time_target_down);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_down->Draw("L");
  
  c1->cd(2);
  g_stime_target_down->Draw("L");








  //=======================================================================
  //                   *** Ag: Target magnet left ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='left' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63230 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_left_Ag = new TGraph();
  g_smu_target_left_Ag->SetName("g_smu_target_left_Ag");
  g_smu_target_left_Ag->SetLineWidth(2);
  g_smu_target_left_Ag->SetLineColor(44);
  
  TGraph *g_stime_target_left_Ag = new TGraph();
  g_stime_target_left_Ag->SetName("g_stime_target_left_Ag");
  g_stime_target_left_Ag->SetLineWidth(2);
  g_stime_target_left_Ag->SetLineColor(44);
  
  Double_t sum_mu_target_left_Ag   = 0;
  Double_t sum_time_target_left_Ag = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_left_Ag   -> SetPoint(j, j, sum_mu_target_left_Ag);
      g_stime_target_left_Ag -> SetPoint(j, j, sum_time_target_left_Ag);      
    }
    
    sum_mu_target_left_Ag   += query[i].n_muons;
    sum_time_target_left_Ag += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_left_Ag   -> SetPoint(query[i].day, query[i].day, sum_mu_target_left_Ag);
    g_stime_target_left_Ag -> SetPoint(query[i].day, query[i].day, sum_time_target_left_Ag);


    day_old = query[i].day;

    printf("day: %d mu_left: %f \n",query[i].day,sum_mu_target_left_Ag);

    delete row;
  }

  g_smu_target_left_Ag   -> SetPoint(g_smu_target_left_Ag->GetN(),   day1, sum_mu_target_left_Ag);
  g_stime_target_left_Ag -> SetPoint(g_stime_target_left_Ag->GetN(), day1, sum_time_target_left_Ag);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_left_Ag->Draw("L");
  
  c1->cd(2);
  g_stime_target_left_Ag->Draw("L");















  //=======================================================================
  //                   *** Ag: Target magnet right ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='right' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63230 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_right_Ag = new TGraph();
  g_smu_target_right_Ag->SetName("g_smu_target_right_Ag");
  g_smu_target_right_Ag->SetLineWidth(2);
  g_smu_target_right_Ag->SetLineColor(48);
  
  TGraph *g_stime_target_right_Ag = new TGraph();
  g_stime_target_right_Ag->SetName("g_stime_target_right_Ag");
  g_stime_target_right_Ag->SetLineWidth(2);
  g_stime_target_right_Ag->SetLineColor(48);
  
  Double_t sum_mu_target_right_Ag   = 0;
  Double_t sum_time_target_right_Ag = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_right_Ag   -> SetPoint(j, j, sum_mu_target_right_Ag);
      g_stime_target_right_Ag -> SetPoint(j, j, sum_time_target_right_Ag);      
    }
    
    sum_mu_target_right_Ag   += query[i].n_muons;
    sum_time_target_right_Ag += query[i].hours;

    //g_smu_target_right   -> SetPoint(g_smu_target_right->GetN(),   query[i].day, sum_mu_target_right);
    //g_stime_target_right -> SetPoint(g_stime_target_right->GetN(), query[i].day, sum_time_target_right);

    g_smu_target_right_Ag   -> SetPoint(query[i].day, query[i].day, sum_mu_target_right_Ag);
    g_stime_target_right_Ag -> SetPoint(query[i].day, query[i].day, sum_time_target_right_Ag);


    day_old = query[i].day;

    printf("day: %d mu_right: %f \n",query[i].day,sum_mu_target_right_Ag);

    delete row;
  }

  g_smu_target_right_Ag   -> SetPoint(g_smu_target_right_Ag->GetN(),   day1, sum_mu_target_right_Ag);
  g_stime_target_right_Ag -> SetPoint(g_stime_target_right_Ag->GetN(), day1, sum_time_target_right_Ag);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_right_Ag->Draw("L");
  
  c1->cd(2);
  g_stime_target_right_Ag->Draw("L");






  //=======================================================================
  //                   *** Ag: Target magnet up ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='up' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63230 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_up_Ag = new TGraph();
  g_smu_target_up_Ag->SetName("g_smu_target_up_Ag");
  g_smu_target_up_Ag->SetLineWidth(2);
  g_smu_target_up_Ag->SetLineColor(46);
  
  TGraph *g_stime_target_up_Ag = new TGraph();
  g_stime_target_up_Ag->SetName("g_stime_target_up_Ag");
  g_stime_target_up_Ag->SetLineWidth(2);
  g_stime_target_up_Ag->SetLineColor(46);
  
  Double_t sum_mu_target_up_Ag   = 0;
  Double_t sum_time_target_up_Ag = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_up_Ag   -> SetPoint(j, j, sum_mu_target_up_Ag);
      g_stime_target_up_Ag -> SetPoint(j, j, sum_time_target_up_Ag);      
    }
    
    sum_mu_target_up_Ag   += query[i].n_muons;
    sum_time_target_up_Ag += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_up_Ag   -> SetPoint(query[i].day, query[i].day, sum_mu_target_up_Ag);
    g_stime_target_up_Ag -> SetPoint(query[i].day, query[i].day, sum_time_target_up_Ag);


    day_old = query[i].day;

    printf("day: %d mu_up: %f \n",query[i].day,sum_mu_target_up_Ag);

    delete row;
  }

  g_smu_target_up_Ag   -> SetPoint(g_smu_target_up_Ag->GetN(),   day1, sum_mu_target_up_Ag);
  g_stime_target_up_Ag -> SetPoint(g_stime_target_up_Ag->GetN(), day1, sum_time_target_up_Ag);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_up_Ag->Draw("L");
  
  c1->cd(2);
  g_stime_target_up_Ag->Draw("L");







  //=======================================================================
  //                   *** Ag: Target magnet down ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='down' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63230 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_down_Ag = new TGraph();
  g_smu_target_down_Ag->SetName("g_smu_target_down_Ag");
  g_smu_target_down_Ag->SetLineWidth(2);
  g_smu_target_down_Ag->SetLineColor(41);
  
  TGraph *g_stime_target_down_Ag = new TGraph();
  g_stime_target_down_Ag->SetName("g_stime_target_down_Ag");
  g_stime_target_down_Ag->SetLineWidth(2);
  g_stime_target_down_Ag->SetLineColor(41);
  
  Double_t sum_mu_target_down_Ag   = 0;
  Double_t sum_time_target_down_Ag = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_down_Ag   -> SetPoint(j, j, sum_mu_target_down_Ag);
      g_stime_target_down_Ag -> SetPoint(j, j, sum_time_target_down_Ag);      
    }
    
    sum_mu_target_down_Ag   += query[i].n_muons;
    sum_time_target_down_Ag += query[i].hours;

    //g_smu_target_down   -> SetPoint(g_smu_target_down->GetN(),   query[i].day, sum_mu_target_down);
    //g_stime_target_down -> SetPoint(g_stime_target_down->GetN(), query[i].day, sum_time_target_down);

    g_smu_target_down_Ag   -> SetPoint(query[i].day, query[i].day, sum_mu_target_down_Ag);
    g_stime_target_down_Ag -> SetPoint(query[i].day, query[i].day, sum_time_target_down_Ag);


    day_old = query[i].day;

    printf("day: %d mu_down: %f \n",query[i].day,sum_mu_target_down_Ag);

    delete row;
  }

  g_smu_target_down_Ag   -> SetPoint(g_smu_target_down_Ag->GetN(),   day1, sum_mu_target_down_Ag);
  g_stime_target_down_Ag -> SetPoint(g_stime_target_down_Ag->GetN(), day1, sum_time_target_down_Ag);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_down_Ag->Draw("L");
  
  c1->cd(2);
  g_stime_target_down_Ag->Draw("L");













  //========================================================================
  //          *** Ag target, ball moved by 2cm upstream ***
  //========================================================================




  //=======================================================================
  //                   *** Ag: Target magnet left, z=-2cm ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='left' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63774 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_left_Ag_m2 = new TGraph();
  g_smu_target_left_Ag_m2->SetName("g_smu_target_left_Ag_m2");
  g_smu_target_left_Ag_m2->SetLineWidth(2);
  g_smu_target_left_Ag_m2->SetLineColor(44);
  
  TGraph *g_stime_target_left_Ag_m2 = new TGraph();
  g_stime_target_left_Ag_m2->SetName("g_stime_target_left_Ag_m2");
  g_stime_target_left_Ag_m2->SetLineWidth(2);
  g_stime_target_left_Ag_m2->SetLineColor(44);
  
  Double_t sum_mu_target_left_Ag_m2   = 0;
  Double_t sum_time_target_left_Ag_m2 = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_left_Ag_m2   -> SetPoint(j, j, sum_mu_target_left_Ag_m2);
      g_stime_target_left_Ag_m2 -> SetPoint(j, j, sum_time_target_left_Ag_m2);      
    }
    
    sum_mu_target_left_Ag_m2   += query[i].n_muons;
    sum_time_target_left_Ag_m2 += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_left_Ag_m2   -> SetPoint(query[i].day, query[i].day, sum_mu_target_left_Ag_m2);
    g_stime_target_left_Ag_m2 -> SetPoint(query[i].day, query[i].day, sum_time_target_left_Ag_m2);


    day_old = query[i].day;

    printf("day: %d mu_left: %f \n",query[i].day,sum_mu_target_left_Ag_m2);

    delete row;
  }

  g_smu_target_left_Ag_m2   -> SetPoint(g_smu_target_left_Ag_m2->GetN(),   day1, sum_mu_target_left_Ag_m2);
  g_stime_target_left_Ag_m2 -> SetPoint(g_stime_target_left_Ag_m2->GetN(), day1, sum_time_target_left_Ag_m2);

  delete res;
  delete query;


  //c1->cd(1);
  //g_smu_target_left_Ag_m2->Draw("L");
  
  //c1->cd(2);
  //g_stime_target_left_Ag_m2->Draw("L");















  //=======================================================================
  //                 *** Ag: Target magnet right, z=-2cm ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='right' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63774 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_right_Ag_m2 = new TGraph();
  g_smu_target_right_Ag_m2->SetName("g_smu_target_right_Ag_m2");
  g_smu_target_right_Ag_m2->SetLineWidth(2);
  g_smu_target_right_Ag_m2->SetLineColor(48);
  
  TGraph *g_stime_target_right_Ag_m2 = new TGraph();
  g_stime_target_right_Ag_m2->SetName("g_stime_target_right_Ag_m2");
  g_stime_target_right_Ag_m2->SetLineWidth(2);
  g_stime_target_right_Ag_m2->SetLineColor(48);
  
  Double_t sum_mu_target_right_Ag_m2   = 0;
  Double_t sum_time_target_right_Ag_m2 = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_right_Ag_m2   -> SetPoint(j, j, sum_mu_target_right_Ag_m2);
      g_stime_target_right_Ag_m2 -> SetPoint(j, j, sum_time_target_right_Ag_m2);      
    }
    
    sum_mu_target_right_Ag_m2   += query[i].n_muons;
    sum_time_target_right_Ag_m2 += query[i].hours;

    //g_smu_target_right   -> SetPoint(g_smu_target_right->GetN(),   query[i].day, sum_mu_target_right);
    //g_stime_target_right -> SetPoint(g_stime_target_right->GetN(), query[i].day, sum_time_target_right);

    g_smu_target_right_Ag_m2   -> SetPoint(query[i].day, query[i].day, sum_mu_target_right_Ag_m2);
    g_stime_target_right_Ag_m2 -> SetPoint(query[i].day, query[i].day, sum_time_target_right_Ag_m2);


    day_old = query[i].day;

    printf("day: %d mu_right: %f \n",query[i].day,sum_mu_target_right_Ag_m2);

    delete row;
  }

  g_smu_target_right_Ag_m2   -> SetPoint(g_smu_target_right_Ag_m2->GetN(),   day1, sum_mu_target_right_Ag_m2);
  g_stime_target_right_Ag_m2 -> SetPoint(g_stime_target_right_Ag_m2->GetN(), day1, sum_time_target_right_Ag_m2);

  delete res;
  delete query;


  //c1->cd(1);
  //g_smu_target_right_Ag_m2->Draw("L");
  
  //c1->cd(2);
  //g_stime_target_right_Ag_m2->Draw("L");






  //=======================================================================
  //                   *** Ag: Target magnet up ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='up' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63774 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_up_Ag_m2 = new TGraph();
  g_smu_target_up_Ag_m2->SetName("g_smu_target_up_Ag_m2");
  g_smu_target_up_Ag_m2->SetLineWidth(2);
  g_smu_target_up_Ag_m2->SetLineColor(46);
  
  TGraph *g_stime_target_up_Ag_m2 = new TGraph();
  g_stime_target_up_Ag_m2->SetName("g_stime_target_up_Ag_m2");
  g_stime_target_up_Ag_m2->SetLineWidth(2);
  g_stime_target_up_Ag_m2->SetLineColor(46);
  
  Double_t sum_mu_target_up_Ag_m2   = 0;
  Double_t sum_time_target_up_Ag_m2 = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_up_Ag_m2   -> SetPoint(j, j, sum_mu_target_up_Ag_m2);
      g_stime_target_up_Ag_m2 -> SetPoint(j, j, sum_time_target_up_Ag_m2);      
    }
    
    sum_mu_target_up_Ag_m2   += query[i].n_muons;
    sum_time_target_up_Ag_m2 += query[i].hours;

    //g_smu_target_left   -> SetPoint(g_smu_target_left->GetN(),   query[i].day, sum_mu_target_left);
    //g_stime_target_left -> SetPoint(g_stime_target_left->GetN(), query[i].day, sum_time_target_left);

    g_smu_target_up_Ag_m2   -> SetPoint(query[i].day, query[i].day, sum_mu_target_up_Ag_m2);
    g_stime_target_up_Ag_m2 -> SetPoint(query[i].day, query[i].day, sum_time_target_up_Ag_m2);


    day_old = query[i].day;

    printf("day: %d mu_up: %f \n",query[i].day,sum_mu_target_up_Ag_m2);

    delete row;
  }

  g_smu_target_up_Ag_m2   -> SetPoint(g_smu_target_up_Ag_m2->GetN(),   day1, sum_mu_target_up_Ag_m2);
  g_stime_target_up_Ag_m2 -> SetPoint(g_stime_target_up_Ag_m2->GetN(), day1, sum_time_target_up_Ag_m2);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_up_Ag_m2->Draw("L");
  
  c1->cd(2);
  g_stime_target_up_Ag_m2->Draw("L");







  //=======================================================================
  //                   *** Ag: Target magnet down ***
  //========================================================================

  res = db->Query("select count(r.run), sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed),"
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime)),"
		  "to_days(starttime)-to_days('2007-06-13') as date "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where "
		  "tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and target_magnet='down' "
                  "and target='Ag' "
	          "and WFD_start=230 "
		  "and time>10 and starttime > 0 and stoptime > 0 and r.run>63774 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by date");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  TGraph *g_smu_target_down_Ag_m2 = new TGraph();
  g_smu_target_down_Ag_m2->SetName("g_smu_target_down_Ag_m2");
  g_smu_target_down_Ag_m2->SetLineWidth(2);
  g_smu_target_down_Ag_m2->SetLineColor(41);
  
  TGraph *g_stime_target_down_Ag_m2 = new TGraph();
  g_stime_target_down_Ag_m2->SetName("g_stime_target_down_Ag_m2");
  g_stime_target_down_Ag_m2->SetLineWidth(2);
  g_stime_target_down_Ag_m2->SetLineColor(41);
  
  Double_t sum_mu_target_down_Ag_m2   = 0;
  Double_t sum_time_target_down_Ag_m2 = 0;

  day_old = 0;

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    for (int j=day_old+1; j<query[i].day; j++) {
      g_smu_target_down_Ag_m2   -> SetPoint(j, j, sum_mu_target_down_Ag_m2);
      g_stime_target_down_Ag_m2 -> SetPoint(j, j, sum_time_target_down_Ag_m2);      
    }
    
    sum_mu_target_down_Ag_m2   += query[i].n_muons;
    sum_time_target_down_Ag_m2 += query[i].hours;

    //g_smu_target_down   -> SetPoint(g_smu_target_down->GetN(),   query[i].day, sum_mu_target_down);
    //g_stime_target_down -> SetPoint(g_stime_target_down->GetN(), query[i].day, sum_time_target_down);

    g_smu_target_down_Ag_m2   -> SetPoint(query[i].day, query[i].day, sum_mu_target_down_Ag_m2);
    g_stime_target_down_Ag_m2 -> SetPoint(query[i].day, query[i].day, sum_time_target_down_Ag_m2);


    day_old = query[i].day;

    printf("day: %d mu_down: %f \n",query[i].day,sum_mu_target_down_Ag_m2);

    delete row;
  }

  g_smu_target_down_Ag_m2   -> SetPoint(g_smu_target_down_Ag_m2->GetN(),   day1, sum_mu_target_down_Ag_m2);
  g_stime_target_down_Ag_m2 -> SetPoint(g_stime_target_down_Ag_m2->GetN(), day1, sum_time_target_down_Ag_m2);

  delete res;
  delete query;


  c1->cd(1);
  g_smu_target_down_Ag_m2->Draw("L");
  
  c1->cd(2);
  g_stime_target_down_Ag_m2->Draw("L");




  //=========================================================================================
  //
  //
  //       *** General statistics overview since repositioning of the ball ***
  //
  //
  //==========================================================================================

  struct s_stat_info {
    int   nruns  [4]; /*** left right up down ***/
    float nhours [4];
    float nmuons [4];
    void init() {
      for (int i=0; i<4; i++) {
	nruns[i] = 0;
	nhours[i] = nmuons[i] = 0;
      }
    }
  };

  s_stat_info stat_Quartz, stat_Ag, stat_AK3;

  stat_Quartz.init();
  stat_Ag.init();
  stat_AK3.init();

  res = db->Query("select "
		  "count(r.run),"
                  "sum(N*tau*0.451*0.776/20.0/1.0e10*nsegments/nevents_analyzed), "
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime))/3600.0 as hours, "
		  "target, "
		  "target_magnet "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and WFD_start=230 "
		  "and time>10 "
		  "and starttime > 0 "
		  "and stoptime > 0 "
		  "and r.run>63774 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by target,target_magnet");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();

    char str_target[512];
    strcpy(str_target,row->GetField(3));

    char str_magnet[512];
    strcpy(str_magnet,row->GetField(4));

    int i_magnet = -1;

    if ( !strcmp(str_magnet,"left") ) 
      i_magnet = 0;
    else if ( !strcmp(str_magnet,"right") ) 
      i_magnet = 1;
    else if ( !strcmp(str_magnet,"up")) 
      i_magnet = 2;
    else if ( !strcmp(str_magnet,"down") )
      i_magnet = 3;
    else
      continue;


    s_stat_info *tgt = NULL;

    if ( !strcmp(str_target,"Quartz") )
      tgt = &stat_Quartz;
    else if ( !strcmp(str_target,"Ag") )
      tgt = &stat_Ag;
    else if ( !strcmp(str_target,"AK3") )
      tgt = &stat_AK3;
    
    if ( i_magnet >= 0 && tgt != NULL) {
      tgt->nruns [i_magnet] = atoi(row->GetField(0));
      tgt->nmuons[i_magnet] = atof(row->GetField(1));
      tgt->nhours[i_magnet] = atof(row->GetField(2));
    }

    delete row;
  }

  delete res;
  delete query;


  //=========================================================================================
  //
  //
  //       *** General statistics before the ball was moved by 2cm upstream ***
  //
  //
  //==========================================================================================

  s_stat_info stat_Quartz_old, stat_Ag_old;

  stat_Quartz_old.init();
  stat_Ag_old.init();

  res = db->Query("select "
		  "count(r.run),"
                  "sum(N*tau*0.451*0.776/20.0/1.0e10*nsegments/nevents_analyzed), "
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime))/3600.0 as hours, "
		  "target, "
		  "target_magnet "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and WFD_start=230 "
		  "and time>10 "
		  "and starttime > 0 "
		  "and stoptime > 0 "
		  "and r.run<63775 and ( (r.run>63230 and target='Ag') or (target!='Ag')) "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "group by target,target_magnet");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();

    char str_target[512];
    strcpy(str_target,row->GetField(3));

    char str_magnet[512];
    strcpy(str_magnet,row->GetField(4));

    int i_magnet = -1;

    if ( !strcmp(str_magnet,"left") ) 
      i_magnet = 0;
    else if ( !strcmp(str_magnet,"right") ) 
      i_magnet = 1;
    else if ( !strcmp(str_magnet,"up")) 
      i_magnet = 2;
    else if ( !strcmp(str_magnet,"down") )
      i_magnet = 3;
    else
      continue;


    s_stat_info *tgt = NULL;

    if ( !strcmp(str_target,"Quartz") )
      tgt = &stat_Quartz_old;
    else if ( !strcmp(str_target,"Ag") )
      tgt = &stat_Ag_old;
    
    if ( i_magnet >= 0 && tgt != NULL) {
      tgt->nruns [i_magnet] = atoi(row->GetField(0));
      tgt->nmuons[i_magnet] = atof(row->GetField(1));
      tgt->nhours[i_magnet] = atof(row->GetField(2));
    }

    delete row;
  }

  delete res;
  delete query;





  //=========================================================================================
  //
  //
  //                  *** Quartz target, +/- 15 deg. magnet rotation ***
  //
  //
  //==========================================================================================

  s_stat_info stat_Quartz_15;

  stat_Quartz_15.init();

  res = db->Query("select "
		  "count(r.run),"
                  "sum(N*tau*0.451*0.776/20.0/1.0e10*nsegments/nevents_analyzed), "
		  "sum(unix_timestamp(stoptime)-unix_timestamp(starttime))/3600.0 as hours, "
		  "target, "
		  "target_magnet "
		  "from Runlog as r "
		  "left join RunInfo using (run) "
		  "left join RunScalers using (run) "
		  "where tau between 1900 and 2400 "
		  "and quality='Y' "
		  "and WFD_start=230 "
		  "and time>10 "
		  "and starttime > 0 "
		  "and stoptime > 0 "
		  "and r.run>63774 "
		  "and nevents_analyzed>0 "
		  "and nsegments  is not null "
		  "and target='quartz' "
		  "group by target,target_magnet");
  
  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  query = new s_query[nrows];  

  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();

    char str_target[512];
    strcpy(str_target,row->GetField(3));

    char str_magnet[512];
    strcpy(str_magnet,row->GetField(4));

    int i_magnet = -1;

    if ( !strcmp(str_magnet,"15LU") ) 
      i_magnet = 0;
    else if ( !strcmp(str_magnet,"15LD") ) 
      i_magnet = 1;
    else if ( !strcmp(str_magnet,"15RU")) 
      i_magnet = 2;
    else if ( !strcmp(str_magnet,"15RD") )
      i_magnet = 3;
    else
      continue;


    s_stat_info *tgt = NULL;

    if ( !strcmp(str_target,"quartz") )
      tgt = &stat_Quartz_15;
    //else if ( !strcmp(str_target,"Ag") )
    //  tgt = &stat_Ag_old;
    else
      continue;
    
    if ( i_magnet >= 0 && tgt != NULL) {
      tgt->nruns [i_magnet] = atoi(row->GetField(0));
      tgt->nmuons[i_magnet] = atof(row->GetField(1));
      tgt->nhours[i_magnet] = atof(row->GetField(2));
    }

    delete row;
  }

  delete res;
  delete query;















  //=======================================================================
  //                   *** Plot comments ***
  //========================================================================

  TLegend  *leg;
  
  c1->cd(1);
  
  leg = new TLegend(0.139,0.350,0.541,0.900,"target magnet:", "NDC");
  leg->AddEntry(g_smu_target_left,"left","l");
  leg->AddEntry(g_smu_target_right,"right","l");
  leg->AddEntry(g_smu_target_up,"up","l");
  leg->AddEntry(g_smu_target_down,"down","l");
  leg->AddEntry(g_smu_target_left_Ag,"Ag, left","l");
  leg->AddEntry(g_smu_target_right_Ag,"Ag, right","l");
  leg->AddEntry(g_smu_target_up_Ag,"Ag, up","l");
  leg->AddEntry(g_smu_target_down_Ag,"Ag, down","l");
  leg->AddEntry(smu,"sum","l");
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->Draw();

  c1->cd(2);
  
  leg = new TLegend(0.139,0.350,0.541,0.900,"target magnet:", "NDC");
  leg->AddEntry(g_stime_target_left,"left","l");
  leg->AddEntry(g_stime_target_right,"right","l");
  leg->AddEntry(g_stime_target_up,"up","l");
  leg->AddEntry(g_stime_target_down,"down","l");
  leg->AddEntry(g_stime_target_left_Ag,"Ag, left","l");
  leg->AddEntry(g_stime_target_right_Ag,"Ag, right","l");
  leg->AddEntry(g_stime_target_up_Ag,"Ag, up","l");
  leg->AddEntry(g_stime_target_down_Ag,"Ag, down","l");
  leg->AddEntry(stime,"sum","l");
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  leg->Draw();

  delete db;
  
  c1->Update();

  c1->SaveAs("../png/overview.png");

  //================================================================================
  //             *** HTML output ****
  //================================================================================
  ofstream *os = new ofstream ("../html/overview.html");
  *os << "<font size=\"+2\" color=\"#FF0000\">Good muons in this plot are muons "
      << "integrated over WFD clock between 250 to 9500; only good runs (labeled as quality='Y') "
      << "are considered; "
      << "background (B term in the fit) is excluded. "
      << "</font>"
      << "<hr>";
  
  //===============================================================================
  //            *** print statistics overview ***
  //===============================================================================
  
  os->precision(1);
  *os << "<h3>Statistics overview</h3>"
      << "<b>total number of good muons:</b> <span style='color: blue;'>" << scientific << sum_mu*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking:</b> <span style='color: blue;'>" <<  int(sum_time) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_left':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_left*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_left':</b> <span style='color: blue;'>" << int(sum_time_target_left) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_right':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_right*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_right':</b> <span style='color: blue;'>" << int(sum_time_target_right) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_up':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_up*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_up':</b> <span style='color: blue;'>" << int(sum_time_target_up) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_down':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_down*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_down':</b> <span style='color: blue;'>" << int(sum_time_target_down) << "</span><br>"
      << "<h4>Ag target:</h4>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_left':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_left_Ag*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_left':</b> <span style='color: blue;'>" << int(sum_time_target_left_Ag) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_right':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_right_Ag*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_right':</b> <span style='color: blue;'>" << int(sum_time_target_right_Ag) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_up':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_up_Ag*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_up':</b> <span style='color: blue;'>" << int(sum_time_target_up_Ag) << "</span><br>"
      << "<b>total number of good muons collected with magnet orientantion 'beam_down':</b> <span style='color: blue;'>" <<  scientific << sum_mu_target_down_Ag*1.0e9 << "</span><br>"
      << "<b>total hours of production data taking with magnet orientantion 'beam_down':</b> <span style='color: blue;'>" << int(sum_time_target_down_Ag) << "</span><br>"
      << "<b>total number of good muons:</b> <span style='color: blue;'>" <<  scientific << (sum_mu_target_left_Ag+sum_mu_target_right_Ag+sum_mu_target_up_Ag+sum_mu_target_down_Ag)*1.0e9 << "</span><br>"
      << "<b>total hours:</b> <span style='color: blue;'>" << int(sum_time_target_left_Ag+sum_time_target_right_Ag+sum_time_target_up_Ag+sum_time_target_down_Ag) << "</span><br>"
      << endl;

  os->precision(2);
  *os << "<h3>Statistics overview before ball displacement by -2cm (upstream) (before 01-Jul-2007, run < 63775)</h3>"
      << "<table align='center' rules='all'>"
      << " <tr>"
      << "  <th> target </th>"
      << "  <th> magnet </th>"
      << "  <th> N muons </th>"
      << "  <th> N runs </th>"
      << "  <th> N hours </th>"
      << " </tr>"
      << " </tr>";

  *os << " <tr align='center'><td rowspan=4>Ag</td><td>left</td><td>"  << fixed << stat_Ag_old.nmuons[0] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[0] << "</td><td>" << stat_Ag_old.nhours[0] <<  "</td></tr>"
      << " <tr align='center'><td>right</td><td>" << fixed << stat_Ag_old.nmuons[1] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[1] << "</td><td>" << stat_Ag_old.nhours[1] <<  "</td></tr>"
      << " <tr align='center'><td>up</td><td>"    << fixed << stat_Ag_old.nmuons[2] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[2] << "</td><td>" << stat_Ag_old.nhours[2] <<  "</td></tr>"
      << " <tr align='center'><td>down</td><td>"  << fixed << stat_Ag_old.nmuons[3] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[3] << "</td><td>" << stat_Ag_old.nhours[3] <<  "</td></tr>";
  
  *os << " <tr align='center'><td rowspan=4> Quartz </td><td>left</td><td>"  << fixed << stat_Quartz_old.nmuons[0] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[0] << "</td><td>" << stat_Quartz_old.nhours[0] <<  "</td></tr>"
      << " <tr align='center'><td>right</td><td>" << fixed << stat_Quartz_old.nmuons[1] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[1] << "</td><td>" << stat_Quartz_old.nhours[1] <<  "</td></tr>"
      << " <tr align='center'><td>up</td><td>"    << fixed << stat_Quartz_old.nmuons[2] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[2] << "</td><td>" << stat_Quartz_old.nhours[2] <<  "</td></tr>"
      << " <tr align='center'><td>down</td><td>"  << fixed << stat_Quartz_old.nmuons[3] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[3] << "</td><td>" << stat_Quartz_old.nhours[3] <<  "</td></tr>"
      << "</table>"
      << endl;

  os->precision(2);
  *os << "<h3>Statistics overview since ball displacement by -2cm (upstream) (01-Jul-2007, run 63775)</h3>"
      << "<table align='center' rules='all'>"
      << " <tr>"
      << "  <th> target </th>"
      << "  <th> magnet </th>"
      << "  <th> N muons </th>"
      << "  <th> N runs </th>"
      << "  <th> N hours </th>"
      << " </tr>"
      << " </tr>";

  *os << " <tr align='center'><td rowspan=4>Ag</td><td>left</td><td>"  << fixed << stat_Ag.nmuons[0] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[0] << "</td><td>" << stat_Ag.nhours[0] <<  "</td></tr>"
      << " <tr align='center'><td>right</td><td>" << fixed << stat_Ag.nmuons[1] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[1] << "</td><td>" << stat_Ag.nhours[1] <<  "</td></tr>"
      << " <tr align='center'><td>up</td><td>"    << fixed << stat_Ag.nmuons[2] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[2] << "</td><td>" << stat_Ag.nhours[2] <<  "</td></tr>"
      << " <tr align='center'><td>down</td><td>"  << fixed << stat_Ag.nmuons[3] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Ag.nruns[3] << "</td><td>" << stat_Ag.nhours[3] <<  "</td></tr>";
  
  *os << " <tr align='center'><td rowspan=8> Quartz </td><td>left</td><td>"  << fixed << stat_Quartz.nmuons[0] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[0] << "</td><td>" << stat_Quartz.nhours[0] <<  "</td></tr>"
      << " <tr align='center'><td>right</td><td>" << fixed << stat_Quartz.nmuons[1] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[1] << "</td><td>" << stat_Quartz.nhours[1] <<  "</td></tr>"
      << " <tr align='center'><td>up</td><td>"    << fixed << stat_Quartz.nmuons[2] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[2] << "</td><td>" << stat_Quartz.nhours[2] <<  "</td></tr>"
      << " <tr align='center'><td>down</td><td>"  << fixed << stat_Quartz.nmuons[3] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz.nruns[3] << "</td><td>" << stat_Quartz.nhours[3] <<  "</td></tr>";

  *os << " <tr align='center'><td>15&deg; left-up</td><td>"    << fixed << stat_Quartz_15.nmuons[0] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz_15.nruns[0] << "</td><td>" << stat_Quartz_15.nhours[0] <<  "</td></tr>"
      << " <tr align='center'><td>15&deg; left-down</td><td>"  << fixed << stat_Quartz_15.nmuons[1] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz_15.nruns[1] << "</td><td>" << stat_Quartz_15.nhours[1] <<  "</td></tr>"
      << " <tr align='center'><td>15&deg; right-up</td><td>"   << fixed << stat_Quartz_15.nmuons[2] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz_15.nruns[2] << "</td><td>" << stat_Quartz_15.nhours[2] <<  "</td></tr>"
      << " <tr align='center'><td>15&deg; right-down</td><td>" << fixed << stat_Quartz_15.nmuons[3] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_Quartz_15.nruns[3] << "</td><td>" << stat_Quartz_15.nhours[3] <<  "</td></tr>";


  *os << " <tr align='center'><td rowspan=4>AK3</td><td>left</td><td>"  << fixed << stat_AK3.nmuons[0] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_AK3.nruns[0] << "</td><td>" << stat_AK3.nhours[0] <<  "</td></tr>"
      << " <tr align='center'><td>right</td><td>" << fixed << stat_AK3.nmuons[1] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_AK3.nruns[1] << "</td><td>" << stat_AK3.nhours[1] <<  "</td></tr>"
      << " <tr align='center'><td>up</td><td>"    << fixed << stat_AK3.nmuons[2] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_AK3.nruns[2] << "</td><td>" << stat_AK3.nhours[2] <<  "</td></tr>"
      << " <tr align='center'><td>down</td><td>"  << fixed << stat_AK3.nmuons[3] << "&times;10<sup>10</sup></td>" << "</td><td>"<< stat_AK3.nruns[3] << "</td><td>" << stat_AK3.nhours[3] <<  "</td></tr>";
  
  *os << "</table>"
      << endl;


  delete os;
  
  
}
