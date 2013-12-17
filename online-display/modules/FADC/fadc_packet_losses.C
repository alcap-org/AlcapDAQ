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
  TH1* fadc_hist = (TH1 *)getObject("hNOctalFADCEventsWithPacketLossPercent");
  fadc_hist->GetYaxis()->SetRangeUser(0., 1.2);
  fadc_hist->Draw();
  
}
