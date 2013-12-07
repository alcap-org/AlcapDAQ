/* RunSummary up to Monday July 30
//select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 61377 and r.run < 63775 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet;

select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run < 63775 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet;

select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 63774 and r.run < 64205 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet;

select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 64204 and r.run < 65461 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet;

//select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 65460 and r.run < 65554 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet;

//select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 64204 and r.run < 65554 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet;

select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 65553 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet; */


// ==================================================================
//                    *** RunSummary *** 
// ==================================================================
/* This program extracts information from the MySQL online Runlog 
   database and creates a histogram using this information. The 
   information it extracts is the total amount of muons that Mulan 
   Summer 2007 has collected on each target with magnet orientation in 
   varying positions during specific intervals of the Run.  */

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


//create class mquery which will collect and return the values 
//extracted from the MySQL database
class mquery {
  private: 
    string   target;
    string   magnet;
    Double_t msize;
  public: 
    string   GetTarget(){return target;}
    string   GetMagnet(){return magnet;}
    Double_t GetSize(){return msize;}
    void     SetTarget(string tset) {target = tset;}
    void     SetMagnet(string mset) {magnet = mset;}
    void     SetSize(Double_t sset) {msize = sset;}

};  

Double_t Max(const Double_t *Numbers, const int Count)
{
  Double_t Maximum = Numbers[0];
  for(int i = 0; i < Count; i++)
    if( Maximum < Numbers[i] )
      Maximum = Numbers[i];

  return Maximum;
}





void runsummary()
{
  /*** connect to MySQL online Runlog database ***********************/
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; } 

 


  /*** create canvas, stack of histograms and histograms *************/
  //create canvas
  TCanvas*c1 = new TCanvas("c1", "Run Summary");
  //create gridlines
  c1->SetGrid();
  //color the canvas
  c1->SetFillColor(20);

  //create a pointer to a stack of histograms
  /* An individual histogram can only be one color. A stack of 
     histograms was used so that the final result would look like one 
     histogram which was color coded. */
  THStack * RunSummary= new THStack("RunSummary","Run Summary");
  

  /*create pointer to histogram pretty which will be used for purely 
    aesetic reasons.  (It will separate the down, left, right, 
    and up data.)*/
  TH1D * pretty = new TH1D("pretty", "pretty", 63, 0, 63);
    //set the fill color, line color and pattern of the bars in pretty
    pretty->SetFillColor(28);
    pretty->SetLineColor(20);
    pretty->SetFillStyle(3018);
 
  TH1D * tq = new TH1D("tq", "Total Quartz", 63, 0, 63);
    tq->SetFillColor(42);
  TH1D * tag = new TH1D("tag", "Total Quartz", 63, 0, 63);
    tag->SetFillColor(45);
  TH1D * tak = new TH1D("tak3", "Total Quartz", 63, 0, 63);
    tak->SetFillColor(30);
  TH1D *ttarget[] = {tq, tag, tak};

  /*create a histogram for each target that the data was taken on: 
    Quartz, Silver, and AK3.
    Set a different fill color for each histogram*/
  TH1D * hq = new TH1D("hq", "Quartz", 63, 0, 63);
    hq->SetFillColor(38);
  TH1D * hag= new TH1D("hag", "Ag", 63, 0, 63);
    hag->SetFillColor(50);
  TH1D * hak= new TH1D("hak", "AK3", 63, 0, 63);
    hak->SetFillColor(29);

  //create an array of the histogram pointers. 
  //This will make filling the histograms with data easier
  TH1D * htarget[] = {hq, hag, hak};




  /*** extract information from MySQL online Runlog database *********/
  // extract data size w.r.t hinge orientation collected with each target 

  /*Query MySQL.  MySQL will return 3 columns: target, target_magnet, sum(size).
   MySQL calculates the total amount of data that was taken on each target 
   with different orientations of the magnet and lists this value in the 
   column sum(size).*/
  
  string query[4];
  query[0] = "select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 61377 and r.run < 63775 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet";
  query[1] = "select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 63774 and r.run < 64205 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet";
  query[2] = "select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 64204 and r.run < 65461 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet";
  query[3] = "select target, target_magnet,sum(N*tau*0.451*0.776/20.0/1e9*nsegments/nevents_analyzed) from Runlog as r left join RunInfo using (run) left join RunScalers using (run) where tau between 1900 and 2400 and quality = 'Y' and time >10 and starttime >0 and stoptime >0 and r.run > 65553 and N is not null and nevents_analyzed>0 and nsegments is not null group by target, target_magnet"; 

  Double_t fdata[12];
  //int counter = 0;
  for (int i=0; i<11; i++)
      fdata[i] = 0.;

  

  for (int l=0; l<4; l++){
    /*TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

    if ( !db ) {cout << "CONNECTION ERROR\n";  return; }*/

    TString ask(query[l]);
  
    TSQLResult *res = db->Query(ask);
    //TSQLResult *res = db->Query(
    
    //how many rows did the query return?
    int nrows = res->GetRowCount();

    //create an array of pointers called mdata that is of class mquery and is 
    //big enough to point to enough space for all of the data from the query
    //mquery * mdata[nrows];
    mquery **mdata = new mquery*[nrows];

    //store the data to the locations just created using functions defined 
    //in the mquery class and other fancy root-SQL commands
    for (int i=0; i<nrows; i++ ) { 

      TSQLRow *row = res->Next();

      mdata[i] = new mquery;

      /*for (int m=0; m<nrows; m++)
        mdata[m]=0.;*/

      mdata[i]->SetTarget(row->GetField(0));
      mdata[i]->SetMagnet(row->GetField(1));
      mdata[i]->SetSize(atof(row->GetField(2)));
    
      //print the data to the screen using fuctions defined in the 
      //mquery class
      cout << "i = " << i <<  endl;
      cout << "target is: " << mdata[i]->GetTarget() << endl;
      cout << "magnet is oriented: " << mdata[i]->GetMagnet() << endl;
      cout << "total muons collected: " << mdata[i]->GetSize() << endl;
      cout << "-----------------------------------------------" << endl;
  
      delete row; 
    
    }

    
   
    /*** fill the histogram with data **********************************/


    /*Use the string find function to figure out which histogram and 
      bin each data piece should be assigned to */
    /*tbin is the string used to figure out what target the data was 
      taken on and mbin used to determine the orientation of the magnet.
      The first letters of the targets or magnet orientations are left out   
      because I wanted to avoid any problems with case sensitivity. */
   
    string tbin[3] = {"uartz", "g", "3"};
    string mbin[4] = {"own", "eft", "ight", "p"};
    size_t tfound = 0;
    size_t mfound = 0;
    Double_t ldata[3]= {0., 0., 0.};
    
      
  
 

    for (int i=0; i<nrows; i++) {
   
      for (int k=0; k<4; k++) {
  
        for (int j=0; j<3; j++) { 
          
          //cout << "tfound: " <<tfound << endl;
          //cout << "mfound: " <<mfound << endl;
          //cout << "string::npos: " << string::npos << endl;

          tfound=mdata[i]->GetTarget().find(tbin[j]);
	  mfound=mdata[i]->GetMagnet().find(mbin[k]);
          //(t/h)found will equal npos if it does not find the string it
          //was searching for
                
	  //if the data was found then it is assigned to the appropriate
          //histogram and bin
          if (tfound!=string::npos &&  mfound!=string::npos) {
            //cout << "data found" << endl;
            
            for (int t=0; t<4; t++)
              ttarget[j]->Fill((4*j+4+l*15+t)-.5, mdata[i]->GetSize());
            htarget[j]->SetBinContent((j+1)*4+k+l*15, mdata[i]->GetSize()); 
                       


            /*if (j==0){
              htarget[j]->SetBinContent(j+4+k+l*15, mdata[i]->GetSize());{ 
       	      //cout << "Quartz" << j+4+k+l*15 << endl;
	    }
	    else if (j==1){
              htarget[j]->SetBinContent(j+7+k+l*15, mdata[i]->GetSize());
              //cout << "Ag" << j+7+k+l*15  << endl;
            }
            else if (j==2){
              htarget[j]->SetBinContent(j+10+k+l*15, mdata[i]->GetSize());  
	      //cout << "AK3" << j+10+k+l*15 << endl;
            }
            else {
              cout << "error assigning data to bin\n";
            }*/

            ldata[j] += mdata[i]->GetSize(); 
	      //fdata[counter] =  mdata[i]->GetSize();
	      //counter++;
            
            
          
          } 
        }
      }
      for (int j=0; j<3; j++)
	fdata[3*l+j]=ldata[j];
    }
    delete mdata;
    delete res;
    //delete ask;
    //db->Close();
  }
  //delete res;
  db->Close();

  //delete mdata;
  


  
  /*** Draw the Histogram!*******************************************/ 

  //find the maximum data size value to set the Y axis of the final 
  //histogram appropriately and to fill the "pretty histogram"

  int ftotal = sizeof(fdata)/sizeof(Double_t);
  Double_t fmax = Max(fdata, ftotal);

  for (int i=0; i<12; i++)
  cout << "fdata:" << i << ": " << fdata[i] << endl;
  //for (int i=0; i<nrows; i++)
  //  cout << "mdata:" << i << ": " << mdata[i]->GetSize() << endl;
  cout << fmax << endl;  
  cout << fmax*13./12. << endl;

  RunSummary->SetMaximum(fmax*13./12.);

  for (int i=1; i<62; i+= 15){
    pretty->SetBinContent(i, fmax*13./12.);
    pretty->SetBinContent(i+1, fmax*13./12.);
    pretty->SetBinContent(i+2, fmax*13./12.);
  }
  pretty->SetAxisRange(0, fmax*13./12., "Y");

  RunSummary->Add(pretty);
  
  for (int i=0; i<3; i++)
    RunSummary->Add(ttarget[i]);

  for (int i=0; i<3; i++) 
    RunSummary->Add(htarget[i]);

  for (int i=0; i<3; i++)
    htarget[i]->SetAxisRange(0, fmax*13./12., "Y");



   
  /*Double_t fmax;
    fmax = HingeOrient->GetMaximum(); */

  RunSummary->Draw("nostack");
       
    RunSummary->GetXaxis()->SetTitle("Magnet Orientation");
    RunSummary->GetYaxis()->SetTitle("Giga-muons");
    RunSummary->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        RunSummary->GetXaxis()->SetBinLabel(i+j, "D");
        RunSummary->GetXaxis()->SetBinLabel(1+i+j, "L");
        RunSummary->GetXaxis()->SetBinLabel(2+i+j, "R");
        RunSummary->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    RunSummary->GetXaxis()->SetBinLabel(1, " ");  
    RunSummary->GetXaxis()->SetBinLabel(2, " ");      
    RunSummary->GetXaxis()->SetBinLabel(3, " ");      
    RunSummary->GetXaxis()->SetBinLabel(16, " ");
    RunSummary->GetXaxis()->SetBinLabel(17, " ");      
    RunSummary->GetXaxis()->SetBinLabel(18, " ");      
    RunSummary->GetXaxis()->SetBinLabel(31, " ");
    RunSummary->GetXaxis()->SetBinLabel(32, " ");     
    RunSummary->GetXaxis()->SetBinLabel(33, " ");     
    RunSummary->GetXaxis()->SetBinLabel(46, " ");
    RunSummary->GetXaxis()->SetBinLabel(47, " ");     
    RunSummary->GetXaxis()->SetBinLabel(48, " ");     
    RunSummary->GetXaxis()->SetBinLabel(61, " ");
    RunSummary->GetXaxis()->SetBinLabel(62, " ");
    RunSummary->GetXaxis()->SetBinLabel(63, " ");

    tq->GetXaxis()->SetTitle("Magnet Orientation");
    tq->GetYaxis()->SetTitle("Giga-muons");
    tq->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        tq->GetXaxis()->SetBinLabel(i+j, "D");
        tq->GetXaxis()->SetBinLabel(1+i+j, "L");
        tq->GetXaxis()->SetBinLabel(2+i+j, "R");
        tq->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    tq->GetXaxis()->SetBinLabel(1, " ");  
    tq->GetXaxis()->SetBinLabel(2, " ");      
    tq->GetXaxis()->SetBinLabel(3, " ");      
    tq->GetXaxis()->SetBinLabel(16, " ");
    tq->GetXaxis()->SetBinLabel(17, " ");      
    tq->GetXaxis()->SetBinLabel(18, " ");      
    tq->GetXaxis()->SetBinLabel(31, " ");
    tq->GetXaxis()->SetBinLabel(32, " ");     
    tq->GetXaxis()->SetBinLabel(33, " ");     
    tq->GetXaxis()->SetBinLabel(46, " ");
    tq->GetXaxis()->SetBinLabel(47, " ");     
    tq->GetXaxis()->SetBinLabel(48, " ");     
    tq->GetXaxis()->SetBinLabel(61, " ");
    tq->GetXaxis()->SetBinLabel(62, " ");
    tq->GetXaxis()->SetBinLabel(63, " ");


    tag->GetXaxis()->SetTitle("Magnet Orientation");
    tag->GetYaxis()->SetTitle("Giga-muons");
    tag->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        tag->GetXaxis()->SetBinLabel(i+j, "D");
        tag->GetXaxis()->SetBinLabel(1+i+j, "L");
        tag->GetXaxis()->SetBinLabel(2+i+j, "R");
        tag->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    tag->GetXaxis()->SetBinLabel(1, " ");  
    tag->GetXaxis()->SetBinLabel(2, " ");      
    tag->GetXaxis()->SetBinLabel(3, " ");      
    tag->GetXaxis()->SetBinLabel(16, " ");
    tag->GetXaxis()->SetBinLabel(17, " ");      
    tag->GetXaxis()->SetBinLabel(18, " ");      
    tag->GetXaxis()->SetBinLabel(31, " ");
    tag->GetXaxis()->SetBinLabel(32, " ");     
    tag->GetXaxis()->SetBinLabel(33, " ");     
    tag->GetXaxis()->SetBinLabel(46, " ");
    tag->GetXaxis()->SetBinLabel(47, " ");     
    tag->GetXaxis()->SetBinLabel(48, " ");     
    tag->GetXaxis()->SetBinLabel(61, " ");
    tag->GetXaxis()->SetBinLabel(62, " ");
    tag->GetXaxis()->SetBinLabel(63, " ");

    tak->GetXaxis()->SetTitle("Magnet Orientation");
    tak->GetYaxis()->SetTitle("Giga-muons");
    tak->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        tq->GetXaxis()->SetBinLabel(i+j, "D");
        tq->GetXaxis()->SetBinLabel(1+i+j, "L");
        tq->GetXaxis()->SetBinLabel(2+i+j, "R");
        tq->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    tak->GetXaxis()->SetBinLabel(1, " ");  
    tak->GetXaxis()->SetBinLabel(2, " ");      
    tak->GetXaxis()->SetBinLabel(3, " ");      
    tak->GetXaxis()->SetBinLabel(16, " ");
    tak->GetXaxis()->SetBinLabel(17, " ");      
    tak->GetXaxis()->SetBinLabel(18, " ");      
    tak->GetXaxis()->SetBinLabel(31, " ");
    tak->GetXaxis()->SetBinLabel(32, " ");     
    tak->GetXaxis()->SetBinLabel(33, " ");     
    tak->GetXaxis()->SetBinLabel(46, " ");
    tak->GetXaxis()->SetBinLabel(47, " ");     
    tak->GetXaxis()->SetBinLabel(48, " ");     
    tak->GetXaxis()->SetBinLabel(61, " ");
    tak->GetXaxis()->SetBinLabel(62, " ");
    tak->GetXaxis()->SetBinLabel(63, " ");






    hq->GetXaxis()->SetTitle("Magnet Orientation");
    hq->GetYaxis()->SetTitle("Giga-muons");
    hq->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        hq->GetXaxis()->SetBinLabel(i+j, "D");
        hq->GetXaxis()->SetBinLabel(1+i+j, "L");
        hq->GetXaxis()->SetBinLabel(2+i+j, "R");
        hq->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    hq->GetXaxis()->SetBinLabel(1, " ");  
    hq->GetXaxis()->SetBinLabel(2, " ");      
    hq->GetXaxis()->SetBinLabel(3, " ");      
    hq->GetXaxis()->SetBinLabel(16, " ");
    hq->GetXaxis()->SetBinLabel(17, " ");      
    hq->GetXaxis()->SetBinLabel(18, " ");      
    hq->GetXaxis()->SetBinLabel(31, " ");
    hq->GetXaxis()->SetBinLabel(32, " ");     
    hq->GetXaxis()->SetBinLabel(33, " ");     
    hq->GetXaxis()->SetBinLabel(46, " ");
    hq->GetXaxis()->SetBinLabel(47, " ");     
    hq->GetXaxis()->SetBinLabel(48, " ");     
    hq->GetXaxis()->SetBinLabel(61, " ");
    hq->GetXaxis()->SetBinLabel(62, " ");
    hq->GetXaxis()->SetBinLabel(63, " ");


    hag->GetXaxis()->SetTitle("Magnet Orientation");
    hag->GetYaxis()->SetTitle("Giga-muons");
    hag->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        hag->GetXaxis()->SetBinLabel(i+j, "D");
        hag->GetXaxis()->SetBinLabel(1+i+j, "L");
        hag->GetXaxis()->SetBinLabel(2+i+j, "R");
        hag->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    hag->GetXaxis()->SetBinLabel(1, " ");  
    hag->GetXaxis()->SetBinLabel(2, " ");      
    hag->GetXaxis()->SetBinLabel(3, " ");      
    hag->GetXaxis()->SetBinLabel(16, " ");
    hag->GetXaxis()->SetBinLabel(17, " ");      
    hag->GetXaxis()->SetBinLabel(18, " ");      
    hag->GetXaxis()->SetBinLabel(31, " ");
    hag->GetXaxis()->SetBinLabel(32, " ");     
    hag->GetXaxis()->SetBinLabel(33, " ");     
    hag->GetXaxis()->SetBinLabel(46, " ");
    hag->GetXaxis()->SetBinLabel(47, " ");     
    hag->GetXaxis()->SetBinLabel(48, " ");     
    hag->GetXaxis()->SetBinLabel(61, " ");
    hag->GetXaxis()->SetBinLabel(62, " ");
    hag->GetXaxis()->SetBinLabel(63, " ");

    hak->GetXaxis()->SetTitle("Magnet Orientation");
    hak->GetYaxis()->SetTitle("Giga-muons");
    hak->GetXaxis()->SetLabelSize(0.03);
    for (int i=4; i<50; i+=15){
      for (int j=0; j<12; j+=4){
        hq->GetXaxis()->SetBinLabel(i+j, "D");
        hq->GetXaxis()->SetBinLabel(1+i+j, "L");
        hq->GetXaxis()->SetBinLabel(2+i+j, "R");
        hq->GetXaxis()->SetBinLabel(3+i+j, "U");
      }
    } 
    hak->GetXaxis()->SetBinLabel(1, " ");  
    hak->GetXaxis()->SetBinLabel(2, " ");      
    hak->GetXaxis()->SetBinLabel(3, " ");      
    hak->GetXaxis()->SetBinLabel(16, " ");
    hak->GetXaxis()->SetBinLabel(17, " ");      
    hak->GetXaxis()->SetBinLabel(18, " ");      
    hak->GetXaxis()->SetBinLabel(31, " ");
    hak->GetXaxis()->SetBinLabel(32, " ");     
    hak->GetXaxis()->SetBinLabel(33, " ");     
    hak->GetXaxis()->SetBinLabel(46, " ");
    hak->GetXaxis()->SetBinLabel(47, " ");     
    hak->GetXaxis()->SetBinLabel(48, " ");     
    hak->GetXaxis()->SetBinLabel(61, " ");
    hak->GetXaxis()->SetBinLabel(62, " ");
    hak->GetXaxis()->SetBinLabel(63, " ");

    TLegend *legend = new TLegend(0.77,0.72,0.88,0.89);
    legend->SetFillColor(10);
    legend->AddEntry(tq, "Quartz", "f");
    legend->AddEntry(tag, "Ag", "f");
    legend->AddEntry(tak, "AK3", "f"); 
    legend->SetHeader("Target");
    legend->Draw();

   

}
