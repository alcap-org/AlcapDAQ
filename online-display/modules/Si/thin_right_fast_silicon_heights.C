void thin_right_fast_silicon_heights()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
	TH1 *right_slow[4];
	char histTitle[1024];
	for (int j = 0; j < 4; ++j)
	{
		sprintf(histTitle,"hSiR1_%dFast_Heights",j+1);
		right_slow[j] = (TH1 *)gDirectory->Get(histTitle);
		AlCapCanvas->cd(j+1);
		right_slow[j]->Draw();
	}
}
