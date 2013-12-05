#include <string>

#include "TDirectory.h"
#include "TH2.h"
#include "getHist.h"

TH2* get_histogram_2d(std::string bank_name, std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas
  //  gStyle->SetOptStat("ne"); 
  //  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  //  canvas->Clear();
  //  canvas->Divide(8,3);
  /*****************************************************************/
  std::string histname = "h" + bank_name + "_" + hist_type;
  return (TH2 *)getObject(histname.c_str());

}
