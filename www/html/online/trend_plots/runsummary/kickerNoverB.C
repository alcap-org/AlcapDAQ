#include <math.h>
#include<fstream>
#include<iostream>
#include<string>
#include "TBox.h"
#include "THStack.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TColor.h"
#include "../../common/mysql.h"
#include "../../common/init.C"
using namespace std;

void kickerNoverB()
{
  gROOT->Reset();
  TCanvas*c1 = new TCanvas("c1", "Kicker B/N Scan", 200,10,700,500);
  
  const Int_t j= 35;
  Int_t r[j];
  Double_t x[j], y[j], z[j], n[j], b[j], k[j], en[j], eb[j], ey[j], ex[j];
  //x data: kicker voltage difference
  x[0]=25.00;
  x[1]=25.00;
  x[2]=25.00;
  x[3]=24.75;
  x[4]=24.75;
  x[5]=24.50;
  x[6]=24.50;
  x[7]=24.25;
  x[8]=24.25;
  x[9]=24.00;
  x[10]=24.00;
  x[11]=23.75;
  x[12]=23.75;
  x[13]=23.50;
  x[14]=23.50;
  x[15]=23.25;
  x[16]=23.25;
  x[17]=23.00;
  x[18]=23.00;
  x[19]=22.00;
  x[20]=22.00;
  x[21]=20.00;
  x[22]=20.00;
  x[23]=20.00;
  x[24]=20.00;
  x[25]=20.00;
  x[26]=20.00;
  x[27]=20.00;
  x[28]=20.00;
  x[29]=18.00;
  x[30]=18.00;
  x[31]=16.00;
  x[32]=16.00;
  x[33]=14.00;
  x[34]=14.00;


  //1/y data: N/B
  z[0]=668;
  z[1]=652;
  z[2]=643;
  z[3]=655;
  z[4]=647;
  z[5]=647;
  z[6]=645;
  z[7]=645;
  z[8]=654;
  z[9]=643;
  z[10]=636;
  z[11]=631;
  z[12]=629;
  z[13]=625;
  z[14]=624;
  z[15]=628;
  z[16]=612;
  z[17]=607;
  z[18]=616;
  z[19]=549;
  z[20]=544;
  z[21]=386;
  z[22]=398;
  z[23]=392;
  z[24]=390;
  z[25]=391;
  z[26]=386;
  z[27]=408;
  z[28]=396;
  z[29]=225;
  z[30]=225;
  z[31]=97;
  z[32]=100;
  z[33]=38;
  z[34]=39;

  n[0]=3.786E05;
  n[1]=4.898E05;
  n[2]=5.203E05;
  n[3]=5.019E05;
  n[4]=4.937E05; 
  n[5]=4.904E05;
  n[6]=4.94E05;
  n[7]=4.96E05;
  n[8]=4.941E05;
  n[9]=5.001E05;
  n[10]=5.026E05;
  n[11]=4.994E05;
  n[12]=4.929E05;
  n[13]=4.996E05;
  n[14]=5.021E05;
  n[15]=3.882E05;
  n[16]=4.988E05;
  n[17]=4.956E05;
  n[18]=4.036E05;
  n[19]=5.033E05;
  n[20]=5.082E05;
  n[21]=5.946E05;
  n[22]=5.135E05;
  n[23]=5.153E05;
  n[24]=5.125E05;
  n[25]=4.987E05;
  n[26]=4.984E05;
  n[27]=3.827E05;
  n[28]=4.995E05;
  n[29]=4.755E05;
  n[30]=4.756E05;
  n[31]=4.325E05;
  n[32]=4.344E05;
  n[33]=3.414E05;
  n[34]=3.483E05;

  b[0]=567;
  b[1]=751.5;
  b[2]=808.7;
  b[3]=766.4;
  b[4]=762.7;
  b[5]=757.4;
  b[6]=766.1;
  b[7]=768.5;
  b[8]=755.1;
  b[9]=778.1;
  b[10]=789.7;
  b[11]=792;
  b[12]=783.8;
  b[13]=799;
  b[14]=805;
  b[15]=617.8;
  b[16]=814.7;
  b[17]=816.1;
  b[18]=655.4;
  b[19]=917.3;
  b[20]=934.7;
  b[21]=1539;
  b[22]=1290;
  b[23]=1313;
  b[24]=1315;
  b[25]=1277;
  b[26]=1293;
  b[27]=937.3;
  b[28]=1262;
  b[29]=2109;
  b[30]=2114;
  b[31]=4441;
  b[32]=4333;
  b[33]=8897;
  b[34]=8873;


  //error in N
  en[0]=170;
  en[1]=193;
  en[2]=200;
  en[3]=196;
  en[4]=194;
  en[5]=194;
  en[6]=194;
  en[7]=195;
  en[8]=194;
  en[9]=196;
  en[10]=196;
  en[11]=196;
  en[12]=194;
  en[13]=196;
  en[14]=196;
  en[15]=172;
  en[16]=195;
  en[17]=195;
  en[18]=176;
  en[19]=197;
  en[20]=198;
  en[21]=216;
  en[22]=200;
  en[23]=201;
  en[24]=200;
  en[25]=197;
  en[26]=197;
  en[27]=173;
  en[28]=198;
  en[29]=196;
  en[30]=196;
  en[31]=192;
  en[32]=193;
  en[33]=180;
  en[34]=181;


  //error in B
  eb[0]=2.1;
  eb[1]=2.4;
  eb[2]=2.5;
  eb[3]=2.4;
  eb[4]=2.4;
  eb[5]=2.4;
  eb[6]=2.4;
  eb[7]=2.4;
  eb[8]=2.4;
  eb[9]=2.5;
  eb[10]=2.5;
  eb[11]=2.5;
  eb[12]=2.5;
  eb[13]=2.5;
  eb[14]=2.5;
  eb[15]=2.2;
  eb[16]=2.5;
  eb[17]=2.5;
  eb[18]=2.2;
  eb[19]=2.6;
  eb[20]=2.6;
  eb[21]=3.3;
  eb[22]=3.0;
  eb[23]=3.0;
  eb[24]=3.0;
  eb[25]=3.0;
  eb[26]=3.0;
  eb[27]=2.6;
  eb[28]=2.9;
  eb[29]=3.6;
  eb[30]=3.6;
  eb[31]=4.9;
  eb[32]=4.8;
  eb[33]=6.5;
  eb[34]=6.5;

  r[0]= 67280;
  r[1]= 67281; 
  r[2]= 67282; 
  r[3]= 67316; 
  r[4]= 67317; 
  r[5]= 67283; 
  r[6]= 67284; 
  r[7]= 67314; 
  r[8]= 67315; 
  r[9]= 67285; 
  r[10]= 67286; 
  r[11]= 67312; 
  r[12]= 67313; 
  r[13]= 67287; 
  r[14]= 67288; 
  r[15]= 67310; 
  r[16]= 67311; 
  r[17]= 67289; 
  r[18]= 67290; 
  r[19]= 67308; 
  r[20]= 67309; 
  r[21]= 67294; 
  r[22]= 67295; 
  r[23]= 67296; 
  r[24]= 67297; 
  r[25]= 67298; 
  r[26]= 67299; 
  r[27]= 67300; 
  r[28]= 67301; 
  r[29]= 67302; 
  r[30]= 67303; 
  r[31]= 67306;
  r[32]= 67307; 
  r[33]= 67304; 
  r[34]= 67305; 

  k[0]= 25.00;
  k[1]= 25.00; 
  k[2]= 25.00; 
  k[3]= 24.75; 
  k[4]= 24.75; 
  k[5]= 24.50; 
  k[6]= 24.50; 
  k[7]= 24.25; 
  k[8]= 24.25; 
  k[9]= 24.00; 
  k[10]= 24.00; 
  k[11]= 23.75; 
  k[12]= 23.75; 
  k[13]= 23.50; 
  k[14]= 23.50; 
  k[15]= 23.25; 
  k[16]= 23.25; 
  k[17]= 23.00; 
  k[18]= 23.00; 
  k[19]= 22.00; 
  k[20]= 22.00; 
  k[21]= 20.00; 
  k[22]= 20.00; 
  k[23]= 20.00; 
  k[24]= 20.00; 
  k[25]= 20.00; 
  k[26]= 20.00; 
  k[27]= 20.00; 
  k[28]= 20.00; 
  k[29]= 18.00; 
  k[30]= 18.00; 
  k[31]= 16.00; 
  k[32]= 16.00; 
  k[33]= 14.00; 
  k[34]= 14.00; 
  
  //y data: B/N
  for (Int_t i=0;i<j;i++)
    y[i] = 1/z[i];

  //error in B/N
  for (Int_t i=0;i<j;i++)
    ey[i] = pow((pow(eb[i],2)/pow(n[i],2))+(pow(b[i],2)/pow(n[i],4))*pow(en[i],2),0.5);

  //don't care to plot errors in x, kicker voltage
  for (Int_t i=0;i<j;i++)
    ex[i] = 0.;

 cout << " Run # | HV    | N   | error | B    | error | N/B   | B/N  | error " << endl;
 for (Int_t i=0;i<j;i++)
   cout << r[i] << " | " << k[i] << " | " << n[i] << " | " << en[i] << " | " << b[i] << " | " << eb[i] << " | " <<n[i]/b[i] << " | " << y[i] << " | " << ey[i] << endl;

  TGraphErrors *gr = new TGraphErrors(j,x,y,ex,ey);
  gr->SetTitle("Kicker B/N scan");
  gr->GetXaxis()->SetTitle("Kicker HV");
  gr->GetYaxis()->SetTitle("B/N");
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(20);
  gr->Draw("AP");

  TF1 * f1 = new TF1("f1", "[0]*x + [1]", 23, 25);
  f1->SetLineColor(2);
  f1->SetParameters(0,0.02);
  gr->Fit("f1","R");

  gStyle->SetOptFit(1);




}



































