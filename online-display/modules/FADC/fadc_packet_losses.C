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
  TH2* fadc_hist = (TH2 *)gDirectory->Get("hNOctalFADCAvgPacketLoss");
  fadc_hist->Draw("COLZ");
  
}
