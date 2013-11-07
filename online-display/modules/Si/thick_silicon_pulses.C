void thick_silicon_pulses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
	TH1 *thick_left_island = (TH1 *)gDirectory->Get("hSiL2Fast_Island1;1");
	AlCapCanvas->cd(1);
	thick_left_island->Draw();

	TH1 *thick_left_island = (TH1 *)gDirectory->Get("hSiL2Slow_Island1;1");
	AlCapCanvas->cd(2);
	thick_left_island->Draw();
	
	TH1 *thick_right_island = (TH1 *)gDirectory->Get("hSiR2Fast_Island1;1");
	AlCapCanvas->cd(3);
	thick_right_island->Draw();
	
	TH1 *thick_right_island = (TH1 *)gDirectory->Get("hSiR2Slow_Island1;1");
	AlCapCanvas->cd(4);
	thick_right_island->Draw();	
}
