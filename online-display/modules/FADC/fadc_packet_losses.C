void fadc_packet_losses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  /*****************************************************************/
  
  AlCapCanvas->cd();
  TH1* fadc_hist = (TH1 *)getObject("hNOctalFADCAvgPacketLoss");
  fadc_hist->Draw();
  
}
