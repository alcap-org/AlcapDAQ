void caen_wfd_hits_per_block()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(1,2);

  AlCapCanvas->cd(1);
  TH1* hist = (TH1*)getObject("hWFDHitCountsPerBlock");
  if (hist) {
    hist->Draw();
  }
  AlCapCanvas->cd(2);
  hist = (TH1*)getObject("hWFDHitCountsAvg10Blocks");
  if (hist) {
    hist->Draw();
  }
}
