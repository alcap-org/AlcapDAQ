void thick_germanium_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(1,2);
  /*****************************************************************/
	TH1 *thick_left_fast = (TH1 *)gDirectory->Get("hGeFast_Heights");
	AlCapCanvas->cd(1);
	thick_left_fast->Draw();

	TH1 *thick_left_slow = (TH1 *)gDirectory->Get("hGeSlow_Heights");
	AlCapCanvas->cd(2);
	thick_left_slow->Draw();	
}
