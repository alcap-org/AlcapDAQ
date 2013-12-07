void fadc_overflowing_buffers()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  /*****************************************************************/
  
  AlCapCanvas->cd();
  TH2* fadc_hist = (TH2 *)getObject("hNOctalFADCBufferOverflow");
  fadc_hist->Draw("COLZ");
  
}
