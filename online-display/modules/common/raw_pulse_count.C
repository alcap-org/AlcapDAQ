void raw_pulse_count()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  /*****************************************************************/
  
  AlCapCanvas->cd();
  TH1* hist = (TH1 *)getObject("hHitRawCount");
  hist->Draw();
  
}
