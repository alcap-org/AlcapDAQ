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
	const int max_pulses = 10;
	TH1 *right_fast_pulse[4][max_pulses];
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	for (int j = 0; j < 4; ++j) // loop through the channels
	{
		sprintf(islandHistTitle,"hSiR1_%dFast_Event1_Island38;1",j+1);
		right_fast_island[j] = (TH1 *)gDirectory->Get(islandHistTitle);
		
		AlCapCanvas->cd(j+1);
		if (right_fast_island[j])
			right_fast_island[j]->Draw();
		
		// loop through pulses and find them all
		int pulse_number = 1;
		bool final_pulse = false;
		
		while (final_pulse != true) {
			sprintf(pulseHistTitle,"hSiR1_%dFast_Event1_Island38_Pulse%d;1",j+1, pulse_number);
			right_fast_pulse[j][pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
			
			if (right_fast_pulse[j][pulse_number-1]) {
				right_fast_pulse[j][pulse_number-1]->SetLineColor(kMagenta);
				right_fast_pulse[j][pulse_number-1]->Draw("SAME");
				pulse_number++;
			}
			else
				final_pulse = true;
		}
	}
}
