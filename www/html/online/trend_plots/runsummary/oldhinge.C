#include<fstream>
#include<iostream>
#include<string>
#include "../../common/mysql.h"
#include "../../common/init.C"
using namespace std;

void hinge()
{

 
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

  // *** open HTML file for output ***
  // ofstream *os = new ofstream ("../html/runsummary.html");




  // ==================================================================
  //                    *** Hinge Orientation *** 
  // ==================================================================




  /*** create the histogram ******************************************/
  TH1D * HingeOrient = new TH1D("HingeOrientation", "Hinge Orientation", 17, 0, 17);
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
  //int nfields = res->GetFieldCount();

  struct hquery {
    string   target;
    string   hinge;
    Double_t hsize;
  } ; // hdata [nrows];

  hquery * hdata [nrows];
    
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    //hdata[i] = new hquery;
    hdata[i]->target = row->GetField(0);
    hdata[i]->hinge = row->GetField(1);
    hdata[i]->hsize  = atof(row->GetField(2))/(1024.*1024.*1024.*1024.);
    
    //Convert to Tb
    //mdata[i].msize = mdata[i].msize/(1024.*1024.*1024.*1024.);

    cout << "i = " << i <<  endl;
    cout << "target is: " << hdata[i]->target << endl;
    cout << "hinge is oriented: " << hdata[i]->hinge << endl;
    cout << "total size written: " << hdata[i]->hsize << " Tb" << endl;
    cout << "-----------------------------------------------" << endl;
  
    delete row; 
    
  }

  //delete row;
   
  /*** fill the histogram with data **********************************/

  string tbin[3] = {"uartz", "g", "3"};
  string hbin[4] = {"own", "eft", "ight", "p", "one"};
  size_t tfound;
  size_t hfound;

  for ( int i=0; i<nrows; i ++) {
   
    for (int j=0; j<3; j++) {
  
      for (int k=0; k<4; k++) { 
      
        tfound=hdata[i]->target.find(tbin[j]);
        hfound=hdata[i]->hinge.find(hbin[k]);

        if (tfound!=string::npos &&  hfound!=string::npos) {
       
          if (k==0)
            HingeOrient->SetBinContent(j+2, hdata[i]->hsize);
	  else if (k==1 || k==4)
            HingeOrient->SetBinContent(j+6, hdata[i]->hsize);
          else if (k==2)
            HingeOrient->SetBinContent(j+10, hdata[i]->hsize);  
          else if (k==3)
            HingeOrient->SetBinContent(j+14, hdata[i]->hsize);
          else
            cout << "error assigning data to bin\n";
        } 
      }
    }
  }

  
  delete res;
  delete hdata;
  


  
  /*** Draw the Histogram!*******************************************/ 
  HingeOrient->Draw();
    




}
