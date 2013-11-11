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
	int event_number = 1;
	int island_number = 38;
	
	char islandHistTitle[1024];
	char pulseHistTitle[1024];
	
	//////////////////
	// muSc
	sprintf(islandHistTitle,"muScRaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *muSc_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *muSc_pulse[max_pulses];
	AlCapCanvas->cd(1);
	if (muSc_island)
		muSc_island->Draw();
		
	// loop through pulses and find them all
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
	sprintf(islandHistTitle,"muScARaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *muScA_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *muScA_pulse[max_pulses];
	AlCapCanvas->cd(2);
	if (muScA_island)
		muScA_island->Draw();
	
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
	sprintf(islandHistTitle,"ScLRaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *ScL_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *ScL_pulse[max_pulses];
	AlCapCanvas->cd(3);
	if (ScL_island)
		ScL_island->Draw();
	
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
	sprintf(islandHistTitle,"ScRRaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *ScR_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *ScR_pulse[max_pulses];
	AlCapCanvas->cd(4);
	if (ScR_island)
		ScR_island->Draw();
	
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
	sprintf(islandHistTitle,"ScGeRaw_Event%d_Island%d;1", event_number, island_number);
	TH1 *ScGe_island = (TH1 *)gDirectory->Get(islandHistTitle);
	TH1 *ScGe_pulse[max_pulses];
	AlCapCanvas->cd(4);
	if (ScGe_island)
		ScGe_island->Draw();
	
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
