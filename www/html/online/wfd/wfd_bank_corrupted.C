#include "TPaletteAxis.h"

#include <stdio.h>

#include "../common/init.C"
#include "../common/gethist.C"

void wfd_bank_corrupted(const int run_nr = 0) 
{
  int i;

  const int n_hists = 6;
  /***
  const char *h_names[n_hists] = { "WFDBankCorruptedR0",
				   "WFDBankCorruptedR1",
				   "WFDBankCorruptedG0",
				   "WFDBankCorruptedG1",
				   "WFDBankCorruptedB0",
				   "WFDBankCorruptedB1" };
  ***/
  const char *h_names[n_hists] = { "WFDBankCorruptedB0",
				   "WFDBankCorruptedR1",
				   "WFDBankCorruptedG1",
				   "WFDBankCorruptedR0",
				   "WFDBankCorruptedB1",
				   "WFDBankCorruptedG0" };
  
  TH2D *h2[n_hists];

  //=========================================================
  //             *** initialize style ***
  //=========================================================

  init();
  gStyle->SetOptStat(11);

  //=========================================================
  //                  *** open canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,1000);
  c1->Draw();
  c1->cd();

  c1->Divide(2,3,0.001,0.001,10);

  //=========================================================
  //          *** receive and plot histograms ***
  //=========================================================
  
  for (i=0; i<n_hists; i++) {

    h2[i] = (TH2D*) get_hist(h_names[i], run_nr);

    if ( !h2[i] ) continue;

    int ipad = i+1;   
    c1->cd(ipad);

    h2[i]->SetMinimum(0);
    h2[i]->SetMaximum(7);
    h2[i]->Draw("ColZ");
    c1->GetPad(ipad)->Update();

    h2[i]->SetXTitle("event number");
    h2[i]->SetYTitle("(slot-1)*4 + chan.");

    TPaletteAxis *palette = (TPaletteAxis*)h2[i]->GetListOfFunctions()->FindObject("palette");
    if ( palette ) {
      palette->SetX2NDC( palette->GetX1NDC()+0.01 );
      palette->Draw();
    }

  }
  
  c1->SaveAs("../png/wfd_bank_corrupted.png");

  //====================================================
  //            *** HTML output ***
  //====================================================

  ofstream *os = new ofstream("../html/wfd_bank_corrupted.html");

  int crate, slot, chan;
  char bkname[8];


  //==========================================================================
  //                     *** print error summary ***
  //==========================================================================
    
  *os << "<h2>List of corruptions</h2>\n"
      << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "  <td align=\"right\"><b> bank </b></td>\n"
      << "  <td align=\"left\"><b> Error #</b></td>\n"
      << "</tr>";


  for (crate=1; crate<=6; crate++) 
    for (slot=2; slot<=20; slot++) 
      for (chan=0; chan<=3; chan++) {
	      
      int id = crate - 1;

      Double_t y = (slot-1)*4 + chan;

      Int_t iy = h2[id]->GetYaxis()->FindBin( y );

      const int n_errors = 20;
      Bool_t err_nr[n_errors];
      
      int j;
      for (j=0; j<n_errors; j++) {
	err_nr[j] = kFALSE;
      }

      Bool_t isOk = kTRUE;
      int ix;
      for (ix=2; ix<=h2[id]->GetNbinsX(); ix++) {

	int iZ = h2[id]->GetCellContent(ix,iy);
	
	if ( iZ != 0 ) {
	  if (isOk) {
	    sprintf(bkname,"B%d%c%d",crate,'a'+(slot-1),chan);
	    *os << "<tr>\n"
		<< "  <td align=\"right\"><b> " << bkname  << " </b></td>\n"
		<< "  <td align=\"left\"><b> " ;
	  }
	  
	  if ( iZ >= n_errors ) 
	    iZ = n_errors - 1;
	  
	  if ( !err_nr[iZ] ) {
	    
	    err_nr[iZ] = kTRUE;

	    if ( isOk ) {
	      isOk = kFALSE;
	      *os << iZ  ;
	    } else {
	      *os << ", " << iZ  ;
	    }

	  }

	}	

      }

      if ( !isOk ) {
	*os << "</td></tr>\n";
      }

    }


  *os << "</table><br>\n";


  *os << "<hr><br>\n"
      << "<h2>Error codes</h2>\n"
      << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "  <td align=\"center\"><b> Error code </b></td>\n"
      << "  <td align=\"center\"><b> corruption </b></td>\n"
      << "</tr>\n";

  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 1 </b></td>\n"
      << "  <td align=\"left\"><b> reqwords==0 </b></td>\n"
      << "</tr>";

  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 2 </b></td>\n"
      << "  <td align=\"left\"><b> reqwords>=0x9000 </b></td>\n"
      << "</tr>";


  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 3 </b></td>\n"
      << "  <td align=\"left\"><b> (reqwords!=gotwords) || (status!=0 .and. reqwords>10) </b></td>\n"
      << "</tr>";

  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 4 </b></td>\n"
      << "  <td align=\"left\"><b> bank size non-divisible </b></td>\n"
      << "</tr>";

  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 5 </b></td>\n"
      << "  <td align=\"left\"><b> zeroWord != 0 </b></td>\n"
      << "</tr>";


  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 6 </b></td>\n"
      << "  <td align=\"left\"><b> bad fill counter </b></td>\n"
      << "</tr>";


  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 7 </b></td>\n"
      << "  <td align=\"left\"><b> no reset block </b></td>\n"
      << "</tr>";

  *os << "<tr>\n"
      << "  <td align=\"center\"><b> 8 </b></td>\n"
      << "  <td align=\"left\"><b> bad fill counter at the end of data segment </b></td>\n"
      << "</tr>";




  *os << "</table><br>\n";


  //====================================================
  //            *** print channel names ***
  //====================================================


  *os << "<hr><br>\n"
      << "<h2>Crate Slot Channel to Bank name covertion</h2>";


  *os << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "    <td align=\"center\" ><b> slot         </b></td>\n"
      << "    <td align=\"center\" ><b> channel      </b></td>\n"
      << "    <td align=\"center\" ><b> (slot-1)*4   </b></td>\n"
      << "    <td align=\"center\" ><b> bank         </b></td>\n"
      << "</tr>\n"
    ;

  for (slot=2; slot<=20; slot++) 
    for (chan=0; chan<=3; chan++) {
	
      sprintf(bkname,"B?%c%d",'a'+(slot-1),chan);
      
      *os << "<tr>\n"
	  << "    <td align=\"center\"><b> " << slot << " </b></td>\n"
	  << "    <td align=\"center\"><b> " << chan    << " </b></td>\n"
	  << "    <td align=\"center\"><b> " << (slot-1)*4+chan    << " </b></td>\n"
	  << "    <td align=\"left\"><b> " << bkname  << " </b></td>\n"
	  << "</tr>\n"
	;
    }

  

  *os << "</table><br>\n";




  delete os;



}
