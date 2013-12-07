#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <math.h>

#include "../common/init.C"

#include "TGraph.h"
#include "TCanvas.h"
#include "TVirtualPad.h"
#include "TDatime.h"

#define FORMAT   "%04d%02d%02d%02d%02d%02d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t%8d\t"

#define FE_NAME           "DAWN_monitor"
#define MAX_LINE_LEN      102400
#define NUMBER_OF_CRATES  6

struct RECORD {

  char name[5];
  int demand;
  float measured;

};

RECORD record;


void set_min(RECORD *dst, RECORD *src)
{

#define SETMIN(a) { dst->a = dst->a < src->a ? dst->a : src->a; }

  SETMIN(demand);
  SETMIN(measured);
}

void set_max(RECORD *dst, RECORD *src)
{

#define SETMAX(a) { dst->a = dst->a > src->a ? dst->a : src->a; }

  SETMAX(demand);
  SETMAX(measured);
}


void BEAM(const int rollback)
{

  const int time_offset = -2;

  TGraph *gr_temp1  [NUMBER_OF_CRATES];

  RECORD par_min    [NUMBER_OF_CRATES];
  RECORD par_max    [NUMBER_OF_CRATES];

  int colors[NUMBER_OF_CRATES] = {kBlue,kRed,kBlack,kGreen,kMagenta,46};

  TDatime mytime;


  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);


  //=========================================================
  //                  *** create canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,1000);
  c1->Draw();
  c1->cd();

  c1->Divide(2,4,0.001,0.001,10);
  c1->Update();

  char name[256];

  float markersize = 0.3;

  int i;  
  /*** Set names ***/
  for (i=0; i<NUMBER_OF_CRATES; i++) {
    sprintf(name,"temp1_crate_%d",i+1);
    gr_temp1[i] = new TGraph(1024);
    gr_temp1[i]->SetName(name); 
    gr_temp1[i]->SetTitle("temp1"); 
    gr_temp1[i]->SetMarkerStyle(21);
    gr_temp1[i]->SetMarkerSize(markersize);
    gr_temp1[i]->SetMarkerColor(colors[i]);

    par_min[i].crate = par_max[i].crate = i+1;
    
    par_min[i].demand = 
      par_min[i].measured = 123456;
       
    par_max[i].demand = 
      par_max[i].measured = -123456;

  }

  
  /*** actual time, UTC ***/
  time_t time_now;  
  time( &time_now );

  printf("time_utc_now: %d -> %s\n",time_now, ctime( &time_now ));

  /*** local UTC time in tm structure ***/
  struct tm time_now_tm;
  gmtime_r( &time_now, &time_now_tm );

  printf("time_utc_now_tm: %04d-%02d-%02d %02d:%02d:%02d  -> %s\n",
	 1900+time_now_tm.tm_year,
	 time_now_tm.tm_mon+1,
	 time_now_tm.tm_mday,
	 time_now_tm.tm_hour,
	 time_now_tm.tm_min,
	 time_now_tm.tm_sec,
	 asctime(&time_now_tm));

  /*** start time, UTC ***/
  time_t time_start = time_now - rollback*60*60;
  if ( rollback == 0 )
    time_start = 1152650714;

  tm time_start_tm;
  gmtime_r( &time_start, &time_start_tm );

  time_t time_i = time_start;
  tm time_i_tm;
  gmtime_r( &time_i, &time_i_tm );

  char *line = new char[MAX_LINE_LEN];

//  do {
//
//    char is_name[1024];
//    sprintf(is_name,"../slowdata/%04d%02d%02d-%s.dat",
//	    time_i_tm.tm_year+1900,
//	    time_i_tm.tm_mon+1,
//	    time_i_tm.tm_mday,
//	    FE_NAME);
//    
//    printf("ifname: %s\n",is_name);
//    
//    ifstream *is = new ifstream( is_name);
//    while ( is->good() ) {
//
//      is->getline(line,MAX_LINE_LEN);
//
//      if ( !strncmp(line,"#names:",7) ) {
//	/*** list of parameters ***/
//      } else if ( !strncmp(line,"#format:",7) ) {
//	/*** format line ***/
//      } else if ( line[0] == '#') {
//	/*** comment line ***/
//      } else {
//	/*** data line ***/
//	
//	sscanf(line, FORMAT, 
//	       &record.year,
//	       &record.mon,
//	       &record.day,
//	       &record.hour,
//	       &record.min,
//	       &record.sec,
//	       
//	       &record.crate,
//	       &record.temp1,
//	       &record.temp2,
//	       &record.temp3,
//	       &record.V_3,
//	       &record.V_5,
//	       &record.V_12, 
//	       &record.V_M5,
//	       &record.F1_RPM,
//	       &record.F2_RPM,
//	       &record.F3_RPM,
//	       &record.F4_RPM,
//	       &record.F5_RPM,
//	       &record.F6_RPM );
//	//printf("%s\n",line);
//
//	//printf("crate :%d\n",record.crate);
//
//	tm x_tm_utc;
//	x_tm_utc.tm_year  = record.year - 1900;
//	x_tm_utc.tm_mon   = record.mon  - 1;
//	x_tm_utc.tm_mday  = record.day;
//	x_tm_utc.tm_hour  = record.hour - time_offset;
//	x_tm_utc.tm_min   = record.min;
//	x_tm_utc.tm_sec   = record.sec;
//
//	/*** make time_t time ***/
//	time_t x_utc = mktime(&x_tm_utc);
//
//	if ( x_utc < time_start ) continue;
//
//	if ( record.crate >= 1 && record.crate <= NUMBER_OF_CRATES ) {
//	  i = record.crate - 1;
//
//	  /*** bad temperature sensor ***/
//	  if ( record.temp3 == 209 )
//	    record.temp3 = 123456;
//
//	  gr_temp1[i]->SetPoint(gr_temp1[i]->GetN(),x_utc,record.temp1);
//
//	  set_min(par_min+i, &record);
//	  set_max(par_max+i, &record);
//	}
//      }
//    } 
//    
//    delete is;    
//    
//    time_i += 86400; /*** seconds in day = 24*60*60 ***/
//    gmtime_r( &time_i, &time_i_tm );
//    
//  } while(time_i_tm.tm_year*10000 + time_i_tm.tm_mon*100 + time_i_tm.tm_mday <=
//	  time_now_tm.tm_year*10000 + time_now_tm.tm_mon*100 + time_now_tm.tm_mday );
  
  
  delete line;
  
  printf("time_start: %d\n",time_start);

  TH1F *frame;
  TVirtualPad *pad;

  time_t tmin = time_start - 200;
  time_t tmax = time_now + 200;// + 2*3600;

  Double_t ymin, ymax;

  //===========================================================
  //             *** plot graphs ***
  //===========================================================


  c1->Update();

#define SETMINMAX(par) { \
    p_min = par_min[i].par;\
    p_max = par_max[i].par;\
\
    if (fabs(p_min) != 123456) {\
      ymin = p_min < ymin ? p_min - 1 : ymin;\
    }\
\
    if (fabs(p_max) != 123456) {\
      ymax = p_max > ymax ? p_max + 1 : ymax;\
    } \
}

#define DRAWFRAME(title,ytitle) {\
  pad->cd();\
  pad->SetBottomMargin(0.1);\
  frame = pad->DrawFrame(tmin,ymin,tmax,ymax,title);\
  frame->GetXaxis()->SetTimeDisplay(1);\
  frame->GetXaxis()->SetTimeFormat("%H:%M");\
  frame->GetXaxis()->SetTimeOffset(time_offset);\
  frame->GetXaxis()->SetLabelSize(0.045);\
  frame->GetXaxis()->ChooseTimeFormat(0.1);\
  frame->GetXaxis()->SetTitleOffset(1.0);\
  frame->GetYaxis()->SetTitleOffset(1.1);\
  frame->SetXTitle("time");\
  frame->SetYTitle(ytitle);\
  frame->GetXaxis()->SetTitleSize(0.055);\
  frame->GetXaxis()->SetLabelSize(0.05);\
  frame->GetYaxis()->SetTitleOffset(0.65);\
  frame->GetYaxis()->SetTitleSize(0.055);\
  frame->GetYaxis()->SetLabelSize(0.05);\
  frame->GetYaxis()->SetTitleOffset(0.65);\
}


 Double_t p_min, p_max;

  //===========================================================
  //             *** temp1 ***
  //===========================================================
    
  pad = c1->GetPad(1);
  
  pad->cd();
  
  ymin = 20;
  ymax = 45;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(temp1);
  }

  DRAWFRAME("temp1","Temperature (C)");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_temp1[i]->Draw("P");
  }

  
  //===========================================================
  //             *** temp2 ***
  //===========================================================
    
  pad = c1->GetPad(2);

  ymin = 20;
  ymax = 45;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(temp2);
  }
    
  DRAWFRAME("temp2","Temperature (C)");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_temp2[i]->Draw("P");
  }


  //===========================================================
  //             *** temp3 ***
  //===========================================================
    
  pad = c1->GetPad(3);

  ymin = 20;
  ymax = 45;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(temp3);
  }

  DRAWFRAME("temp3","Temperature (C)");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_temp3[i]->Draw("P");
  }

  //===========================================================
  //             *** V_3 ***
  //===========================================================
    
  pad = c1->GetPad(4);

  ymin = 3100;
  ymax = 3500;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(V_3);
  }

  /*** millivolts -> volts ***/
  ymin /= 1000;
  ymax /= 1000;

  DRAWFRAME("3.3V","V");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_V_3[i]->Draw("P");
  }


  //===========================================================
  //             *** V_5 ***
  //===========================================================
  
  pad = c1->GetPad(5);

  ymin = 4700;
  ymax = 5200;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(V_5);
  }

  /*** millivolts -> volts ***/
  ymin /= 1000;
  ymax /= 1000;

  DRAWFRAME("+5V","V");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_V_5[i]->Draw("P");
  }


  //===========================================================
  //             *** V_M5 ***
  //===========================================================
    
  pad = c1->GetPad(6);

  ymin = 4700;
  ymax = 5200;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(V_M5);
  }

  /*** millivolts -> volts ***/
  ymin /= 1000;
  ymax /= 1000;

  DRAWFRAME("-5V","-V");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_V_M5[i]->Draw("P");
  }


  //===========================================================
  //             *** V_12 ***
  //===========================================================
    
  pad = c1->GetPad(7);

  ymin = 11500;
  ymax = 12500;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(V_12);
  }

  /*** millivolts -> volts ***/
  ymin /= 1000;
  ymax /= 1000;

  DRAWFRAME("12V","V");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_V_12[i]->Draw("P");
  }


  //===========================================================
  //             *** Fans ***
  //===========================================================
    
  pad = c1->GetPad(8);

  ymin = 2800.0;
  ymax = 4000.0;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    SETMINMAX(F1_RPM);
    SETMINMAX(F2_RPM);
    SETMINMAX(F3_RPM);
    SETMINMAX(F4_RPM);
    SETMINMAX(F5_RPM);
    SETMINMAX(F6_RPM);
  }
  
  DRAWFRAME("fans","Fan speed (RPM)");

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    gr_F1_RPM[i]->Draw("P");
    gr_F2_RPM[i]->Draw("P");
    gr_F3_RPM[i]->Draw("P");
    gr_F4_RPM[i]->Draw("P");
    gr_F5_RPM[i]->Draw("P");
    gr_F6_RPM[i]->Draw("P");
  }
  
  c1->Update();
  
  //c1->SaveAs("../png/BEAM.png");

  //==========================================================
  //           *** print min. and max. values ***
  //==========================================================

  /***
  for (i=0; i<NUMBER_OF_CRATES; i++) {
    printf(par_min[i].temp1,par_max[i].temp1);
  }  
  ***/

  ofstream *os = new ofstream("../html/BEAM.html");

  *os << "<h2> In progress.... </h2>\n";

  delete os;

}
