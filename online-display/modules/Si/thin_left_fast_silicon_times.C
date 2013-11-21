void thin_left_fast_silicon_times()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
	TH1 *left_slow[4];
	char histTitle[1024];
	for (int j = 0; j < 4; ++j)
	{
		sprintf(histTitle,"hSiL1_%dFast_Times",j+1);
		left_slow[j] = (TH1 *)gDirectory->Get(histTitle);
		AlCapCanvas->cd(j+1);
		left_slow[j]->Draw();
	}
}
