thick_right_slow_silicon_heights()
{
  /*****************************************************************/
  // Prepare the canvas

  gStyle->SetOptStat("ne");

  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Divide(2,2);

  /*****************************************************************/

  AlCapCanvas->cd(1);
  TH1 *right_1 = getHist("hSiR1_1Slow_Heights");
  right_1->Draw();
  
  AlCapCanvas->cd(2);
  TH1 *right_2 = getHist("hSiR1_2Slow_Heights");
  right_2->Draw();
  
  AlCapCanvas->cd(3);
  TH1 *right_3 = getHist("hSiR1_3Slow_Heights");
  right_3->Draw();
  
  AlCapCanvas->cd(4);
  TH1 *right_4 = getHist("hSiR1_4Slow_Heights");
  right_4->Draw();

}
