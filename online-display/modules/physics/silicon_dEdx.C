void silicon_dEdx()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,1);
  /*****************************************************************/
  
  AlCapCanvas->cd(1);
  TH2* left_hist = (TH2 *)getObject("hdEdx_SiL");
  left_hist->Draw("COLZ");

  AlCapCanvas->cd(2);
  TH2* right_hist = (TH2 *)getObject("hdEdx_SiR");
  right_hist->Draw("COLZ");
  
}
