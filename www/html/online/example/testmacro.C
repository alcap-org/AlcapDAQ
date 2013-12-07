#include "../common/init.C"
#include "../common/gethist.C"


void testmacro(const int run_nr = 0) 
{

  //=========================================================
  //   *** some initialization for pretty print view ***
  //=========================================================

  init();

  //=========================================================
  //                  *** create canvas ***
  //=========================================================
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,600);
  c1->Draw();
  c1->cd();

  //=========================================================
  //      *** receive histograms for given run_nr ***
  //=========================================================


  TH2D  *h2 = (TH2D*) get_hist("WFDRawTimeG0", run_nr);

  //=========================================================
  //      *** plot the histogram ***
  //=========================================================

  if ( h2 ) {

     h2->Draw("ColZ");
  }   

  //=========================================================
  //      *** save canvas as PNG image file ***
  //=========================================================

  c1->SaveAs("../png/testmacro.png");

  //=========================================================
  //   *** finally, you can event put some comments  ***
  //       on the WEB page, which will appear below
  //       your picture in HTML format!
  //=========================================================

  ofstream *os = new ofstream("../html/testmacro.html");
  *os << "<h2>Hello, Word!</h2>";
  *os << "<font color=\"#0000FF\">Congratulation! You have created your first on-line plot!</font>";  
  delete os;

}
