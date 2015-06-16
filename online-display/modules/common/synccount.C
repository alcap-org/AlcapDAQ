void synccount()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/

  TH1* d4 = (TH1 *)getObject("hSyncCount_Crate4");
  TH1* d7 = (TH1 *)getObject("hSyncCount_Crate7");
  TH1* d8 = (TH1 *)getObject("hSyncCount_Crate8");
  TH1* t4 = (TH1 *)getObject("hSyncCount_TDC");

  AlCapCanvas->cd(1); d4->Draw();
  AlCapCanvas->cd(2); d7->Draw();
  AlCapCanvas->cd(3); d8->Draw();
  AlCapCanvas->cd(4); t4->Draw();
}
