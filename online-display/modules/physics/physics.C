void physics()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
  
  // Do the dEdx plots first
  AlCapCanvas->cd(1);
  TH2* left_hist = (TH2 *)getObject("hdEdx_SiL");
  left_hist->Draw("COLZ");

  AlCapCanvas->cd(2);
  TH2* right_hist = (TH2 *)getObject("hdEdx_SiR");
  right_hist->Draw("COLZ");
  
  // Now do the full germanium and the zoom in
  AlCapCanvas->cd(3);
  gPad->SetLogy();
  TH1* ge_full = (TH1 *)getObject("hGeSpectrum");
  ge_full->Draw();
}
