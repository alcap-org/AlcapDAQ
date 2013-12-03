#include <string>

#include "TDirectory.h"
#include "TH1.h"
#include "TOnlineFrame.h"

TH1* get_histogram(std::string bank_name, std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas
  //  gStyle->SetOptStat("ne"); 
  //  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  //  canvas->Clear();
  //  canvas->Divide(8,3);
  /*****************************************************************/
	TOnlineFrame *onlineFrame = (TOnlineFrame *)gClient->GetRoot();
	//onlineFrame->Print();
  std::string histname = "h" + bank_name + "_" + hist_type;
	return (TH1 *)gDirectory->Get(histname.c_str());

}
