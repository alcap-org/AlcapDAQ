#include<fstream>
#include<iostream>
#include<string>
#include "TBox.h"
#include "THStack.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TColor.h"
#include "../../common/mysql.h"
#include "../../common/init.C"
using namespace std;



class hquery {
  private: 
    string   target;
    string   hinge;
    Double_t hsize;
  public: 
    string   GetTarget(){return target;}
    string   GetHinge(){return hinge;}
    Double_t GetSize(){return hsize;}
    void     SetTarget(string tset) {target = tset;}
    void     SetHinge(string hset) {hinge = hset;}
    void     SetSize(Double_t sset) {hsize = sset;}

};  






void hinge()
{
   
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

  // *** open HTML file for output ***
  // ofstream *os = new ofstream ("../html/runsummary.html");




  // ==================================================================
  //                    *** Hinge Orientation *** 
  // ==================================================================


  c1 = new TCanvas("c1", "Hinge Orientation");
  c1->SetGrid();
  c1->SetFillColor(20);

  /*** create the histogram ******************************************/
  THStack * HingeOrient= new THStack("HingeOrient","Data Taken w.r.t Hinge Orientation");
  TH1D * hq = new TH1D("hq", "Quartz", 17, 0, 17);
  TH1D * hag= new TH1D("hag", "Ag", 17, 0, 17);
  TH1D * hak= new TH1D("hak", "AK3", 17,0, 17);
  TH1D * htarget[] = {hq, hag, hak};
    htarget[0]->SetFillColor(42);
    htarget[1]->SetFillColor(50);
    htarget[2]->SetFillColor(30);
  
  


  


  /*** extract data size w.r.t hinge orientation collected with each target ***/
  TSQLResult *res = db->Query("select target, "
                              "emc_magnet, "
                              "sum(size) "
                              "from Runlog "
                              "where "
                              "quality = 'y'"
                              "group by target, "
                              "emc_magnet"
                              );

  const int nrows = res->GetRowCount();

  hquery * hdata [nrows];
  
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    hdata[i] = new hquery;

    hdata[i]->SetTarget(row->GetField(0));
    hdata[i]->SetHinge(row->GetField(1));
    hdata[i]->SetSize(atof(row->GetField(2))/(1024.*1024.*1024.*1024.));
    
    cout << "i = " << i <<  endl;
    cout << "target is: " << hdata[i]->GetTarget() << endl;
    cout << "hinge is oriented: " << hdata[i]->GetHinge() << endl;
    cout << "total size written: " << hdata[i]->GetSize() << " Tb" << endl;
    cout << "-----------------------------------------------" << endl;
  
    delete row; 
    
  }


   
  /*** fill the histogram with data **********************************/

  string tbin[3] = {"uartz", "g", "3"};
  string hbin[5] = {"own", "eft", "ight", "p", "one"};
  size_t tfound;
  size_t hfound;

  for ( int i=0; i<nrows; i ++) {
   
    for (int j=0; j<3; j++) {
  
      for (int k=0; k<5; k++) { 
      
           tfound=hdata[i]->GetTarget().find(tbin[j]);
	   hfound=hdata[i]->GetHinge().find(hbin[k]);

        if (tfound!=string::npos &&  hfound!=string::npos) {
          cout << "data found" << endl;
          if (k==0)
            htarget[j]->SetBinContent(j+2, hdata[i]->GetSize());
	  else if (k==1 || k==4)
            htarget[j]->SetBinContent(j+6, hdata[i]->GetSize());
          else if (k==2)
            htarget[j]->SetBinContent(j+10, hdata[i]->GetSize());  
          else if (k==3)
            htarget[j]->SetBinContent(j+14, hdata[i]->GetSize());
          else
            cout << "error assigning data to bin\n";
        } 
      }
    }
  }

  
  delete res;
  delete hdata;
  


  
  /*** Draw the Histogram!*******************************************/ 

  for (int i=0; i<3; i++) 
    HingeOrient->Add(htarget[i]);
   
  Double_t fmax;
  fmax = HingeOrient->GetMaximum(); 
    
  cout << fmax << endl;  
  cout << fmax*13./12. << endl;

  TH1D * pretty = new TH1D("pretty", "pretty", 17, 0, 17);
  pretty->SetFillColor(28);
  pretty->SetLineColor(20);
  pretty->SetFillStyle(3018);
  for (int i=1; i<18; i+= 4)
    pretty->SetBinContent(i, fmax*13./12.);
  
  HingeOrient->Add(pretty);

 
  
  



  HingeOrient->Draw("nostack");
       
    HingeOrient->GetXaxis()->SetTitle("Hinge Orientation");
    HingeOrient->GetYaxis()->SetTitle("Tb");
    HingeOrient->GetXaxis()->SetBinLabel(1, " ");  
    HingeOrient->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    HingeOrient->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    HingeOrient->GetXaxis()->SetBinLabel(4, " ");      //AK D
    HingeOrient->GetXaxis()->SetBinLabel(5, " ");
    HingeOrient->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    HingeOrient->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    HingeOrient->GetXaxis()->SetBinLabel(8, " ");      //AK L
    HingeOrient->GetXaxis()->SetBinLabel(9, " ");
    HingeOrient->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    HingeOrient->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    HingeOrient->GetXaxis()->SetBinLabel(12, " ");     //AK R
    HingeOrient->GetXaxis()->SetBinLabel(13, " ");
    HingeOrient->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    HingeOrient->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    HingeOrient->GetXaxis()->SetBinLabel(16, " ");     //AK U
    HingeOrient->GetXaxis()->SetBinLabel(17, " ");

    hq->GetXaxis()->SetTitle("Hinge Orientation");
    hq->GetYaxis()->SetTitle("Tb");
    hq->GetXaxis()->SetBinLabel(1, " ");  
    hq->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    hq->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    hq->GetXaxis()->SetBinLabel(4, " ");      //AK D
    hq->GetXaxis()->SetBinLabel(5, " ");
    hq->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    hq->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    hq->GetXaxis()->SetBinLabel(8, " ");      //AK L
    hq->GetXaxis()->SetBinLabel(9, " ");
    hq->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    hq->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    hq->GetXaxis()->SetBinLabel(12, " ");     //AK R
    hq->GetXaxis()->SetBinLabel(13, " ");
    hq->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    hq->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    hq->GetXaxis()->SetBinLabel(16, " ");     //AK U
    hq->GetXaxis()->SetBinLabel(17, " ");

    hag->GetXaxis()->SetTitle("Hinge Orientation");
    hag->GetYaxis()->SetTitle("Tb");
    hag->GetXaxis()->SetBinLabel(1, " ");  
    hag->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    hag->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    hag->GetXaxis()->SetBinLabel(4, " ");      //AK D
    hag->GetXaxis()->SetBinLabel(5, " ");
    hag->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    hag->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    hag->GetXaxis()->SetBinLabel(8, " ");      //AK L
    hag->GetXaxis()->SetBinLabel(9, " ");
    hag->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    hag->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    hag->GetXaxis()->SetBinLabel(12, " ");     //AK R
    hag->GetXaxis()->SetBinLabel(13, " ");
    hag->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    hag->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    hag->GetXaxis()->SetBinLabel(16, " ");     //AK U
    hag->GetXaxis()->SetBinLabel(17, " ");

    hak->GetXaxis()->SetTitle("Hinge Orientation");
    hak->GetYaxis()->SetTitle("Tb");
    hak->GetXaxis()->SetBinLabel(1, " ");  
    hak->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    hak->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    hak->GetXaxis()->SetBinLabel(4, " ");      //AK D
    hak->GetXaxis()->SetBinLabel(5, " ");
    hak->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    hak->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    hak->GetXaxis()->SetBinLabel(8, " ");      //AK L
    hak->GetXaxis()->SetBinLabel(9, " ");
    hak->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    hak->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    hak->GetXaxis()->SetBinLabel(12, " ");     //AK R
    hak->GetXaxis()->SetBinLabel(13, " ");
    hak->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    hak->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    hak->GetXaxis()->SetBinLabel(16, " ");     //AK U
    hak->GetXaxis()->SetBinLabel(17, " ");

    
    for (int i=0; i<3; i++)
      htarget[i]->SetAxisRange(0, fmax*13./12., "Y");
    pretty->SetAxisRange(0, fmax*13./12., "Y");
    HingeOrient->SetMaximum(fmax*13./12.);

    legend = new TLegend(0.67,0.72,0.88,0.89);
    legend->AddEntry(hq, "Quartz", "f");
    legend->AddEntry(hag, "Ag", "f");
    legend->AddEntry(hak, "AK3", "f"); 
    legend->SetHeader("Target");
    legend->Draw();



}
