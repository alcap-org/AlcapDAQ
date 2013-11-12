void thick_scintillator_pulses()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,3);
  /*****************************************************************/
  
	const int max_pulses = 10;
	int max_event_number = 2000;
	int max_island_number = 100;
	
	int event_number = rand()%max_event_number;
	int island_number = rand()%max_island_number;
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	TH1 *muSc_island = NULL;
	TH1 *muScA_island = NULL;
	TH1 *ScL_island = NULL;
	TH1 *ScR_island = NULL;
	TH1 *ScGe_island = NULL;
	
	// Find an event and island number where there are actually pulses
	bool island_found = false;
	while (island_found != true) {
		sprintf(islandHistTitle,"muScRaw_Event%d_Island%d;1", event_number, island_number);
		muSc_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"muScARaw_Event%d_Island%d;1", event_number, island_number);
		muScA_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"ScLRaw_Event%d_Island%d;1", event_number, island_number);
		ScL_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"ScRRaw_Event%d_Island%d;1", event_number, island_number);
		ScR_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		sprintf(islandHistTitle,"ScGeRaw_Event%d_Island%d;1", event_number, island_number);
		ScGe_island = (TH1 *)gDirectory->Get(islandHistTitle);
		
		
		if (muSc_island || muScA_island || ScL_island || ScR_island || ScGe_island )
			island_found = true; // found an island
		else {
			event_number = rand()%max_event_number;
			island_number = rand()%max_island_number;
		}
	}
	
	//////////////////
	// muSc
	AlCapCanvas->cd(1);
	if (muSc_island)
		muSc_island->Draw();
		
	// loop through pulses and find them all
	TH1 *muSc_pulse[max_pulses];
	int pulse_number = 1;
	bool final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"muScRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		muSc_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (muSc_pulse[pulse_number-1]) {
			muSc_pulse[pulse_number-1]->SetLineColor(kMagenta);
			muSc_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	//////////////////
	// muScA
	AlCapCanvas->cd(2);
	if (muScA_island)
		muScA_island->Draw();
	
	TH1 *muScA_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"muScARaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		muScA_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (muScA_pulse[pulse_number-1]) {
			muScA_pulse[pulse_number-1]->SetLineColor(kMagenta);
			muScA_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	//////////////////
	// ScL
	AlCapCanvas->cd(3);
	if (ScL_island)
		ScL_island->Draw();
	
	TH1 *ScL_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"ScLRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		ScL_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (ScL_pulse[pulse_number-1]) {
			ScL_pulse[pulse_number-1]->SetLineColor(kMagenta);
			ScL_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	
	
	//////////////////
	// ScR
	AlCapCanvas->cd(4);
	if (ScR_island)
		ScR_island->Draw();
	
	TH1 *ScR_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"ScRRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		ScR_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (ScR_pulse[pulse_number-1]) {
			ScR_pulse[pulse_number-1]->SetLineColor(kMagenta);
			ScR_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}
	
	//////////////////
	// ScGe
	AlCapCanvas->cd(4);
	if (ScGe_island)
		ScGe_island->Draw();
	
	TH1 *ScGe_pulse[max_pulses];
	pulse_number = 1;
	final_pulse = false;
	
	while (final_pulse != true) {
		sprintf(pulseHistTitle,"ScGeRaw_Event%d_Island%d_Pulse%d;1", event_number, island_number, pulse_number);
		ScGe_pulse[pulse_number-1] = (TH1 *)gDirectory->Get(pulseHistTitle);
		
		if (ScGe_pulse[pulse_number-1]) {
			ScGe_pulse[pulse_number-1]->SetLineColor(kMagenta);
			ScGe_pulse[pulse_number-1]->Draw("SAME");
			pulse_number++;
		}
		else
			final_pulse = true;
	}	
}
