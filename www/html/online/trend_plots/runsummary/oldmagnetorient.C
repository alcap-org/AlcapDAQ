#include<fstream>
#include<iostream>
#include<string>
#include "../../common/mysql.h"
#include "../../common/init.C"
using namespace std;

void magnetorient()
{

 
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

  // *** open HTML file for output ***
  // ofstream *os = new ofstream ("../html/runsummary.html");




  // ==================================================================
  //                    *** Magnet Orientation *** 
  // ==================================================================




  /*** create the histogram ******************************************/
  TH1D * MagnetOrient = new TH1D("MagnetOrientation", "Magnet Orientation", 17, 0, 17);
    MagnetOrient->GetXaxis()->SetTitle("Magnet Orientation");
    MagnetOrient->GetYaxis()->SetTitle("Tb");
    MagnetOrient->GetXaxis()->SetBinLabel(1, " ");  
    MagnetOrient->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    MagnetOrient->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    MagnetOrient->GetXaxis()->SetBinLabel(4, " ");      //AK D
    MagnetOrient->GetXaxis()->SetBinLabel(5, " ");
    MagnetOrient->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    MagnetOrient->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    MagnetOrient->GetXaxis()->SetBinLabel(8, " ");      //AK L
    MagnetOrient->GetXaxis()->SetBinLabel(9, " ");
    MagnetOrient->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    MagnetOrient->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    MagnetOrient->GetXaxis()->SetBinLabel(12, " ");     //AK R
    MagnetOrient->GetXaxis()->SetBinLabel(13, " ");
    MagnetOrient->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    MagnetOrient->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    MagnetOrient->GetXaxis()->SetBinLabel(16, " ");     //AK U
    MagnetOrient->GetXaxis()->SetBinLabel(17, " ");

  


  


  /*** extract data size w.r.t magnet orientation collected with each target ***/
  TSQLResult *res = db->Query("select target, "
                              "target_magnet, "
                              "sum(size) "
                              "from Runlog "
                              "where "
                              "quality = 'y'"
                              "and (target = 'quartz' "
                              "or target = 'ag' "
                              "or target = 'ak3') "
                              "group by target, "
                              "target_magnet"
                              );

  const int nrows = res->GetRowCount();
  //int nfields = res->GetFieldCount();

  struct mquery {
    string   target;
    string   magnet;
    Double_t msize;
  } ; // mdata [nrows];

  mquery * mdata [nrows];
    
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    mdata[i] = new mquery[nrows];
    mdata[i]->target = row->GetField(0);
    mdata[i]->magnet = row->GetField(1);
    mdata[i]->msize  = atof(row->GetField(2))/(1024.*1024.*1024.*1024.);
    
    //Convert to Tb
    //mdata[i].msize = mdata[i].msize/(1024.*1024.*1024.*1024.);

    cout << "i = " << i <<  endl;
    cout << "target is: " << mdata[i]->target << endl;
    cout << "magnet is oriented: " << mdata[i]->magnet << endl;
    cout << "total size written: " << mdata[i]->msize << " Tb" << endl;
    cout << "-----------------------------------------------" << endl;
  
    delete row; 
    
  }

  //delete row;
   
  /*** fill the histogram with data **********************************/

  string tbin[3] = {"uartz", "g", "3"};
  string mbin[4] = {"own", "eft","ight", "p"};
  size_t tfound;
  size_t mfound;

  for ( int i=0; i<nrows; i ++) {
   
    for (int j=0; j<3; j++) {
  
      for (int k=0; k<4; k++) { 
      
        tfound=mdata[i]->target.find(tbin[j]);
        mfound=mdata[i]->magnet.find(mbin[k]);

        if (tfound!=string::npos &&  mfound!=string::npos) {
       
          if (k==0)
            MagnetOrient->SetBinContent(j+2, mdata[i]->msize);
	  else if (k==1)
            MagnetOrient->SetBinContent(j+6, mdata[i]->msize);
          else if (k==2)
            MagnetOrient->SetBinContent(j+10, mdata[i]->msize);  
          else if (k==3)
            MagnetOrient->SetBinContent(j+14, mdata[i]->msize);
          else
            cout << "error assigning data to bin\n";
        } 
      }
    }
  }

  
  delete res;
  delete mdata;
  


  
  /*** Draw the Histogram!*******************************************/ 
  MagnetOrient->Draw();
    




}
