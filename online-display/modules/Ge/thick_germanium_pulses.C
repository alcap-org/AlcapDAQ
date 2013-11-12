void thick_germanium_pulses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(1,2);
  /*****************************************************************/
  
	const int max_pulses = 10;
	int max_event_number = 2000;
	int max_island_number = 100;
	
	int event_number = rand()%max_event_number;
	int island_number = rand()%max_island_number;
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	TH1 *ge_fast_island = NULL;
	TH1 *ge_slow_island = NULL;
	
	// Find an event and island number where there are actually pulses
	bool island_found = false;
	while (island_found != true) {
		sprintf(islandHistTitle,"hGeFastRaw_Event%d_Island%d;1", event_number, island_number);
		ge_fast_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"hGeSlowRaw_Event%d_Island%d;1", event_number, island_number);
		ge_slow_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		
		if (ge_fast_island || ge_slow_island)
			island_found = true; // found an island
		else {
			event_number = rand()%max_event_number;
			island_number = rand()%max_island_number;
		}
	}
	
	//////////////////
	// Ge Fast
	AlCapCanvas->cd(1);
	if (ge_fast_island)
		ge_fast_island->Draw();
		
	// loop through pulses and find them all
	TH1 *ge_fast_pulse[max_pulses];
	int pulse_number = 1;
	bool final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hGeFastRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		ge_fast_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (ge_fast_pulse[pulse_number-1]) {
			ge_fast_pulse[pulse_number-1]->SetLineColor(kMagenta);
			ge_fast_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	//////////////////
	// Ge Slow
	AlCapCanvas->cd(2);
	if (ge_slow_island)
		ge_slow_island->Draw();
	
	TH1 *ge_slow_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hGeSlowRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		ge_slow_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (ge_slow_pulse[pulse_number-1]) {
			ge_slow_pulse[pulse_number-1]->SetLineColor(kMagenta);
			ge_slow_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
}
