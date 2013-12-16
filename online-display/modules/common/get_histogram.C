#include <string>
#include <iostream>

#include "TDirectory.h"
#include "TH1.h"
#include "getHist.h"

TH1* get_histogram(std::string bank_name, std::string hist_type,std::string folder="")
{
  /*****************************************************************/
  // Prepare the canvas
  //  gStyle->SetOptStat("ne"); 
  //  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  //  canvas->Clear();
  //  canvas->Divide(8,3);
  /*****************************************************************/
  std::string histname = "h" + bank_name + "_" + hist_type;
  if(!folder.empty()) histname=folder+"/"+histname;
  cout<<"Fetching: "<<histname<<endl;
  return (TH1 *)getObject(histname.c_str());

}
