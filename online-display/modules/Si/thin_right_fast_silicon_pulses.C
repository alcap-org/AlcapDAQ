void thin_right_fast_silicon_pulses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
	TH1 *right_fast_island[4];
	TH1 *right_fast_pulse[4];
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	for (int j = 0; j < 4; ++j)
	{
		sprintf(islandHistTitle,"hSiR1_%dFast_Island1;1",j+1);
		right_fast_island[j] = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(pulseHistTitle,"hSiR1_%dFast_Island1_Pulse1;1",j+1);
		right_fast_pulse[j] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		AlCapCanvas->cd(j+1);
		
		if (right_fast_island[j])
			right_fast_island[j]->Draw();
		
		if (right_fast_pulse[j]) {
			right_fast_pulse[j]->SetLineColor(kMagenta);
			right_fast_pulse[j]->Draw("SAME");
		}
	}
}
