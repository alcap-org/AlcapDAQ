thin_silicon_heights()
{
  /*****************************************************************/
  // Prepare the canvas

  gStyle->SetOptStat("ne");

  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Divide(2,2);

  /*****************************************************************/

  AlCapCanvas->cd(1);
  TH1 *left_slow = getHist("hSiL2Slow_Heights");
  left_slow->Draw();
  
  AlCapCanvas->cd(2);
  TH1 *right_slow = getHist("hSiR2Slow_Heights");
  right_slow->Draw();
  
  AlCapCanvas->cd(3);
  TH1 *left_fast = getHist("hSiL2Fast_Heights");
  left_fast->Draw();
  
  AlCapCanvas->cd(4);
  TH1 *right_fast = getHist("hSiR2Fast_Heights");
  right_fast->Draw();

}
