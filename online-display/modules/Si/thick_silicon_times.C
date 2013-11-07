void thick_silicon_times()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
	TH1 *thick_left_fast = (TH1 *)gDirectory->Get("hSiL2Fast_Times");
	AlCapCanvas->cd(1);
	thick_left_fast->Draw();

	TH1 *thick_left_slow = (TH1 *)gDirectory->Get("hSiL2Slow_Times");
	AlCapCanvas->cd(2);
	thick_left_slow->Draw();
	
	TH1 *thick_right_fast = (TH1 *)gDirectory->Get("hSiR2Fast_Times");
	AlCapCanvas->cd(3);
	thick_right_fast->Draw();
	
	TH1 *thick_right_slow = (TH1 *)gDirectory->Get("hSiR2Slow_Times");
	AlCapCanvas->cd(4);
	thick_right_slow->Draw();	
}
