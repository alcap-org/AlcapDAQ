/***NOTE: after running this program in root, click on the canvas to get the correct x axis labels. */
//helpful website: http://root.cern.ch/cgi-bin/print_hit_bold.pl/root/roottalk/roottalk01/2726.html?GetFieldnull#first_hit

// ==================================================================
//                    *** Target *** 
// ==================================================================
/* This program extracts information from the MySQL online Runlog 
   database and creates a histogram using this information. The 
   information it extracts is the sum total amount of data that Mulan 
   Summer 2007 has taken on each target with the hinge (flapper) in 
   varying positions.  */

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


//create class hquery which will collect and return the values 
//extracted from the MySQL database
class tquery {
  private: 
    string   target;
    Double_t tsize;
  public: 
    string   GetTarget(){return target;}
    Double_t GetSize(){return tsize;}
    void     SetTarget(string tset) {target = tset;}
    void     SetSize(Double_t sset) {tsize = sset;}

};  

void target()
{
  /*** connect to MySQL online Runlog database ***********************/
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

 


  /*** create canvas, stack of histograms and histograms *************/
  //create canvas
  c1 = new TCanvas("c1", "Target");
  //create gridlines
  c1->SetGrid();
  //color the canvas
  c1->SetFillColor(20);

  //create a pointer to a stack of histograms
  /* An individual histogram can only be one color. A stack of 
     histograms was used so that the final result would look like one 
     histogram which was color coded. */
  THStack * TargetSummary= new THStack("TargetSummary","Data Taken w.r.t Target");
  /*create a histogram for each target that the data was taken on: 
    Quartz, Silver, and AK3.
    Set a different fill color for each histogram*/
  TH1D * hq = new TH1D("hq", "Quartz", 5, 0, 5);
    hq->SetFillColor(42);
  TH1D * hag= new TH1D("hag", "Ag", 5, 0, 5);
    hag->SetFillColor(50);
  TH1D * hak= new TH1D("hak", "AK3", 5, 0, 5);
    hak->SetFillColor(30);

  //create an array of the histogram pointers. 
  //This will make filling the histograms with data easier
  TH1D * htarget[] = {hq, hag, hak};
  
  


  


  /*** extract information from MySQL online Runlog database *********/
  // extract data size w.r.t hinge orientation collected with each target 

  /*Query MySQL.  MySQL will return 3 columns: target, emc_magnet, sum(size).
   Mid-Summer it was decided that the field emc_magnet would actually be 
   used for the position of the hinge/flapper.  Before this decision the 
   field contained entries that all read "none".  During this time when 
   the field reads "none" the flapper was positioned "left". MySQL calculates
   the total amount of data that was taken on each target with different 
   orientations of the flapper and lists this value in the column sum(size).*/

   TString query("select target, "
                "sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) " 
                "from Runlog as r "
                "left join RunInfo using (run) "
                "left join RunScalers using (run) "
                "where "
                "tau between 1900 and 2400 "
                "and quality='Y' "
                "and time>10 and starttime >0 and stoptime >0 and r.run>61377 "
                "and N is not null "
                "and nevents_analyzed>0 "
                "and nsegments is not null "
                "group by target "
                ); 
 
  
  TSQLResult *res = db->Query(query); 
  /*     TSQLResult *res = db->Query("select target, "
                "sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) " 
                "from Runlog as r "
                "left join RunInfo using (run) "
                "left join RunScalers using (run) "
                "where "
                "tau between 1900 and 2400 "
                "and quality='Y' "
                "and time>10 and starttime >0 and stoptime >0 and r.run>64204 "
                "and N is not null "
                "and nevents_analyzed>0 "
                "and nsegments is not null "
                "group by target "
                ); */

  /* cout << res << endl;
   if (!res){
     Warning("Foo", "Query \"%s\" returned nothing", query.Data());
     delete res;
     return;
   }
  if (res->GetRowCount() <=0) {
    Warning("Foo", "Query \"%s\" returned nothing", query.Data());
    delete res;
    return;
    }*/

  //how many rows did the query return?
  const int nrows = res->GetRowCount();

  //create an array of pointers called tdata that is of class tquery and is 
  //big enough to point to enough space for all of the data from the query
    tquery * tdata [nrows];
    //tquery * tdata = new tquery[nrows];
  //store the data to the locations just created using functions defined 
  //in the tquery class and other fancy root-SQL commands
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    tdata[i] = new tquery;

    tdata[i]->SetTarget(row->GetField(0));
    tdata[i]->SetSize(strtol(row->GetField(1), NULL, 0));
    //      tdata[i].SetSize(atof(row->GetField(1)));

    //print the data to the screen using fuctions defined in the 
    //tquery class
    cout << "i = " << i <<  endl;
    cout << "target is: " << tdata[i]->GetTarget() << endl;
    cout << "total muons collected: " << tdata[i]->GetSize() << endl;
    cout << "-----------------------------------------------" << endl;
  
    delete row; 
    
  }


   
  /*** fill the histogram with data **********************************/


  /*Use the string find function to figure out which histogram and 
    bin each data piece should be assigned to */
  /*tbin is the string used to figure out what target the data was 
    taken on. The first letters of the targets are left out
    because I wanted to avoid any problems with case sensitivity. */
  string tbin[3] = {"uartz", "g", "3"};
  size_t tfound;

  for ( int i=0; i<nrows; i ++) {
   
    for (int j=0; j<3; j++) {
  
     
           tfound=tdata[i]->GetTarget().find(tbin[j]);
	   //tfound will equal npos if it does not find the string it
           //was searching for
                
	//if the data was found then it is assigned to the appropriate
        //histogram and bin
      if (tfound!=string::npos) {
        cout << "data found" << endl;
            
        if (j==0)
          htarget[j]->SetBinContent(2, tdata[i]->GetSize());
        else if (j==1) 
          htarget[j]->SetBinContent(3, tdata[i]->GetSize());
        else if (j==2)
          htarget[j]->SetBinContent(4, tdata[i]->GetSize());  
        else
          cout << "error assigning data to bin\n";

      } 
    }
  }
 

  
  delete res;

  db -> Close();

  


  
  /*** Draw the Histogram!*******************************************/ 


  for (i=0; i<3; i++)
  TargetSummary->Add(htarget[i]);

  TargetSummary->Draw("nostack");
       
    TargetSummary->GetXaxis()->SetTitle("Target");
    TargetSummary->GetYaxis()->SetTitle("muons");
    TargetSummary->GetXaxis()->SetBinLabel(2, "Quartz");
    TargetSummary->GetXaxis()->SetBinLabel(3, "Ag");
    TargetSummary->GetXaxis()->SetBinLabel(4, "AK3");
    
    hq->GetXaxis()->SetTitle("Target");
    hq->GetYaxis()->SetTitle("muons");
    hq->GetXaxis()->SetBinLabel(2, "Quartz");
    hq->GetXaxis()->SetBinLabel(3, "Ag");
    hq->GetXaxis()->SetBinLabel(4, "AK3");

    hag->GetXaxis()->SetTitle("Target");
    hag->GetYaxis()->SetTitle("muons");
    hag->GetXaxis()->SetBinLabel(2, "Quartz");
    hag->GetXaxis()->SetBinLabel(3, "Ag");
    hag->GetXaxis()->SetBinLabel(4, "AK3");

    hak->GetXaxis()->SetTitle("Target");
    hak->GetYaxis()->SetTitle("muons");
    hak->GetXaxis()->SetBinLabel(2, "Quartz");
    hak->GetXaxis()->SetBinLabel(3, "Ag");
    hak->GetXaxis()->SetBinLabel(4, "AK3");





    delete tdata;



}
