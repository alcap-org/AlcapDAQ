void thin_left_slow_silicon_pulses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
	TH1 *left_slow_island[4];
	const int max_pulses = 10;
	TH1 *left_slow_pulse[4][max_pulses];
	
	int event_number = 1;
	int island_number = 38;
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	for (int j = 0; j < 4; ++j) // loop through the channels
	{
		sprintf(islandHistTitle,"hSiL1_%dSlowRaw_Event%d_Island%d;1",j+1, event_number, island_number);
		left_slow_island[j] = (TH1 *)gDirectory->Get(islandHistTitle);
		
		AlCapCanvas->cd(j+1);
		if (left_slow_island[j])
			left_slow_island[j]->Draw();
		
		// loop through pulses and find them all
		int pulse_number = 1;
		bool final_pulse = false;
		
		while (final_pulse != true) {
			sprintf(pulseHistTitle,"hSiL1_%dSlowRaw_Event%d_Island%d_Pulse%d;1",j+1, event_number, island_number, pulse_number);
			left_slow_pulse[j][pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
			
			if (left_slow_pulse[j][pulse_number-1]) {
				left_slow_pulse[j][pulse_number-1]->SetLineColor(kMagenta);
				left_slow_pulse[j][pulse_number-1]->Draw("SAME");
				pulse_number++;
			}
			else
				final_pulse = true;
		}
	}
}
