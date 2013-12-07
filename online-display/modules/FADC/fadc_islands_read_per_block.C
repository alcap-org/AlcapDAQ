void fadc_islands_read_per_block()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  /*****************************************************************/
  
  AlCapCanvas->cd();
  TH2* fadc_blocks_read = (TH2 *)getObject("hNOctalFADCIslandsReadPerBlock");
  fadc_blocks_read->Draw("COLZ");
  
}
