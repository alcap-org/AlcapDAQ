void thick_scintillator_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,3);
  /*****************************************************************/
	TH1 *muSc = (TH1 *)gDirectory->Get("hmuSc_Heights");
	AlCapCanvas->cd(1);
	muSc->Draw();

	TH1 *muScA = (TH1 *)gDirectory->Get("hmuScA_Heights");
	AlCapCanvas->cd(2);
	muScA->Draw();
	
	TH1 *ScL = (TH1 *)gDirectory->Get("hScL_Heights");
	AlCapCanvas->cd(3);
	ScL->Draw();
	
	TH1 *ScR = (TH1 *)gDirectory->Get("hScR_Heights");
	AlCapCanvas->cd(4);
	ScR->Draw();
	
	TH1 *ScGe = (TH1 *)gDirectory->Get("hScGe_Heights");
	AlCapCanvas->cd(5);
	ScGe->Draw();
}
