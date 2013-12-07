// ==================================================================
//                    *** Ball_Z *** 
// ==================================================================
/* This program extracts information from the MySQL online Runlog 
   database and creates a histogram using this information. The 
   information it extracts is the sum total amount of data that Mulan 
   Summer 2007 has taken on each target with ball_z in 
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


//create class bquery which will collect and return the values 
//extracted from the MySQL database
class bquery {
  private: 
    string   target;
    string   magnet;
    Double_t bsize;
  public: 
    string   GetTarget(){return target;}
    string   GetMagnet(){return magnet;}
    Double_t GetSize(){return bsize;}
    void     SetTarget(string tset) {target = tset;}
    void     SetBallz(string bset) {ballz = bset;}
    void     SetSize(Double_t sset) {bsize = sset;}

};  

Double_t Max(const Double_t *Numbers, const int Count)
{
  Double_t Maximum = Numbers[0];
  for(int i = 0; i < Count; i++)
    if( Maximum < Numbers[i] )
      Maximum = Numbers[i];

  return Maximum;
}





void ball_z()
{
  /*** connect to MySQL online Runlog database ***********************/
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

 


  /*** create canvas, stack of histograms and histograms *************/
  //create canvas
  c1 = new TCanvas("c1", "Ball_z Position");
  //create gridlines
  c1->SetGrid();
  //color the canvas
  c1->SetFillColor(20);

  //create a pointer to a stack of histograms
  /* An individual histogram can only be one color. A stack of 
     histograms was used so that the final result would look like one 
     histogram which was color coded. */
  THStack * Ballz= new THStack("Ball_z Position","Data Taken w.r.t Ball_z Position");
  

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

  /*Query MySQL.  MySQL will return 3 columns: target, ball_z, sum(muons).
   MySQL calculates the total amount of muons that were collected on each 
   target with different z positions of the ball and lists this value in the 
   column sum(muons).*/

  TSQLResult *res = db->Query("select target, "
                              "ball_z, "
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
                              "target_ballz"
                              );

  //how many rows did the query return?
  const int nrows = res->GetRowCount();

  //create an array of pointers called bdata that is of class bquery and is 
  //big enough to point to enough space for all of the data from the query
  bquery * bdata [nrows];
//  bquery * bdata = new bquery[nrows];
  //store the data to the locations just created using functions defined 
  //in the bquery class and other fancy root-SQL commands
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    bdata[i] = new bquery;

    bdata[i]->SetTarget(row->GetField(0));
    bdata[i]->SetBallz(row->GetField(1));
    bdata[i]->SetSize(atof(row->GetField(2)));
    
    //print the data to the screen using fuctions defined in the 
    //bquery class
    cout << "i = " << i <<  endl;
    cout << "target is: " << bdata[i]->GetTarget() << endl;
    cout << "ball_z position: " << bdata[i]->GetBallz() << endl;
    cout << "total muons collected: " << bdata[i]->GetSize() << endl;
    cout << "-----------------------------------------------" << endl;
  
    delete row; 
    
  }

  delete res;
  db->Close();
   
  /*** fill the histogram with data **********************************/


  /*Use the string find function to figure out which histogram and 
    bin each data piece should be assigned to */
  /*tbin is the string used to figure out what target the data was 
    taken on and bbin used to determine the orientation of the ballz.
    The first letters of the targets or ballz orientations are left out
    because I wanted to avoid any problems with case sensitivity. */
  string tbin[3] = {"uartz", "g", "3"};
  string bbin[4] = {"own", "eft", "ight", "p"};
  size_t tfound;
  size_t bfound;
  
  Double_t fdata [17];
  int counter = 0;

  for (i=0; i<17; i++)
    fdata[i] = 0.;

  for ( int i=0; i<nrows; i++) {
   
    for (int j=0; j<3; j++) {
  
      for (int k=0; k<4; k++) { 
      
           tfound=bdata[i]->GetTarget().find(tbin[j]);
	   mfound=bdata[i]->GetBallz().find(bbin[k]);
           //(t/h)found will equal npos if it does not find the string it
           //was searching for
                
	//if the data was found then it is assigned to the appropriate
        //histogram and bin
        if (tfound!=string::npos &&  bfound!=string::npos) {
          cout << "data found" << endl;

            
          if (k==0)
            htarget[j]->SetBinContent(j+2, bdata[i]->GetSize());
          else if (k==1)
            htarget[j]->SetBinContent(j+6, bdata[i]->GetSize());
          else if (k==2)
            htarget[j]->SetBinContent(j+10, bdata[i]->GetSize());  
          else if (k==3)
            htarget[j]->SetBinContent(j+14, bdata[i]->GetSize());
          else
            cout << "error assigning data to bin\n";

          fdata[counter] = bdata[i]->GetSize();
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
    cout << "hdata:" << i << ": " << bdata[i]->GetSize() << endl;
  cout << fmax << endl;  
  cout << fmax*13./12. << endl;

  Ballz->SetMaximum(fmax*13./12.);

  for (int i=1; i<18; i+= 4)
    pretty->SetBinContent(i, fmax*13./12.);
  pretty->SetAxisRange(0, fmax*13./12., "Y");

  Ballz->Add(pretty);
  for (int i=0; i<3; i++) 

  Ballz->Add(htarget[i]);
  for (int i=0; i<3; i++)
      htarget[i]->SetAxisRange(0, fmax*13./12., "Y");



   
  /*Double_t fmax;
    fmax = HingeOrient->GetMaximum(); */

  Ballz->Draw("nostack");
       
    Ballz->GetXaxis()->SetTitle("Ball_z Position");
    Ballz->GetYaxis()->SetTitle("muons");
    Ballz->GetXaxis()->SetBinLabel(1, " ");  
    Ballz->GetXaxis()->SetBinLabel(2, " ");      //Q  D
    Ballz->GetXaxis()->SetBinLabel(3, "Down");   //Ag D
    Ballz->GetXaxis()->SetBinLabel(4, " ");      //AK D
    Ballz->GetXaxis()->SetBinLabel(5, " ");
    Ballz->GetXaxis()->SetBinLabel(6, " ");      //Q  L
    Ballz->GetXaxis()->SetBinLabel(7, "Left");   //Ag L
    Ballz->GetXaxis()->SetBinLabel(8, " ");      //AK L
    Ballz->GetXaxis()->SetBinLabel(9, " ");
    Ballz->GetXaxis()->SetBinLabel(10, " ");     //Q  R
    Ballz->GetXaxis()->SetBinLabel(11, "Right"); //Ag R
    Ballz->GetXaxis()->SetBinLabel(12, " ");     //AK R
    Ballz->GetXaxis()->SetBinLabel(13, " ");
    Ballz->GetXaxis()->SetBinLabel(14, " ");     //Q  U  
    Ballz->GetXaxis()->SetBinLabel(15, "Up");    //Ag U
    Ballz->GetXaxis()->SetBinLabel(16, " ");     //AK U
    Ballz->GetXaxis()->SetBinLabel(17, " ");

    hq->GetXaxis()->SetTitle("Ball_z Position");
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

    hag->GetXaxis()->SetTitle("Ball_z Position");
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

    hak->GetXaxis()->SetTitle("Ball_z Position");
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

    delete bdata;

}
