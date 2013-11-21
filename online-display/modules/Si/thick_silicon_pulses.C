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
	int max_event_number = 2000;
	int max_island_number = 100;
	
	int event_number = rand()%max_event_number;
	int island_number = rand()%max_island_number;		
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	TH1 *thick_left_fast_island = NULL;
	TH1 *thick_left_slow_island = NULL;
	TH1 *thick_right_fast_island = NULL;
	TH1 *thick_right_slow_island = NULL;
	
	// Find an event and island number where there are actually pulses
	bool island_found = false;
	while (island_found != true) {
		sprintf(islandHistTitle,"hSiL2FastRaw_Event%d_Island%d;1", event_number, island_number);
		thick_left_fast_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"hSiL2SlowRaw_Event%d_Island%d;1", event_number, island_number);
		thick_left_slow_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"hSiR2FastRaw_Event%d_Island%d;1", event_number, island_number);
		thick_right_fast_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"hSiR2SlowRaw_Event%d_Island%d;1", event_number, island_number);
		thick_right_slow_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		
		if (thick_left_fast_island || thick_left_slow_island || thick_right_fast_island || thick_right_slow_island )
			island_found = true; // found an island
		else {
			event_number = rand()%max_event_number;
			island_number = rand()%max_island_number;
		}
	}
	
	//////////////////
	// SiL2 Fast
	AlCapCanvas->cd(1);
	if (thick_left_fast_island)
		thick_left_fast_island->Draw();	
		
	// loop through pulses and find them all
	TH1 *thick_left_fast_pulse[max_pulses];
	int pulse_number = 1;
	bool final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiL2FastRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
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
	AlCapCanvas->cd(2);
	if (thick_left_slow_island)
		thick_left_slow_island->Draw();
	
	TH1 *thick_left_slow_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiL2SlowRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
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
	AlCapCanvas->cd(3);
	if (thick_right_fast_island)
		thick_right_fast_island->Draw();
		
	TH1 *thick_right_fast_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiR2FastRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
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
	AlCapCanvas->cd(4);
	if (thick_right_slow_island)
		thick_right_slow_island->Draw();
	
	TH1 *thick_right_slow_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hSiR2SlowRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
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
