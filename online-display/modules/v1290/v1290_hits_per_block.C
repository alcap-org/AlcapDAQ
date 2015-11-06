void v1290_hits_per_block()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,1);

  AlCapCanvas->cd(1);
  TH1* hist = (TH1*)getObject("hTDCHitCountsPerBlock");
  if (hist) {
    hist->Draw();
  }
  AlCapCanvas->cd(2);
  hist = (TH1*)getObject("hTDCHitCountsAvg10Blocks");
  if (hist) {
    hist->Draw();
  }
}
