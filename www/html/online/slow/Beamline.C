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
#include "TFrame.h"
#include "TGaxis.h"

#define FORMAT   "%04d%02d%02d%02d%02d%02d\t%s\t%d\t%f"

#define FE_NAME            "Beamline"
#define MAX_LINE_LEN       102400
#define NUMBER_OF_MAGNETS  29

const char *names[NUMBER_OF_MAGNETS] = {
  "QTD71",
  "QTD72",
  "QTB71",
  "ASK71",
  "HSA71",
  "QSK71",
  "QSK72",
  "QSK73",
  "QSK74",
  "HSA72",
  "ASK72",
  "HSD71",
  "QSB71",
  "QSB72",
  "SEP",
  "QSB73",
  "QSB75",
  "WEN",
  "QSD02",
  "QSC01",
  "AEK01",
  "FS71-O",
  "FS71-U",
  "FS71-L",
  "FS71-R",
  "FS72-O",
  "FS72-U",
  "FS72-L",
  "FS72-R" };

struct RECORD {

  int year;
  int mon;
  int day;
  int hour;
  int min;
  int sec;

  char name[256];
  int demand;
  float measured;

};

RECORD record;

const float coeff[NUMBER_OF_MAGNETS] = {
  1631.0 / 0.0825,
  1155.0 / 0.0578,
   505.0 / 0.0273,
  7419.0 / 0.1152,
  0,
  2983.0 / 0.1455,
  2086.0 / 0.103,
   640.0 / 0.0324,
  1200.0 / 0.0595,
  0,    
  6919.0 / 0.1057,
  0,
   460.0 / 0.0227,
   970.0 / 0.0466,
   850.0 / 0.4146,
    97.9 / 0.0056,
   674.0 / 0.0337,
  1390.0 / 0.0673,
   255.0 / 0.0598,
   381.0 / 0.0923,
   255.0 / 0.062,
   175.0 / 0.175,
   400.0 / 0.3992,
   200.0 / 0.2002,
   200.0 / 0.2,
    80.0 / 0.08,
   125.0 / 0.1255,
   300.0 / 0.2996,
   300.0 / 0.3001 };

const float recommended_values[NUMBER_OF_MAGNETS] = {
  1631.0,
 -1155.0,
   505.0,
  7419.0,
       0,
  2983.0,
 -2086.0,
  -640.0,
  1200.0,
       0,    
  6919.0,
       0,
  -460.0,
   970.0,
   895.0,
    97.9,
  -674.0,
  1390.0,
   255.0,
  -381.0,
  -245.0,
   300.0,
   400.0,
   200.0,
   200.0,
   105.0,
   120.0,
   300.0,
   300.0 };



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


void Beamline(const int rollback)
{

  const int time_offset = -2;

  TGraph *gr_demand      [NUMBER_OF_MAGNETS];
  TGraph *gr_measured    [NUMBER_OF_MAGNETS];
  TGraph *gr_recommended [NUMBER_OF_MAGNETS]; /*** best beam tune settings ***/

  RECORD par_min    [NUMBER_OF_MAGNETS];
  RECORD par_max    [NUMBER_OF_MAGNETS];

  int colors[2] = {kBlue,kRed};

  TDatime mytime;


  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);


  //=========================================================
  //                  *** create canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,4000);
  c1->Draw();
  c1->cd();

  c1->Divide(2,15,0.001,0.0001,10);
  //c1->Divide(2,4,0.001,0.001,10);
  c1->Update();

  char name[256];

  float markersize = 0.1;

  int i;  
  /*** Set names ***/
  for (i=0; i<NUMBER_OF_MAGNETS; i++) {
    sprintf(name,"%s_demand",names[i]);
    gr_demand[i] = new TGraph(1024);
    gr_demand[i]->SetName(name); 
    gr_demand[i]->SetTitle(name); 
    gr_demand[i]->SetMarkerStyle(21);
    gr_demand[i]->SetMarkerSize(markersize);
    gr_demand[i]->SetMarkerColor(colors[0]);
    gr_demand[i]->SetLineColor(colors[0]);

    sprintf(name,"%s_measured",names[i]);
    gr_measured[i] = new TGraph(1024);
    gr_measured[i]->SetName(name); 
    gr_measured[i]->SetTitle(name); 
    gr_measured[i]->SetMarkerStyle(21);
    gr_measured[i]->SetMarkerSize(markersize);
    gr_measured[i]->SetMarkerColor(colors[1]);
    gr_measured[i]->SetLineColor(colors[1]);

    sprintf(name,"%s_recommended",names[i]);
    gr_recommended[i] = new TGraph(1024);
    gr_recommended[i]->SetName(name); 
    gr_recommended[i]->SetTitle(name); 
    gr_recommended[i]->SetMarkerStyle(21);
    gr_recommended[i]->SetMarkerSize(markersize);
    gr_recommended[i]->SetMarkerColor(kGreen);
    gr_recommended[i]->SetLineColor(kGreen);
    gr_recommended[i]->SetLineWidth(2);

    par_min[i].demand   = 1234567890;
    par_min[i].measured = 1234567890;
       
    par_max[i].demand   = -1234567890;
    par_max[i].measured = -1234567890;

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
	       
	       &record.name,
	       &record.demand,
	       &record.measured);

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

	int id = -1;

	for (i=0; i<NUMBER_OF_MAGNETS; i++) {
	  if ( !strcmp(record.name,names[i]) ) {
	    id = i;
	    break;
	  }
	}

	if ( id < 0 ) continue;
	
	//printf("name: [%s], id: %d\n",record.name, id);

	record.measured *= coeff[id];

	gr_demand[id]->SetPoint(gr_demand[id]->GetN(),x_utc,record.demand);
	gr_measured[id]->SetPoint(gr_measured[id]->GetN(),x_utc,record.measured);

	//printf("name: [%s], demand: %d measured: %f\n",record.name, record.demand,  record.measured*coeff[id]);

	set_min(par_min+i, &record);
	set_max(par_max+i, &record);
	  	 
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
  TVirtualPad *pad, *pad_top;
  
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
    if (fabs(p_min) != 1234567890) {\
      ymin = p_min < ymin ? p_min - fabs(p_min*0.03) : ymin;\
    }\
    ymin = recommended_values[i] < ymin ? recommended_values[i] - fabs(recommended_values[i]*0.05) : ymin;\
\
    if (fabs(p_max) != 1234567890) {\
      ymax = p_max > ymax ? p_max + fabs(p_max*0.03) : ymax;\
    } \
    ymax = recommended_values[i] > ymax ? recommended_values[i] + fabs(recommended_values[i]*0.05) : ymax;\
}

#define DRAWFRAMELEFT(title,ytitle) {\
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
  frame->GetXaxis()->SetLabelSize(0.055);\
  frame->GetYaxis()->SetTitleOffset(0.65);\
  frame->GetYaxis()->SetTitleSize(0.055);\
  frame->GetYaxis()->SetLabelSize(0.055);\
  frame->GetYaxis()->SetTitleOffset(0.65);\
}

#define DRAWFRAMERIGHT(title,ytitle) {\
  pad->cd();\
  pad_top = new TPad("pad_top","pad_top",0,0,1,1,-1,-1,-2); \
  pad_top->Draw(); \
  pad_top->cd(); \
  pad_top->SetBottomMargin(0.1); \
  frame = pad_top->DrawFrame(tmin,ymin,tmax,ymax,title); \
  frame->GetXaxis()->SetTimeDisplay(1); \
  frame->GetXaxis()->SetTimeFormat("%H:%M"); \
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
  pad_top->SetFillStyle(0); \
  pad_top->GetFrame()->SetFillStyle(0); \
}


 Double_t p_min, p_max;

 
 for (i=0; i<NUMBER_OF_MAGNETS; i++) {
 //for (i=0; i<4; i++) {

    
   pad = c1->GetPad(i+1);
   
   pad->cd();

   
   ymin =  100000;
   ymax = -100000;
   SETMINMAX(measured);
   SETMINMAX(demand);
   //SETMINMAX(recommended);

   // update: set min-max basing on the average
   Double_t y_mean = 0;
   Double_t y_mean2 = 0;
   Int_t np = 0;
   for (int k=0; k<gr_measured[i]->GetN(); k++) {

     Double_t X, Y;
     gr_measured[i]->GetPoint(k, X, Y);

     if ( X < tmin || X > tmax ) continue;

     y_mean  += Y;
     y_mean2 += Y*Y;
     np++;

   }

   for (int k=0; k<gr_demand[i]->GetN(); k++) {

     Double_t X, Y;
     gr_demand[i]->GetPoint(k, X, Y);

     if ( X < tmin || X > tmax ) continue;

     y_mean  += Y;
     y_mean2 += Y*Y;
     np++;

   }

   if ( np > 0 ) {
     y_mean  /= np;
     y_mean2 /= np;     
   }

   Double_t sigma = sqrt(fabs(y_mean*y_mean - y_mean2));
   if ( sigma == 0.0 ) sigma = 1.0;
   ymin = y_mean - 8.0*sigma;
   ymax = y_mean + 8.0*sigma;

   // repeat and cut points far away from average 

   y_mean = 0;
   y_mean2 = 0;
   np = 0;
   for (int k=0; k<gr_measured[i]->GetN(); k++) {

     Double_t X, Y;
     gr_measured[i]->GetPoint(k, X, Y);

     if ( X < tmin || X > tmax ) continue;
     if ( Y < ymin || Y > ymax ) continue;

     y_mean  += Y;
     y_mean2 += Y*Y;
     np++;

   }

   for (int k=0; k<gr_demand[i]->GetN(); k++) {

     Double_t X, Y;
     gr_demand[i]->GetPoint(k, X, Y);

     if ( X < tmin || X > tmax ) continue;
     if ( Y < ymin || Y > ymax ) continue;

     y_mean  += Y;
     y_mean2 += Y*Y;
     np++;

   }

   if ( np > 0 ) {
     y_mean  /= np;
     y_mean2 /= np;     
   }

   sigma = sqrt(fabs(y_mean*y_mean - y_mean2));
   if ( sigma == 0.0 ) sigma = 1.0;
   ymin = y_mean - 8.0*sigma;
   ymax = y_mean + 8.0*sigma;




   /*** set recommended values ***/
   gr_recommended[i]->SetPoint( 0, tmin, recommended_values[i]);
   gr_recommended[i]->SetPoint( 1, tmax, recommended_values[i]);
   gr_recommended[i]->SetLineWidth(10);
   
   
   
   DRAWFRAMELEFT(names[i],"DAC");
   gr_recommended[i]->Draw("L");
   gr_measured[i]->Draw("P");
   gr_demand[i]->Draw("P");

   //ymin = 0;
   //ymax = 0;
   //SETMINMAX(demand);
   //DRAWFRAMERIGHT(names[i],"measured");

   /***
   TGaxis *axis = new TGaxis(frame->GetXaxis()->GetXmax(),
			     frame->GetYaxis()->GetXmin(),
			     frame->GetXaxis()->GetXmax(),
			     frame->GetYaxis()->GetXmax(),
			     0,
			     1,510,
			     "+L");

   axis->SetLineColor(4);
   axis->SetLabelSize(0.04);
   axis->SetTitle("Demand");
   axis->SetTitleSize(0.05);
   axis->SetTitleOffset(1.0);
   axis->Draw();
        
   ***/

   c1->Update();


 }
  
  c1->SaveAs("../png/Beamline.png");

  //=========================================================================
  //          *** comments for the on-line page ***
  //=========================================================================

  ofstream *os = new ofstream("../html/Beamline.html");
  *os << "<b><font color=\"#0000FF\">Blue:</font> Demand, <font color=\"#FF0000\">Red:</font> Measured, <font color=\"#00FF00\">Green:</font> Recommended values from best beam tune</b><br>";
  delete os;

}
