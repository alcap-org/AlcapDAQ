void caen_wfd_errors()
{
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  //AlCapCanvas->Divide(3,1);

  //  TH1* hDT5720Errors = getObject("hDT5720Errors");
  TH1* hDT5730Errors = getObject("hDT5730Errors");
  //TH1* hV1724Errors = getObject("hV1724Errors");
  //AlCapCanvas->cd(1); hDT5720Errors->Draw();
  /*AlCapCanvas->cd(2);*/ hDT5730Errors->Draw();
  //AlCapCanvas->cd(3); hV1724Errors->Draw();
}
