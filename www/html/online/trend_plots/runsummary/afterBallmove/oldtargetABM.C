//http://root.cern.ch/cgi-bin/print_hit_bold.pl/root/roottalk/roottalk01/2726.html?GetFieldnull#first_hit

// ==================================================================
//                    *** Target *** 
// ==================================================================
/* This program extracts information from the MySQL online Runlog 
   database and creates a histogram using this information. The 
   information it extracts is the sum total amount of data that Mulan 
   Summer 2007 has taken on each target */

#include<fstream>
#include<iostream>
#include<string>
#include "TBox.h"
#include "THStack.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TColor.h"
#include "../../../common/mysql.h"
#include "../../../common/init.C"
using namespace std;

void targetABM()
{

  TSQLResult *res;
  TSQLRow    *row;

  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }








  /*** create the histogram ******************************************/
  c1 = new TCanvas("c1", "Target");
  c1->SetGrid();
  c1->SetFillColor(20);
  THStack * TargetSummary= new THStack("TargetSummary", "Data Taken w.r.t Target");
  TH1D * hq= new TH1D("hq", "Quartz", 5, 0, 5);
    hq->SetFillColor(42);
  TH1D * hag= new TH1D("hag", "Ag", 5, 0, 5);
    hag->SetFillColor(50);
  TH1D * hak= new TH1D("hak", "AK3", 5, 0, 5);
    hak->SetFillColor(30);
  //  TH1D TargetSummary("Target Summary", "Target Summary", 3, 0, 3); 
  //TargetSummary.Draw();


  


  /*** total data size collected with quartz target ******************/
  res = db->Query("select sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) " 
                  "from Runlog as r "
                  "left join RunInfo using (run) "
                  "left join RunScalers using (run) "
                  "where target = 'quartz' "
                  "and tau between 1900 and 2400 "
                  "and quality='Y' "
                  "and time>10 and starttime >0 and stoptime >0 and r.run>64204 "
                  "and N is not null "
                  "and nevents_analyzed>0 "
                  "and nsegments is not null "
                 );
  cout << res << endl;

  row = res->Next();

    Double_t quartz_size = atof(row->GetField(0));
    
    // *os << "<h3>Quartz</h3>\n"
    //  << "total size written: "    << quartz_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
    //  << endl;

    
    

    cout << "Quartz" << endl;
    cout << "total muons collected: " << quartz_size << endl;
    cout << "-----------------------------------------------" << endl;
    
  // fill the quartz bin of the histogram with data
  hq->SetBinContent(2, quartz_size);
  //TargetSummary.Draw();



  
  

  /*** total data size collected with Ag target **********************/
   TString query("select sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) " 
                "from Runlog as r "
                "left join RunInfo using (run) "
                "left join RunScalers using (run) "
                "where target = 'Ag' "
                "and tau between 1900 and 2400 "
                "and quality='Y' "
                "and time>10 and starttime >0 and stoptime >0 and r.run>64204 "
                "and N is not null "
                "and nevents_analyzed>0 "
                "and nsegments is not null "
                );
  /* TString query("select * from Runlog "
     "where run = 0 ");*/


 

  res = db->Query(query);
  cout << res << endl;
  if (!res){
    Warning("Foo", "Query \"%s\" returned nothing", query.Data());
    delete res;
    return;
  }
  if (res->GetRowCount() <=0) {
    Warning("Foo", "Query \"%s\" returned nothing", query.Data());
    delete res;
    return;
  }
  


  /* if (*res==SQL_NULL_DATA){
    Warning("Foo", "Query \"%s\" returned nothing", query.Data());
    delete res;
    return;
    }*/



  row = res->Next();

  
//  Double_t Ag_size = strtol(row->GetField(0), NULL, 0);

  
    //*os << "<h3>Ag</h3>\n"
    // << "total size written: "    << Ag_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
    //<< endl;

    //Convert to Tb
 
     

    cout << "Ag" << endl;
//    cout << "total muons collected: " << Ag_size << endl;
    cout << "-----------------------------------------------" << endl;

  // fill the Ag bin of the histogram with data  
//  hag->SetBinContent(3, Ag_size);
  //TargetSummary.Draw()






  /*** total data size collected with AK3 target **********************/
  res = db->Query("select sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) " 
                  "from Runlog as r "
                  "left join RunInfo using (run) "
                  "left join RunScalers using (run) "
                  "where target = 'AK3' "
                  "and tau between 1900 and 2400 "
                  "and quality='Y' "
                  "and time>10 and starttime >0 and stoptime >0 and r.run>64204 "
                  "and N is not null "
                  "and nevents_analyzed>0 "
                  "and nsegments is not null "
                  );

  row = res->Next();

    Double_t AK3_size = atof(row->GetField(0));

    // *os << "<h3>AK3</h3>\n"
    //<< "total size written: "    << AK3_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
    //<< endl;

    //Convert to Tb
    

    cout << "AK3" << endl;
    cout << "total muons collected: " << AK3_size << endl;
    cout << "-----------------------------------------------" << endl;

  // fill the AK3 bin of the histogram with data
  hak->SetBinContent(4, AK3_size);
  //TargetSummary.Draw()

  delete row;
  delete res;


  db -> Close();


  /*** Draw the Histogram!*******************************************/ 
  TargetSummary->Add(hq);
  TargetSummary->Add(hag);
  TargetSummary->Add(hak);
  TargetSummary->Draw("nostack");
    TargetSummary->GetXaxis()->SetTitle("Target Type");
    TargetSummary->GetYaxis()->SetTitle("Tb");
    TargetSummary->GetXaxis()->SetBinLabel(2, "Quartz");
    TargetSummary->GetXaxis()->SetBinLabel(3, "Ag");
    TargetSummary->GetXaxis()->SetBinLabel(4, "AK3");
    
    hq->GetXaxis()->SetTitle("Target Type");
    hq->GetYaxis()->SetTitle("Tb");
    hq->GetXaxis()->SetBinLabel(2, "Quartz");
    hq->GetXaxis()->SetBinLabel(3, "Ag");
    hq->GetXaxis()->SetBinLabel(4, "AK3");

    hag->GetXaxis()->SetTitle("Target Type");
    hag->GetYaxis()->SetTitle("Tb");
    hag->GetXaxis()->SetBinLabel(2, "Quartz");
    hag->GetXaxis()->SetBinLabel(3, "Ag");
    hag->GetXaxis()->SetBinLabel(4, "AK3");

    hak->GetXaxis()->SetTitle("Target Type");
    hak->GetYaxis()->SetTitle("Tb");
    hak->GetXaxis()->SetBinLabel(2, "Quartz");
    hak->GetXaxis()->SetBinLabel(3, "Ag");
    hak->GetXaxis()->SetBinLabel(4, "AK3");


}
