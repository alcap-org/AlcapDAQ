thick_left_fast_silicon_heights()
{
  /*****************************************************************/
  // Prepare the canvas

  gStyle->SetOptStat("ne");

  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Divide(2,2);

  /*****************************************************************/

  AlCapCanvas->cd(1);
  TH1 *left_1 = getHist("hSiL1_1Fast_Heights");
  left_1->Draw();
  
  AlCapCanvas->cd(2);
  TH1 *left_2 = getHist("hSiL1_2Fast_Heights");
  left_2->Draw();
  
  AlCapCanvas->cd(3);
  TH1 *left_3 = getHist("hSiL1_3Fast_Heights");
  left_3->Draw();
  
  AlCapCanvas->cd(4);
  TH1 *left_4 = getHist("hSiL1_4Fast_Heights");
  left_4->Draw();

}
