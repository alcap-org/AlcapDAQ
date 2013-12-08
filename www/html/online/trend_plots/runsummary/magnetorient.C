// ==================================================================
//                    *** Magnet Orientation *** 
// ==================================================================
/* This program extracts information from the MySQL online Runlog 
   database and creates a histogram using this information. The 
   information it extracts is the sum total amount of data that Mulan 
   Summer 2007 has taken on each target with magnet orientation in 
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





void magnetorient()
{
  /*** connect to MySQL online Runlog database ***********************/
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

 


  /*** create canvas, stack of histograms and histograms *************/
  //create canvas
  c1 = new TCanvas("c1", "Magnet Orientation");
  //create gridlines
  c1->SetGrid();
  //color the canvas
  c1->SetFillColor(20);

  //create a pointer to a stack of histograms
  /* An individual histogram can only be one color. A stack of 
     histograms was used so that the final result would look like one 
     histogram which was color coded. */
  THStack * Magnet= new THStack("MagnetOrient","Data Taken w.r.t Magnet Orientation");
  

  /*create pointer to histogram pretty which will be used for purely 
    aesetic reasons.  (It will separate the down, left, right, 
    and up data.)*/
  TH1D * pretty = new TH1D("pretty", "pretty", 17, 0, 17);
    //set the fill color, line color and pattern of the bars in pretty
    pretty->SetFillColor(28);
    pretty->SetLineColor(20);
    pretty->SetFillStyle(3018);
 
  /*create a histogram for each target that the data was taken on: 
    Quartz, Silver, and AK3.
    Set a different fill color for each histogram*/
  TH1D * hq = new TH1D("hq", "Quartz", 17, 0, 17);
    hq->SetFillColor(42);
  TH1D * hag= new TH1D("hag", "Ag", 17, 0, 17);
    hag->SetFillColor(50);
  TH1D * hak= new TH1D("hak", "AK3", 17, 0, 17);
    hak->SetFillColor(30);

  //create an array of the histogram pointers. 
  //This will make filling the histograms with data easier
  TH1D * htarget[] = {hq, hag, hak};
  
  


  


  /*** extract information from MySQL online Runlog database *********/
  // extract data size w.r.t hinge orientation collected with each target 

  /*Query MySQL.  MySQL will return 3 columns: target, target_magnet, sum(size).
   MySQL calculates the total amount of data that was taken on each target 
   with different orientations of the magnet and lists this value in the 
   column sum(size).*/

  TSQLResult *res = db->Query("select target, "
                              "target_magnet, "
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
                              "group by target, "
                              "target_magnet"
                              );

  //how many rows did the query return?
  const int nrows = res->GetRowCount();

  //create an array of pointers called mdata that is of class mquery and is 
  //big enough to point to enough space for all of the data from the query
  mquery * mdata [nrows];
//  mquery * mdata = new mquery[nrows];
  //store the data to the locations just created using functions defined 
  //in the mquery class and other fancy root-SQL commands
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    mdata[i] = new mquery;

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

  delete res;
  db->Close();
   
  /*** fill the histogram with data **********************************/


  /*Use the string find function to figure out which histogram and 
    bin each data piece should be assigned to */
  /*tbin is the string used to figure out what target the data was 
    taken on and mbin used to determine the orientation of the magnet.
    The first letters of the targets or magnet orientations are left out
    because I wanted to avoid any problems with case sensitivity. */
  string tbin[3] = {"uartz", "g", "3"};
  string mbin[4] = {"own", "eft", "ight", "p"};
  size_t tfound;
  size_t mfound;
  
  Double_t fdata [17];
  int counter = 0;

  for (i=0; i<17; i++)
    fdata[i] = 0.;

  for ( int i=0; i<nrows; i++) {
   
    for (int j=0; j<3; j++) {
  
      for (int k=0; k<4; k++) { 
      
           tfound=mdata[i]->GetTarget().find(tbin[j]);
	   mfound=mdata[i]->GetMagnet().find(mbin[k]);
           //(t/h)found will equal npos if it does not find the string it
           //was searching for
                
	//if the data was found then it is assigned to the appropriate
        //histogram and bin
        if (tfound!=string::npos &&  mfound!=string::npos) {
          cout << "data found" << endl;

            
          if (k==0)
            htarget[j]->SetBinContent(j+2, mdata[i]->GetSize());
          else if (k==1)
            htarget[j]->SetBinContent(j+6, mdata[i]->GetSize());
          else if (k==2)
            htarget[j]->SetBinContent(j+10, mdata[i]->GetSize());  
          else if (k==3)
            htarget[j]->SetBinContent(j+14, mdata[i]->GetSize());
          else
            cout << "error assigning data to bin\n";

          fdata[counter] = mdata[i]->GetSize();
          counter++;
          
          
        } 
      }
    }
  }

  

  //delete hdata;
  


  
  /*** Draw the Histogram!*******************************************/ 

  //find the maximum data size value to set the Y axis of the final 
  //histogram appropriately and to fill the "pretty histogram"

  int ftotal = sizeof(fdata)/sizeof(Double_t);
  Double_t fmax = Max(fdata, ftotal);

  cout << "fdata:" << i << ": " << fdata[i] << endl;
  for (i=0; i<nrows; i++)
    cout << "hdata:" << i << ": " << mdata[i]->GetSize() << endl;
  cout << fmax << endl;  
  cout << fmax*13./12. << endl;

  Magnet->SetMaximum(fmax*13./12.);

  for (int i=1; i<18; i+= 4)
    pretty->SetBinContent(i, fmax*13./12.);
  pretty->SetAxisRange(0, fmax*13./12., "Y");

  Magnet->Add(pretty);
  for (int i=0; i<3; i++) 

  Magnet->Add(htarget[i]);
  for (int i=0; i<3; i++)
      htarget[i]->SetAxisRange(0, fmax*13./12., "Y");



   
  /*Double_t fmax;
    fmax = HingeOrient->GetMaximum(); */

  Magnet->Draw("nostack");
       
    Magnet->GetXaxis()->SetTitle("Magnet Orientation");
    Magnet->GetYaxis()->SetTitle("muons");
    Magnet->GetXaxis()->SetBinLabel(1, " ");  
    Magnet->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    Magnet->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    Magnet->GetXaxis()->SetBinLabel(4, " ");      //AK D
    Magnet->GetXaxis()->SetBinLabel(5, " ");
    Magnet->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    Magnet->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    Magnet->GetXaxis()->SetBinLabel(8, " ");      //AK L
    Magnet->GetXaxis()->SetBinLabel(9, " ");
    Magnet->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    Magnet->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    Magnet->GetXaxis()->SetBinLabel(12, " ");     //AK R
    Magnet->GetXaxis()->SetBinLabel(13, " ");
    Magnet->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    Magnet->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    Magnet->GetXaxis()->SetBinLabel(16, " ");     //AK U
    Magnet->GetXaxis()->SetBinLabel(17, " ");

    hq->GetXaxis()->SetTitle("Magnet Orientation");
    hq->GetYaxis()->SetTitle("muons");
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

    hag->GetXaxis()->SetTitle("Magnet Orientation");
    hag->GetYaxis()->SetTitle("muons");
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

    hak->GetXaxis()->SetTitle("Magnet Orientation");
    hak->GetYaxis()->SetTitle("muons");
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

    legend = new TLegend(0.77,0.72,0.88,0.89);
    legend->SetFillColor(10);
    legend->AddEntry(hq, "Quartz", "f");
    legend->AddEntry(hag, "Ag", "f");
    legend->AddEntry(hak, "AK3", "f"); 
    legend->SetHeader("Target");
    legend->Draw();

    delete mdata;

}
