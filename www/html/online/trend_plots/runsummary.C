#include<fstream>
#include "../common/mysql.h"
#include "../common/init.C"
using namespace std;

void runsummary()
{

  TSQLResult *res;
  TSQLRow    *row;

  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

  // *** open HTML file for output ***
  // ofstream *os = new ofstream ("../html/runsummary.html");




  // ==================================================================
  //                    *** Target Type *** 
  // ==================================================================




  /*** create the histogram ******************************************/
  TH1D * TargetSummary = new TH1D("TargetSummary", "Target Summary", 3, 0, 3);
    TargetSummary->GetXaxis()->SetTitle("Target Type");
    TargetSummary->GetYaxis()->SetTitle("Tb");
    TargetSummary->GetXaxis()->SetBinLabel(1, "Quartz");
    TargetSummary->GetXaxis()->SetBinLabel(2, "Ag");
    TargetSummary->GetXaxis()->SetBinLabel(3, "AK3");
    
  //  TH1D TargetSummary("Target Summary", "Target Summary", 3, 0, 3); 
  //TargetSummary.Draw();


  


  /*** total data size collected with quartz target ******************/
  res = db->Query("select sum(size) "
                  "from Runlog "
                  "where "
                  "target = 'quartz' "
                  "and "
                  "quality = 'y'"
                 );

  row = res->Next();

    Double_t quartz_size = atof(row->GetField(0));
    
    // *os << "<h3>Quartz</h3>\n"
    //  << "total size written: "    << quartz_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
    //  << endl;

    //Convert to Tb
    quartz_size = quartz_size/(1024.*1024.*1024.*1024.);

    cout << "Quartz" << endl;
    cout << "total size written: " << quartz_size << " Tb " << endl;
    cout << "-----------------------------------------------" << endl;
    
  // fill the quartz bin of the histogram with data
  TargetSummary->SetBinContent(1, quartz_size);
  //TargetSummary.Draw();

  delete row;
  delete res;

  
  

  /*** total data size collected with Ag target **********************/
  res = db->Query("select sum(size) "
                  "from Runlog "
                  "where target = 'Ag' "
                  "and "
                  "quality = 'y'"
                  );

  row = res->Next();

    Double_t Ag_size = atof(row->GetField(0));

    //*os << "<h3>Ag</h3>\n"
    // << "total size written: "    << Ag_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
    //<< endl;

    //Convert to Tb
    Ag_size = Ag_size/(1024.*1024.*1024.*1024.);

    cout << "Ag" << endl;
    cout << "total size written: " << Ag_size/(1024.*1024.*1024.*1024.) << " Tb " << endl;
    cout << "-----------------------------------------------" << endl;

  // fill the Ag bin of the histogram with data  
  TargetSummary->SetBinContent(2, Ag_size);
  //TargetSummary.Draw()

  delete row;
  delete res;




  /*** total data size collected with AK3 target **********************/
  res = db->Query("select sum(size) "
                  "from Runlog "
                  "where target = 'AK3' "
                  "and "
                  "quality = 'y'"
                  );

  row = res->Next();

    Double_t AK3_size = atof(row->GetField(0));

    // *os << "<h3>AK3</h3>\n"
    //<< "total size written: "    << AK3_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
    //<< endl;

    //Convert to Tb
    AK3_size = AK3_size/(1024.*1024.*1024.*1024.);

    cout << "AK3" << endl;
    cout << "total size written: " << AK3_size/(1024.*1024.*1024.*1024.) << "Tb " << endl;
    cout << "-----------------------------------------------" << endl;

  // fill the AK3 bin of the histogram with data
  TargetSummary->SetBinContent(3, AK3_size);
  //TargetSummary.Draw()

  delete row;
  delete res;





  /*** Draw the Histogram!*******************************************/ 
  TargetSummary->Draw();
    




}
