void thick_silicon_pulses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
  
	const int max_pulses = 10;
	int event_number = 1;
	int island_number = 38;
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	//////////////////
	// SiL2 Fast
	sprintf(islandHistTitle,"hSiL2Fast_Event%d_Island%d;1", event_number, island_number);
	TH1 *thick_left_fast_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *thick_left_fast_pulse[max_pulses];
	AlCapCanvas->cd(1);
	if (thick_left_fast_island)
		thick_left_fast_island->Draw();
		
	// loop through pulses and find them all
	int pulse_number = 1;
	bool final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiL2Fast_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		thick_left_fast_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (thick_left_fast_pulse[pulse_number-1]) {
			thick_left_fast_pulse[pulse_number-1]->SetLineColor(kMagenta);
			thick_left_fast_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	//////////////////
	// SiL2 Slow
	sprintf(islandHistTitle,"hSiL2Slow_Event%d_Island%d;1", event_number, island_number);
	TH1 *thick_left_slow_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *thick_left_slow_pulse[max_pulses];
	AlCapCanvas->cd(2);
	if (thick_left_slow_island)
		thick_left_slow_island->Draw();
	
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiL2Slow_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		thick_left_slow_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (thick_left_slow_pulse[pulse_number-1]) {
			thick_left_slow_pulse[pulse_number-1]->SetLineColor(kMagenta);
			thick_left_slow_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	//////////////////
	// SiR2 Fast
	sprintf(islandHistTitle,"hSiR2Fast_Event%d_Island%d;1", event_number, island_number);
	TH1 *thick_right_fast_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *thick_right_fast_pulse[max_pulses];
	AlCapCanvas->cd(3);
	if (thick_right_fast_island)
		thick_right_fast_island->Draw();
	
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiR2Fast_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		thick_right_fast_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (thick_right_fast_pulse[pulse_number-1]) {
			thick_right_fast_pulse[pulse_number-1]->SetLineColor(kMagenta);
			thick_right_fast_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	
	//////////////////
	// SiR2 Slow
	sprintf(islandHistTitle,"hSiR2Slow_Event%d_Island%d;1", event_number, island_number);
	TH1 *thick_right_slow_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *thick_right_slow_pulse[max_pulses];
	AlCapCanvas->cd(4);
	if (thick_right_slow_island)
		thick_right_slow_island->Draw();
	
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiR2Slow_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		thick_right_slow_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (thick_right_slow_pulse[pulse_number-1]) {
			thick_right_slow_pulse[pulse_number-1]->SetLineColor(kMagenta);
			thick_right_slow_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}	
}
