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
	int event_number = 3;
	int island_number = 3;
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	//////////////////
	// Ge Fast
	sprintf(islandHistTitle,"hGeFastRaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *thick_fast_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *thick_fast_pulse[max_pulses];
	AlCapCanvas->cd(1);
	if (thick_fast_island)
		thick_fast_island->Draw();
		
	// loop through pulses and find them all
	int pulse_number = 1;
	bool final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hGeFastRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		thick_fast_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (thick_fast_pulse[pulse_number-1]) {
			thick_fast_pulse[pulse_number-1]->SetLineColor(kMagenta);
			thick_fast_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	//////////////////
	// Ge Slow
	sprintf(islandHistTitle,"hGeSlowRaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *thick_slow_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *thick_slow_pulse[max_pulses];
	AlCapCanvas->cd(2);
	if (thick_slow_island)
		thick_slow_island->Draw();
	
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"hGeSlowRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		thick_slow_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (thick_slow_pulse[pulse_number-1]) {
			thick_slow_pulse[pulse_number-1]->SetLineColor(kMagenta);
			thick_slow_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
}
