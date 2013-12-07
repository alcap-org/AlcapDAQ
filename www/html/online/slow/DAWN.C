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

  int year;
  int mon;
  int day;
  int hour;
  int min;
  int sec;

  int crate;
  int temp1;
  int temp2;
  int temp3;
  int V_3;
  int V_5;
  int V_12; 
  int V_M5;
  int F1_RPM;
  int F2_RPM;
  int F3_RPM;
  int F4_RPM;
  int F5_RPM;
  int F6_RPM;
};

RECORD record;


void set_min(RECORD *dst, RECORD *src)
{

#define SETMIN(a) { dst->a = dst->a < src->a ? dst->a : src->a; }

  SETMIN(temp1);
  SETMIN(temp2);
  SETMIN(temp3);
  SETMIN(V_3);
  SETMIN(V_5);
  SETMIN(V_12);
  SETMIN(V_M5);
  SETMIN(F1_RPM);
  SETMIN(F2_RPM);
  SETMIN(F3_RPM);
  SETMIN(F4_RPM);
  SETMIN(F5_RPM);
  SETMIN(F6_RPM);

}

void set_max(RECORD *dst, RECORD *src)
{

#define SETMAX(a) { dst->a = dst->a > src->a ? dst->a : src->a; }

  SETMAX(temp1);
  SETMAX(temp2);
  SETMAX(temp3);
  SETMAX(V_3);
  SETMAX(V_5);
  SETMAX(V_12);
  SETMAX(V_M5);
  SETMAX(F1_RPM);
  SETMAX(F2_RPM);
  SETMAX(F3_RPM);
  SETMAX(F4_RPM);
  SETMAX(F5_RPM);
  SETMAX(F6_RPM);

}


void DAWN(const int rollback)
{

  const int time_offset = -2;

  TGraph *gr_temp1  [NUMBER_OF_CRATES];
  TGraph *gr_temp2  [NUMBER_OF_CRATES];
  TGraph *gr_temp3  [NUMBER_OF_CRATES];
  TGraph *gr_V_3    [NUMBER_OF_CRATES];
  TGraph *gr_V_5    [NUMBER_OF_CRATES];
  TGraph *gr_V_12   [NUMBER_OF_CRATES];
  TGraph *gr_V_M5   [NUMBER_OF_CRATES];
  TGraph *gr_F1_RPM [NUMBER_OF_CRATES];
  TGraph *gr_F2_RPM [NUMBER_OF_CRATES];
  TGraph *gr_F3_RPM [NUMBER_OF_CRATES];
  TGraph *gr_F4_RPM [NUMBER_OF_CRATES];
  TGraph *gr_F5_RPM [NUMBER_OF_CRATES];
  TGraph *gr_F6_RPM [NUMBER_OF_CRATES];

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

    sprintf(name,"temp2_crate_%d",i+1);
    gr_temp2[i] = new TGraph(1024);
    gr_temp2[i]->SetName(name); 
    gr_temp2[i]->SetTitle("temp2"); 
    gr_temp2[i]->SetMarkerStyle(21);
    gr_temp2[i]->SetMarkerSize(markersize);
    gr_temp2[i]->SetMarkerColor(colors[i]);

    
    sprintf(name,"temp3_crate_%d",i+1);
    gr_temp3[i] = new TGraph(1024);
    gr_temp3[i]->SetName(name); 
    gr_temp3[i]->SetTitle("temp3"); 
    gr_temp3[i]->SetMarkerStyle(21);
    gr_temp3[i]->SetMarkerSize(markersize);
    gr_temp3[i]->SetMarkerColor(colors[i]);

    sprintf(name,"V_3_crate_%d",i+1);
    gr_V_3[i] = new TGraph(1024);
    gr_V_3[i]->SetName(name); 
    gr_V_3[i]->SetTitle("+3V"); 
    gr_V_3[i]->SetMarkerStyle(21);
    gr_V_3[i]->SetMarkerSize(markersize);
    gr_V_3[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"V_5_crate_%d",i+1);
    gr_V_5[i] = new TGraph(1024);
    gr_V_5[i]->SetName(name); 
    gr_V_5[i]->SetTitle("+5V"); 
    gr_V_5[i]->SetMarkerStyle(21);
    gr_V_5[i]->SetMarkerSize(markersize);
    gr_V_5[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"V_12_crate_%d",i+1);
    gr_V_12[i] = new TGraph(1024);
    gr_V_12[i]->SetName(name); 
    gr_V_12[i]->SetTitle("12V"); 
    gr_V_12[i]->SetMarkerStyle(21);
    gr_V_12[i]->SetMarkerSize(markersize);
    gr_V_12[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"V_M5_crate_%d",i+1);
    gr_V_M5[i] = new TGraph(1024);
    gr_V_M5[i]->SetName(name); 
    gr_V_M5[i]->SetTitle("-5V"); 
    gr_V_M5[i]->SetMarkerStyle(21);
    gr_V_M5[i]->SetMarkerSize(markersize);
    gr_V_M5[i]->SetMarkerColor(colors[i]);
    

    sprintf(name,"F1_RPM_crate_%d",i+1);
    gr_F1_RPM[i] = new TGraph(1024);
    gr_F1_RPM[i]->SetName(name); 
    gr_F1_RPM[i]->SetTitle("F1, RPM"); 
    gr_F1_RPM[i]->SetMarkerStyle(21);
    gr_F1_RPM[i]->SetMarkerSize(markersize);
    gr_F1_RPM[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"F2_RPM_crate_%d",i+1);
    gr_F2_RPM[i] = new TGraph(1024);
    gr_F2_RPM[i]->SetName(name); 
    gr_F2_RPM[i]->SetTitle("F2, RPM"); 
    gr_F2_RPM[i]->SetMarkerStyle(21);
    gr_F2_RPM[i]->SetMarkerSize(markersize);
    gr_F2_RPM[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"F3_RPM_crate_%d",i+1);
    gr_F3_RPM[i] = new TGraph(1024);
    gr_F3_RPM[i]->SetName(name); 
    gr_F3_RPM[i]->SetTitle("F3, RPM"); 
    gr_F3_RPM[i]->SetMarkerStyle(21);
    gr_F3_RPM[i]->SetMarkerSize(markersize);
    gr_F3_RPM[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"F4_RPM_crate_%d",i+1);
    gr_F4_RPM[i] = new TGraph(1024);
    gr_F4_RPM[i]->SetName(name); 
    gr_F4_RPM[i]->SetTitle("F4, RPM"); 
    gr_F4_RPM[i]->SetMarkerStyle(21);
    gr_F4_RPM[i]->SetMarkerSize(markersize);
    gr_F4_RPM[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"F5_RPM_crate_%d",i+1);
    gr_F5_RPM[i] = new TGraph(1024);
    gr_F5_RPM[i]->SetName(name); 
    gr_F5_RPM[i]->SetTitle("F5, RPM"); 
    gr_F5_RPM[i]->SetMarkerStyle(21);
    gr_F5_RPM[i]->SetMarkerSize(markersize);
    gr_F5_RPM[i]->SetMarkerColor(colors[i]);
    
    sprintf(name,"F6_RPM_crate_%d",i+1);
    gr_F6_RPM[i] = new TGraph(1024);
    gr_F6_RPM[i]->SetName(name); 
    gr_F6_RPM[i]->SetTitle("F6, RPM"); 
    gr_F6_RPM[i]->SetMarkerStyle(21);
    gr_F6_RPM[i]->SetMarkerSize(markersize);
    gr_F6_RPM[i]->SetMarkerColor(colors[i]);
    
    par_min[i].crate = par_max[i].crate = i+1;
    
    par_min[i].temp1 = 
      par_min[i].temp2 = 
      par_min[i].temp3 = 
      par_min[i].V_3 = 
      par_min[i].V_5 = 
      par_min[i].V_12 = 
      par_min[i].V_M5 = 
      par_min[i].F1_RPM = 
      par_min[i].F2_RPM = 
      par_min[i].F3_RPM = 
      par_min[i].F4_RPM = 
      par_min[i].F5_RPM = 
      par_min[i].F6_RPM = 123456;
       
    par_max[i].temp1 = 
      par_max[i].temp2 = 
      par_max[i].temp3 = 
      par_max[i].V_3 = 
      par_max[i].V_5 = 
      par_max[i].V_12 = 
      par_max[i].V_M5 = 
      par_max[i].F1_RPM = 
      par_max[i].F2_RPM = 
      par_max[i].F3_RPM = 
      par_max[i].F4_RPM = 
      par_max[i].F5_RPM = 
      par_max[i].F6_RPM = -123456;

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

  do {

    char is_name[1024];
    sprintf(is_name,"../slowdata/%04d%02d%02d-%s.dat",
	    time_i_tm.tm_year+1900,
	    time_i_tm.tm_mon+1,
	    time_i_tm.tm_mday,
	    FE_NAME);
    
    printf("ifname: %s\n",is_name);
    
    ifstream *is = new ifstream( is_name);
    while ( is->good() ) {

      is->getline(line,MAX_LINE_LEN);

      if ( !strncmp(line,"#names:",7) ) {
	/*** list of parameters ***/
      } else if ( !strncmp(line,"#format:",7) ) {
	/*** format line ***/
      } else if ( line[0] == '#') {
	/*** comment line ***/
      } else {
	/*** data line ***/
	
	sscanf(line, FORMAT, 
	       &record.year,
	       &record.mon,
	       &record.day,
	       &record.hour,
	       &record.min,
	       &record.sec,
	       
	       &record.crate,
	       &record.temp1,
	       &record.temp2,
	       &record.temp3,
	       &record.V_3,
	       &record.V_5,
	       &record.V_12, 
	       &record.V_M5,
	       &record.F1_RPM,
	       &record.F2_RPM,
	       &record.F3_RPM,
	       &record.F4_RPM,
	       &record.F5_RPM,
	       &record.F6_RPM );
	//printf("%s\n",line);

	//printf("crate :%d\n",record.crate);

	tm x_tm_utc;
	x_tm_utc.tm_year  = record.year - 1900;
	x_tm_utc.tm_mon   = record.mon  - 1;
	x_tm_utc.tm_mday  = record.day;
	x_tm_utc.tm_hour  = record.hour - time_offset;
	x_tm_utc.tm_min   = record.min;
	x_tm_utc.tm_sec   = record.sec;

	/*** make time_t time ***/
	time_t x_utc = mktime(&x_tm_utc);

	if ( x_utc < time_start ) continue;

	if ( record.crate >= 1 && record.crate <= NUMBER_OF_CRATES ) {
	  i = record.crate - 1;

	  /*** bad temperature sensor ***/
	  if ( record.temp3 == 209 )
	    record.temp3 = 123456;

	  gr_temp1[i]->SetPoint(gr_temp1[i]->GetN(),x_utc,record.temp1);
	  gr_temp2[i]->SetPoint(gr_temp2[i]->GetN(),x_utc,record.temp2);
	  gr_temp3[i]->SetPoint(gr_temp3[i]->GetN(),x_utc,record.temp3);

	  gr_V_3[i]->SetPoint(gr_V_3[i]->GetN(),x_utc,1.0*record.V_3/1000.0);
	  gr_V_5[i]->SetPoint(gr_V_5[i]->GetN(),x_utc,1.0*record.V_5/1000.0);

	  gr_V_12[i]->SetPoint(gr_V_12[i]->GetN(),x_utc,1.0*record.V_12/1000.0);
	  gr_V_M5[i]->SetPoint(gr_V_M5[i]->GetN(),x_utc,1.0*record.V_M5/1000.0);

	  gr_F1_RPM[i]->SetPoint(gr_F1_RPM[i]->GetN(),x_utc,record.F1_RPM);
	  gr_F2_RPM[i]->SetPoint(gr_F2_RPM[i]->GetN(),x_utc,record.F2_RPM);
	  gr_F3_RPM[i]->SetPoint(gr_F3_RPM[i]->GetN(),x_utc,record.F3_RPM);
	  gr_F4_RPM[i]->SetPoint(gr_F4_RPM[i]->GetN(),x_utc,record.F4_RPM);
	  gr_F5_RPM[i]->SetPoint(gr_F5_RPM[i]->GetN(),x_utc,record.F5_RPM);
	  gr_F6_RPM[i]->SetPoint(gr_F6_RPM[i]->GetN(),x_utc,record.F6_RPM);

	  set_min(par_min+i, &record);
	  set_max(par_max+i, &record);
	  
	  

	}

      }
      
    } 
    
    delete is;    
    
    time_i += 86400; /*** seconds in day = 24*60*60 ***/
    gmtime_r( &time_i, &time_i_tm );
    
  } while(time_i_tm.tm_year*10000 + time_i_tm.tm_mon*100 + time_i_tm.tm_mday <=
	  time_now_tm.tm_year*10000 + time_now_tm.tm_mon*100 + time_now_tm.tm_mday );
  
  
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
  
  c1->SaveAs("../png/DAWN.png");

  //==========================================================
  //           *** print min. and max. values ***
  //==========================================================

  /***
  for (i=0; i<NUMBER_OF_CRATES; i++) {
    printf(par_min[i].temp1,par_max[i].temp1);
  }  
  ***/

  ofstream *os = new ofstream("../html/DAWN.html");

  *os << "<h2> Max. values </h2>\n";

#define PRINTHEADER \
  *os << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"\
      << "<tr bgcolor=\"#A0FFA0\">\n"\
      << "    <td align=\"center\" ><b> crate         </b></td>\n"\
      << "    <td align=\"center\" ><b> temp1 (C)     </b></td>\n"\
      << "    <td align=\"center\" ><b> temp2 (C)     </b></td>\n"\
      << "    <td align=\"center\" ><b> temp3 (C)     </b></td>\n"\
      << "    <td align=\"center\" ><b> 3.3V          </b></td>\n"\
      << "    <td align=\"center\" ><b> +5V           </b></td>\n"\
      << "    <td align=\"center\" ><b> -5V           </b></td>\n"\
      << "    <td align=\"center\" ><b> 12V           </b></td>\n"\
      << "    <td align=\"center\" ><b> F1 (RPM)      </b></td>\n"\
      << "    <td align=\"center\" ><b> F2 (RPM)      </b></td>\n"\
      << "    <td align=\"center\" ><b> F3 (RPM)      </b></td>\n"\
      << "    <td align=\"center\" ><b> F4 (RPM)      </b></td>\n"\
      << "    <td align=\"center\" ><b> F5 (RPM)      </b></td>\n"\
      << "    <td align=\"center\" ><b> F6 (RPM)      </b></td>\n"\
      << "</tr>\n";

  PRINTHEADER;

#undef PRINTPAR
#define PRINTPAR(a,fact) \
     *os << "  <td align=\"center\"><b> ";  \
     if ( fabs( par_max[i].a ) == 123456 ) \
       *os <<  "-" ;\
     else \
         *os << fact*par_max[i].a; \
     *os << " </b></td>\n"; 
 

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    *os << "<tr>\n";
    PRINTPAR(crate,1);
    PRINTPAR(temp1,1);
    PRINTPAR(temp2,1);
    PRINTPAR(temp3,1);
    PRINTPAR(V_3, 0.001);
    PRINTPAR(V_5, 0.001);
    PRINTPAR(V_12,0.001);
    PRINTPAR(V_M5,0.001);
    PRINTPAR(F1_RPM,1);
    PRINTPAR(F2_RPM,1);
    PRINTPAR(F3_RPM,1);
    PRINTPAR(F4_RPM,1);
    PRINTPAR(F5_RPM,1);
    PRINTPAR(F6_RPM,1);
    *os << "</td>\n";
  }
  *os << "</table><br>\n";


#define PRINTPAR(a,fact) \
     *os << "  <td align=\"center\"><b> ";  \
     if ( fabs( par_min[i].a ) == 123456 ) \
       *os <<  "-" ;\
     else \
         *os << fact*par_min[i].a; \
     *os << " </b></td>\n"; 

  *os << "<h2> Min. values </h2>\n";

  PRINTHEADER;

  for (i=0; i<NUMBER_OF_CRATES; i++) {
    *os << "<tr>\n";
    PRINTPAR(crate,1);
    PRINTPAR(temp1,1);
    PRINTPAR(temp2,1);
    PRINTPAR(temp3,1);
    PRINTPAR(V_3, 0.001);
    PRINTPAR(V_5, 0.001);
    PRINTPAR(V_12,0.001);
    PRINTPAR(V_M5,0.001);
    PRINTPAR(F1_RPM,1);
    PRINTPAR(F2_RPM,1);
    PRINTPAR(F3_RPM,1);
    PRINTPAR(F4_RPM,1);
    PRINTPAR(F5_RPM,1);
    PRINTPAR(F6_RPM,1);
    *os << "</td>\n";
  }
  *os << "</table><br>\n";


  delete os;

}
