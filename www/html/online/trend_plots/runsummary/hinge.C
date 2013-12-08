// ==================================================================
//                    *** Hinge Orientation *** 
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

Double_t Max(const Double_t *Numbers, const int Count)
{
  Double_t Maximum = Numbers[0];
  for(int i = 0; i < Count; i++)
    if( Maximum < Numbers[i] )
      Maximum = Numbers[i];

  return Maximum;
}





void hinge()
{
  /*** connect to MySQL online Runlog database ***********************/
  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) {cout << "CONNECTION ERROR\n";  return; }

 


  /*** create canvas, stack of histograms and histograms *************/
  //create canvas
  c1 = new TCanvas("c1", "Hinge Orientation");
  //create gridlines
  c1->SetGrid();
  //color the canvas
  c1->SetFillColor(20);

  //create a pointer to a stack of histograms
  /* An individual histogram can only be one color. A stack of 
     histograms was used so that the final result would look like one 
     histogram which was color coded. */
  THStack * HingeOrient= new THStack("HingeOrient","Data Taken w.r.t Hinge Orientation");
  

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

  /*Query MySQL.  MySQL will return 3 columns: target, emc_magnet, sum(size).
   Mid-Summer it was decided that the field emc_magnet would actually be 
   used for the position of the hinge/flapper.  Before this decision the 
   field contained entries that all read "none".  During this time when 
   the field reads "none" the flapper was positioned "left". MySQL calculates
   the total amount of data that was taken on each target with different 
   orientations of the flapper and lists this value in the column sum(muons).*/

  TSQLResult *res = db->Query("select target, "
                              "emc_magnet, "
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
                              "emc_magnet "
                              );

  //how many rows did the query return?
  const int nrows = res->GetRowCount();

  //create an array of pointers called hdata that is of class hquery and is 
  //big enough to point to enough space for all of the data from the query
  hquery * hdata [nrows];
  //store the data to the locations just created using functions defined 
  //in the hquery class and other fancy root-SQL commands
  for ( int i=0; i<nrows; i++ ) { 

    TSQLRow *row = res->Next();

    hdata[i] = new hquery;

    hdata[i]->SetTarget(row->GetField(0));
    hdata[i]->SetHinge(row->GetField(1));
    hdata[i]->SetSize(atof(row->GetField(2)));
    
    //print the data to the screen using fuctions defined in the 
    //hquery class
    cout << "i = " << i <<  endl;
    cout << "target is: " << hdata[i]->GetTarget() << endl;
    cout << "hinge is oriented: " << hdata[i]->GetHinge() << endl;
    cout << "total muons collected: " << hdata[i]->GetSize() << endl;
    cout << "-----------------------------------------------" << endl;
  
    delete row; 
    
  }

  delete res;
  db->Close();
   
  /*** fill the histogram with data **********************************/


  /*Use the string find function to figure out which histogram and 
    bin each data piece should be assigned to */
  /*tbin is the string used to figure out what target the data was 
    taken on and hbin used to determine the orientation of the hinge.
    The first letters of the targets or hinge orientations are left out
    because I wanted to avoid any problems with case sensitivity. */
  string tbin[3] = {"uartz", "g", "3"};
  string hbin[5] = {"own", "eft", "ight", "p", "one"};
  size_t tfound;
  size_t hfound;

  Double_t ldata[3]= {0., 0., 0.};
  Double_t odata[14];
  int counter = 0;
  for (i=0; i<14; i++)
    odata[i] = 0.;

  for ( int i=0; i<nrows; i ++) {
   
    for (int j=0; j<3; j++) {
  
      for (int k=0; k<5; k++) { 
      
           tfound=hdata[i]->GetTarget().find(tbin[j]);
	   hfound=hdata[i]->GetHinge().find(hbin[k]);
           //(t/h)found will equal npos if it does not find the string it
           //was searching for
                
	//if the data was found then it is assigned to the appropriate
        //histogram and bin
        if (tfound!=string::npos &&  hfound!=string::npos) {
          cout << "data found" << endl;

            
          if (k==0)
            htarget[j]->SetBinContent(j+2, hdata[i]->GetSize());
            /*(the following  else if accounts for the fact that the hinge 
            position records marked as "none" should be considered 
            "left")*/
          else if (k==1 || k==4){
            htarget[j]->Fill(j+5.5, hdata[i]->GetSize());
	    ldata[j] = ldata[j] + hdata[i]->GetSize();
	  }
          else if (k==2)
            htarget[j]->SetBinContent(j+10, hdata[i]->GetSize());  
          else if (k==3)
            htarget[j]->SetBinContent(j+14, hdata[i]->GetSize());
          else
            cout << "error assigning data to bin\n";

          //the following lines are used in preparation for setting the
          // Y axis of the final histogram and filling pretty's bins
          if (k!=1 && k!=4){
	    if (counter<14);
	      odata[counter] = hdata[i]->GetSize();
            counter++;
          }
          
        } 
      }
    }
  }

  

  


  
  /*** Draw the Histogram!*******************************************/ 

  //find the maximum data size value to set the Y axis of the final 
  //histogram appropriately and to fill the "pretty histogram"
  Double_t fdata [17];
  for (i=0; i<3; i++)
    fdata[i] = ldata[i];
  for (i=0; i<14; i++)
    fdata[i+3] = odata[i];
  int ftotal = sizeof(fdata)/sizeof(Double_t);
  Double_t fmax = Max(fdata, ftotal);

  for (i=0; i<3; i++)
    cout << "ldata:" << i << ": " << ldata[i] << endl;
  for (i=0; i<14; i++)
    cout << "odata:" << i << ": " << odata[i] << endl;
  for (i=0; i<17; i++)
    cout << "fdata:" << i << ": " << fdata[i] << endl;
  for (i=0; i<nrows; i++)
    cout << "hdata:" << i << ": " << hdata[i]->GetSize() << endl;
  cout << fmax << endl;  
  cout << fmax*13./12. << endl;

  HingeOrient->SetMaximum(fmax*13./12.);

  for (int i=1; i<18; i+= 4)
    pretty->SetBinContent(i, fmax*13./12.);
  pretty->SetAxisRange(0, fmax*13./12., "Y");

  HingeOrient->Add(pretty);
  for (int i=0; i<3; i++) 

  HingeOrient->Add(htarget[i]);
  for (int i=0; i<3; i++)
      htarget[i]->SetAxisRange(0, fmax*13./12., "Y");



   
  /*Double_t fmax;
    fmax = HingeOrient->GetMaximum(); */

  HingeOrient->Draw("nostack");
       
    HingeOrient->GetXaxis()->SetTitle("Hinge Orientation");
    HingeOrient->GetYaxis()->SetTitle("muons");
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

    hag->GetXaxis()->SetTitle("Hinge Orientation");
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

    hak->GetXaxis()->SetTitle("Hinge Orientation");
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

    delete hdata;

}
