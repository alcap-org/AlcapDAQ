
#include <fstream>

#include "TPaletteAxis.h"

#include "../common/init.C"
#include "../common/gethist.C"

void wfd_raw_area(const int run_nr = 0) 
{

  int i;

  const int n_hists = 6;
  const char *h_names[n_hists] = { "WFDRawAreaR0",
				   "WFDRawAreaR1",
				   "WFDRawAreaG0",
				   "WFDRawAreaG1",
				   "WFDRawAreaB0",
				   "WFDRawAreaB1" };
  
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

    h2[i]->Draw("ColZ");
    h2[i]->SetXTitle("waveform area");
    h2[i]->SetYTitle("(slot-1)*4 + chan.");
    c1->GetPad(ipad)->Update();
    

    TPaletteAxis *palette = (TPaletteAxis*)h2[i]->GetListOfFunctions()->FindObject("palette");
    if ( palette ) {
      palette->SetX2NDC( palette->GetX1NDC()+0.01 );
      palette->Draw();
    }

    if ( h2[i]->GetMaximum() > 10 )
      c1->GetPad(ipad)->SetLogz();

  }

  
  
  c1->SaveAs("../png/wfd_raw_area.png");

  //==========================================================
  //    *** Make list of area/sigma for each channel ***
  //==========================================================

  ofstream *os = new ofstream("../html/wfd_raw_area.html");
  *os << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "    <td align=\"center\" ><b> crate   </b></td>\n"
      << "    <td align=\"center\" ><b> slot    </b></td>\n"
      << "    <td align=\"center\" ><b> channel </b></td>\n"
      << "    <td align=\"center\" ><b> area    </b></td>\n"
      << "    <td align=\"center\" ><b> sigma   </b></td>\n"
      << "</tr>\n"
    ;

  int crate, slot, chan;
  for (crate=0; crate<n_hists; crate++) {
    int ibin = 0;
    int nbinsy = h2[crate]->GetNbinsY();
    while ( ibin <= nbinsy ) 
      for (chan=0; chan<=3; chan++) {
	
	ibin++;

	if (ibin >= nbinsy ) break;
	
	int N = int(h2[crate]->GetYaxis()->GetBinCenter(ibin));

	slot = (N-chan)/4+1;

	char name[256];
	sprintf(name,"h1_crate_%d_slot_%02d_chan_%d",crate+1,slot,chan);
	TH1D *h1 = h2[crate]->ProjectionX(name,ibin,ibin);
	
	*os << "<tr>\n"
	    << "    <td align=\"center\"><b> " << crate+1 << " </b></td>\n"
	    << "    <td align=\"center\"><b> " << slot    << " </b></td>\n"
	    << "    <td align=\"center\"><b> " << chan    << " </b></td>\n";
	os->precision(2);
	*os << "    <td align=\"right\"><b>" << fixed << h1->GetMean() << " </b></td>\n"
	    << "    <td align=\"right\"><b>" << h1->GetRMS()  << " </b></td>\n"
	    << "</tr>\n";
	;
      }
  }
  

  *os << "</table>\n";
  delete os;

}
