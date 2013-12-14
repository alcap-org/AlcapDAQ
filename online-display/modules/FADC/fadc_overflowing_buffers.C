void fadc_overflowing_buffers()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  /*****************************************************************/
  AlCapCanvas->Divide(1,2);
  AlCapCanvas->cd(1);
  TH2* fadc_hist = (TH2 *)getObject("hNOctalFADCBufferOverflow");
  fadc_hist->Draw("");
   AlCapCanvas->cd(2);
  TH2* fadc_hist = (TH2 *)getObject("hNOctalFADCBufferOverflowPercent");
  fadc_hist->Draw("");
 
  
}
