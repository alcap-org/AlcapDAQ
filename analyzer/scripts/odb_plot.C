{
  gROOT->ProcessLine(".L dataset_bg_draw.C+");

  TFile* ifile = new TFile("odbtrends.root", "READ");
  TFile* ofile = new TFile("odbcanvases.root", "RECREATE");
  TTree* trodb = (TTree*)ifile->Get("odbtrend");
  const int n = trodb->GetEntries();
  TCanvas* can = new TCanvas("c1", "Canvas");
  TLegend *leg, *bgleg;
  TMultiGraph* bg = dataset_bg_draw(bgleg); 

  // General plot variables
  TMultiGraph* mgr; TGraph* gr; TH2* h2;

  // Boolean plot setup
  Int_t run;
  trodb->SetBranchAddress("Run", &run);
  const Int_t rmin = trodb->GetMinimum("Run"), rmax = trodb->GetMaximum("Run");
  TPolyMarker* on  = new TPolyMarker();
  TPolyMarker* off = new TPolyMarker();
  on ->SetMarkerColor(3);
  on ->SetMarkerStyle(20);
  off->SetMarkerColor(2);
  off->SetMarkerStyle(3);


  /****************************************************************************/
  /* Channel Enabled */
  /****************************************************************************/
  // C1 SIS3300 (6*8)
  const int ngraphs = 6*8;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B1C1_Enabled", "C1_SIS3300_B1C2_Enabled",
    "C1_SIS3300_B1C3_Enabled", "C1_SIS3300_B1C4_Enabled",
    "C1_SIS3300_B1C5_Enabled", "C1_SIS3300_B1C6_Enabled",
    "C1_SIS3300_B1C7_Enabled", "C1_SIS3300_B1C8_Enabled",
    "C1_SIS3300_B2C1_Enabled", "C1_SIS3300_B2C2_Enabled",
    "C1_SIS3300_B2C3_Enabled", "C1_SIS3300_B2C4_Enabled",
    "C1_SIS3300_B2C5_Enabled", "C1_SIS3300_B2C6_Enabled",
    "C1_SIS3300_B2C7_Enabled", "C1_SIS3300_B2C8_Enabled",
    "C1_SIS3300_B3C1_Enabled", "C1_SIS3300_B3C2_Enabled",
    "C1_SIS3300_B3C3_Enabled", "C1_SIS3300_B3C4_Enabled",
    "C1_SIS3300_B3C5_Enabled", "C1_SIS3300_B3C6_Enabled",
    "C1_SIS3300_B3C7_Enabled", "C1_SIS3300_B3C8_Enabled",
    "C1_SIS3300_B4C1_Enabled", "C1_SIS3300_B4C2_Enabled",
    "C1_SIS3300_B4C3_Enabled", "C1_SIS3300_B4C4_Enabled",
    "C1_SIS3300_B4C5_Enabled", "C1_SIS3300_B4C6_Enabled",
    "C1_SIS3300_B4C7_Enabled", "C1_SIS3300_B4C8_Enabled",
    "C1_SIS3300_B5C1_Enabled", "C1_SIS3300_B5C2_Enabled",
    "C1_SIS3300_B5C3_Enabled", "C1_SIS3300_B5C4_Enabled",
    "C1_SIS3300_B5C5_Enabled", "C1_SIS3300_B5C6_Enabled",
    "C1_SIS3300_B5C7_Enabled", "C1_SIS3300_B5C8_Enabled",
    "C1_SIS3300_B6C1_Enabled", "C1_SIS3300_B6C2_Enabled",
    "C1_SIS3300_B6C3_Enabled", "C1_SIS3300_B6C4_Enabled",
    "C1_SIS3300_B6C5_Enabled", "C1_SIS3300_B6C6_Enabled",
    "C1_SIS3300_B6C7_Enabled", "C1_SIS3300_B6C8_Enabled" };
  const char* labels[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				  "B1Ch5", "B1Ch6", "B1Ch7", "B1Ch8",
				  "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4",
				  "B2Ch5", "B2Ch6", "B2Ch7", "B2Ch8",
				  "B3Ch1", "B3Ch2", "B3Ch3", "B3Ch4",
				  "B3Ch5", "B3Ch6", "B3Ch7", "B3Ch8",
				  "B4Ch1", "B4Ch2", "B4Ch3", "B4Ch4",
				  "B4Ch5", "B4Ch6", "B4Ch7", "B4Ch8",
				  "B5Ch1", "B5Ch2", "B5Ch3", "B5Ch4",
				  "B5Ch5", "B5Ch6", "B5Ch7", "B5Ch8",
				  "B6Ch1", "B6Ch2", "B6Ch3", "B6Ch4",
				  "B6Ch5", "B6Ch6", "B6Ch7", "B6Ch8" };
  h2 = new TH2C("C1SIS3300Enabled", "Crate 1 SIS3300s Enabled Channels;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C1SIS3300ChEn");

  // C1 SIS3350 (2*4)
  const int ngraphs = 2*4;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1_Enabled", "C1_SIS3350_B1C2_Enabled",
    "C1_SIS3350_B1C3_Enabled", "C1_SIS3350_B1C4_Enabled",
    "C1_SIS3350_B2C1_Enabled", "C1_SIS3350_B2C2_Enabled",
    "C1_SIS3350_B2C3_Enabled", "C1_SIS3350_B2C4_Enabled" };
  const char* labels[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				  "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4" };
  h2 = new TH2C("C1SIS3350Enabled", "Crate 1 SIS3350s Enabled Channels;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C1SIS3350ChEn");

  // C4 (8+16)
  const int ngraphs = 8+16;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C4_WFD_Ch0_Enabled", "C4_WFD_Ch1_Enabled",
    "C4_WFD_Ch2_Enabled", "C4_WFD_Ch3_Enabled",
    "C4_WFD_Ch4_Enabled", "C4_WFD_Ch5_Enabled",
    "C4_WFD_Ch6_Enabled", "C4_WFD_Ch7_Enabled",
    "C4_TDC_Chh0Enable",  "C4_TDC_Chh1Enable",
    "C4_TDC_Chh2Enable",  "C4_TDC_Chh3Enable",
    "C4_TDC_Chh4Enable",  "C4_TDC_Chh5Enable",
    "C4_TDC_Chh6Enable",  "C4_TDC_Chh7Enable",
    "C4_TDC_Chh8Enable",  "C4_TDC_Chh9Enable",
    "C4_TDC_Chh10Enable", "C4_TDC_Chh11Enable",
    "C4_TDC_Chh12Enable", "C4_TDC_Chh13Enable",
    "C4_TDC_Chh14Enable", "C4_TDC_Chh15Enable" };
  const char* labels[ngraphs] = { "WFD0",  "WFD1",  "WFD2",  "WFD3",
				  "WFD4",  "WFD5",  "WFD6",  "WFD7",
				  "TDC0",  "TDC1",  "TDC2",  "TDC3",
				  "TDC4",  "TDC5",  "TDC6",  "TDC7",
				  "TDC8",  "TDC9",  "TDC10", "TDC11",
				  "TDC12", "TDC13", "TDC14", "TDC15" };
  h2 = new TH2C("C4Enabled", "Crate 4 Enabled Channels;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C4ChEn");

  // C5 (4)
  const int ngraphs = 4;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C5_Ch0_Enabled", "C5_Ch1_Enabled",
    "C5_Ch2_Enabled", "C5_Ch3_Enabled" };
  const char* labels[ngraphs] = { "Ch0",  "Ch1",  "Ch2",  "Ch3" };
  h2 = new TH2C("C5Enabled", "Crate 5 Enabled Channels;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C5ChEn");

  // C7 (8)
  const int ngraphs = 8;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C7_Ch0_Enabled", "C7_Ch1_Enabled",
    "C7_Ch2_Enabled", "C7_Ch3_Enabled",
    "C7_Ch4_Enabled", "C7_Ch5_Enabled",
    "C7_Ch6_Enabled", "C7_Ch7_Enabled"};
  const char* labels[ngraphs] = { "Ch0",  "Ch1",  "Ch2",  "Ch3",
				  "Ch4",  "Ch5",  "Ch6",  "Ch7" };
  h2 = new TH2C("C7Enabled", "Crate 7 Enabled Channels;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C7ChEn");


  /****************************************************************************/
  /* C1 Trigger Logic */
  /****************************************************************************/

  // C1 SIS3300 Threshold Logic
  const int ngraphs = 6*8;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B1C1_ThreshLogic", "C1_SIS3300_B1C2_ThreshLogic",
    "C1_SIS3300_B1C3_ThreshLogic", "C1_SIS3300_B1C4_ThreshLogic",
    "C1_SIS3300_B1C5_ThreshLogic", "C1_SIS3300_B1C6_ThreshLogic",
    "C1_SIS3300_B1C7_ThreshLogic", "C1_SIS3300_B1C8_ThreshLogic",
    "C1_SIS3300_B2C1_ThreshLogic", "C1_SIS3300_B2C2_ThreshLogic",
    "C1_SIS3300_B2C3_ThreshLogic", "C1_SIS3300_B2C4_ThreshLogic",
    "C1_SIS3300_B2C5_ThreshLogic", "C1_SIS3300_B2C6_ThreshLogic",
    "C1_SIS3300_B2C7_ThreshLogic", "C1_SIS3300_B2C8_ThreshLogic",
    "C1_SIS3300_B3C1_ThreshLogic", "C1_SIS3300_B3C2_ThreshLogic",
    "C1_SIS3300_B3C3_ThreshLogic", "C1_SIS3300_B3C4_ThreshLogic",
    "C1_SIS3300_B3C5_ThreshLogic", "C1_SIS3300_B3C6_ThreshLogic",
    "C1_SIS3300_B3C7_ThreshLogic", "C1_SIS3300_B3C8_ThreshLogic",
    "C1_SIS3300_B4C1_ThreshLogic", "C1_SIS3300_B4C2_ThreshLogic",
    "C1_SIS3300_B4C3_ThreshLogic", "C1_SIS3300_B4C4_ThreshLogic",
    "C1_SIS3300_B4C5_ThreshLogic", "C1_SIS3300_B4C6_ThreshLogic",
    "C1_SIS3300_B4C7_ThreshLogic", "C1_SIS3300_B4C8_ThreshLogic",
    "C1_SIS3300_B5C1_ThreshLogic", "C1_SIS3300_B5C2_ThreshLogic",
    "C1_SIS3300_B5C3_ThreshLogic", "C1_SIS3300_B5C4_ThreshLogic",
    "C1_SIS3300_B5C5_ThreshLogic", "C1_SIS3300_B5C6_ThreshLogic",
    "C1_SIS3300_B5C7_ThreshLogic", "C1_SIS3300_B5C8_ThreshLogic",
    "C1_SIS3300_B6C1_ThreshLogic", "C1_SIS3300_B6C2_ThreshLogic",
    "C1_SIS3300_B6C3_ThreshLogic", "C1_SIS3300_B6C4_ThreshLogic",
    "C1_SIS3300_B6C5_ThreshLogic", "C1_SIS3300_B6C6_ThreshLogic",
    "C1_SIS3300_B6C7_ThreshLogic", "C1_SIS3300_B6C8_ThreshLogic" };
  const char* labels[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				  "B1Ch5", "B1Ch6", "B1Ch7", "B1Ch8",
				  "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4",
				  "B2Ch5", "B2Ch6", "B2Ch7", "B2Ch8",
				  "B3Ch1", "B3Ch2", "B3Ch3", "B3Ch4",
				  "B3Ch5", "B3Ch6", "B3Ch7", "B3Ch8",
				  "B4Ch1", "B4Ch2", "B4Ch3", "B4Ch4",
				  "B4Ch5", "B4Ch6", "B4Ch7", "B4Ch8",
				  "B5Ch1", "B5Ch2", "B5Ch3", "B5Ch4",
				  "B5Ch5", "B5Ch6", "B5Ch7", "B5Ch8",
				  "B6Ch1", "B6Ch2", "B6Ch3", "B6Ch4",
				  "B6Ch5", "B6Ch6", "B6Ch7", "B6Ch8" };
  h2 = new TH2C("C1SIS3300ThreshLogic", "Crate 1 SIS3300s Threshold Logic;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C1SIS3300ThreshLogic");

  // C1 SIS3350 Trigger Logic
  const int ngraphs = 2*4;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1_TrigLogic", "C1_SIS3350_B1C2_TrigLogic",
    "C1_SIS3350_B1C3_TrigLogic", "C1_SIS3350_B1C4_TrigLogic",
    "C1_SIS3350_B2C1_TrigLogic", "C1_SIS3350_B2C2_TrigLogic",
    "C1_SIS3350_B2C3_TrigLogic", "C1_SIS3350_B2C4_TrigLogic" };
  const char* labels[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				  "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4" };
  h2 = new TH2C("C1SIS3350TrigLogic", "Crate 1 SIS3350s Trigger Logic;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C1SIS3350TrigLogic");

  // C1 SIS3350 Trigger Enabled
  const int ngraphs = 2*4;
  on ->SetPolyMarker(0);
  off->SetPolyMarker(0);
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1_TrigEnabled", "C1_SIS3350_B1C2_TrigEnabled",
    "C1_SIS3350_B1C3_TrigEnabled", "C1_SIS3350_B1C4_TrigEnabled",
    "C1_SIS3350_B2C1_TrigEnabled", "C1_SIS3350_B2C2_TrigEnabled",
    "C1_SIS3350_B2C3_TrigEnabled", "C1_SIS3350_B2C4_TrigEnabled" };
  const char* labels[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				  "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4" };
  h2 = new TH2C("C1SIS3350TrigEnabled", "Crate 1 SIS3350s Trigger Enabled;Run",
		rmax-rmin+1, rmin-0.5, rmax+0.5, ngraphs, 0., ngraphs);
  h2->SetStats(0);
  for (int i = 0; i < ngraphs; ++i)
    h2->GetYaxis()->SetBinLabel(i+1, labels[i]);
  Int_t tradd[ngraphs];
  for (int i = 0; i < ngraphs; ++i)
    trodb->SetBranchAddress(grnames[i], &tradd[i]);
  for (int i = 0; i < n; ++i) {
    trodb->GetEntry(i);
    for (int j = 0; j < ngraphs; ++j)
      if (tradd[j])
	on->SetNextPoint(run, j+0.5);
      else
	off->SetNextPoint(run, j+0.5);
  }
  h2 ->Draw();
  bg->Draw();
  bgleg->Draw("F");
  on ->Draw();
  off->Draw();
  can->Write("C1SIS3350TrigEnabled");
  
  /****************************************************************************/
  /* C1B1 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Board 1 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B1C1_Threshhold:Run", "C1_SIS3300_B1C2_Threshhold:Run",
    "C1_SIS3300_B1C3_Threshhold:Run", "C1_SIS3300_B1C4_Threshhold:Run",
    "C1_SIS3300_B1C5_Threshhold:Run", "C1_SIS3300_B1C6_Threshhold:Run",
    "C1_SIS3300_B1C7_Threshhold:Run", "C1_SIS3300_B1C8_Threshhold:Run" };
  const char* leglabs[ngraphs] = { "Ch1", "Ch2", "Ch3", "Ch4",
				   "Ch5", "Ch6", "Ch7", "Ch8" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B1Thresholds");

  
  /****************************************************************************/
  /* C1B2 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Board 2 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B2C1_Threshhold:Run", "C1_SIS3300_B2C2_Threshhold:Run",
    "C1_SIS3300_B2C3_Threshhold:Run", "C1_SIS3300_B2C4_Threshhold:Run",
    "C1_SIS3300_B2C5_Threshhold:Run", "C1_SIS3300_B2C6_Threshhold:Run",
    "C1_SIS3300_B2C7_Threshhold:Run", "C1_SIS3300_B2C8_Threshhold:Run" };
  const char* leglabs[ngraphs] = { "Ch1", "Ch2", "Ch3", "Ch4",
				   "Ch5", "Ch6", "Ch7", "Ch8" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B2Thresholds");

  
  /****************************************************************************/
  /* C1B3 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Board 3 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B3C1_Threshhold:Run", "C1_SIS3300_B3C2_Threshhold:Run",
    "C1_SIS3300_B3C3_Threshhold:Run", "C1_SIS3300_B3C4_Threshhold:Run",
    "C1_SIS3300_B3C5_Threshhold:Run", "C1_SIS3300_B3C6_Threshhold:Run",
    "C1_SIS3300_B3C7_Threshhold:Run", "C1_SIS3300_B3C8_Threshhold:Run" };
  const char* leglabs[ngraphs] = { "Ch1", "Ch2", "Ch3", "Ch4",
				   "Ch5", "Ch6", "Ch7", "Ch8" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B3Thresholds");

  
  /****************************************************************************/
  /* C1B4 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Board 4 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B4C1_Threshhold:Run", "C1_SIS3300_B4C2_Threshhold:Run",
    "C1_SIS3300_B4C3_Threshhold:Run", "C1_SIS3300_B4C4_Threshhold:Run",
    "C1_SIS3300_B4C5_Threshhold:Run", "C1_SIS3300_B4C6_Threshhold:Run",
    "C1_SIS3300_B4C7_Threshhold:Run", "C1_SIS3300_B4C8_Threshhold:Run" };
  const char* leglabs[ngraphs] = { "Ch1", "Ch2", "Ch3", "Ch4",
				   "Ch5", "Ch6", "Ch7", "Ch8" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B4Thresholds");

  
  /****************************************************************************/
  /* C1B5 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Board 5 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B5C1_Threshhold:Run", "C1_SIS3300_B5C2_Threshhold:Run",
    "C1_SIS3300_B5C3_Threshhold:Run", "C1_SIS3300_B5C4_Threshhold:Run",
    "C1_SIS3300_B5C5_Threshhold:Run", "C1_SIS3300_B5C6_Threshhold:Run",
    "C1_SIS3300_B5C7_Threshhold:Run", "C1_SIS3300_B5C8_Threshhold:Run" };
  const char* leglabs[ngraphs] = { "Ch1", "Ch2", "Ch3", "Ch4",
				   "Ch5", "Ch6", "Ch7", "Ch8" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B5Thresholds");


  /****************************************************************************/
  /* C1B6 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Board 6 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B6C1_Threshhold:Run", "C1_SIS3300_B6C2_Threshhold:Run",
    "C1_SIS3300_B6C3_Threshhold:Run", "C1_SIS3300_B6C4_Threshhold:Run",
    "C1_SIS3300_B6C5_Threshhold:Run", "C1_SIS3300_B6C6_Threshhold:Run",
    "C1_SIS3300_B6C7_Threshhold:Run", "C1_SIS3300_B6C8_Threshhold:Run" };
  const char* leglabs[ngraphs] = { "Ch1", "Ch2", "Ch3", "Ch4",
				   "Ch5", "Ch6", "Ch7", "Ch8" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B6Thresholds");


  /****************************************************************************/
  /* C1B7 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3350 Board 1 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1_TrigThreshOn:Run", "C1_SIS3350_B1C1_TrigThreshOff:Run",
    "C1_SIS3350_B1C2_TrigThreshOn:Run", "C1_SIS3350_B1C2_TrigThreshOff:Run",
    "C1_SIS3350_B1C3_TrigThreshOn:Run", "C1_SIS3350_B1C3_TrigThreshOff:Run",
    "C1_SIS3350_B1C4_TrigThreshOn:Run", "C1_SIS3350_B1C4_TrigThreshOff:Run" };
  const char* leglabs[ngraphs] = { "Ch1 On", "Ch1 Off", "Ch2 On", "Ch2 Off",
				   "Ch3 On", "Ch3 Off", "Ch4 On", "Ch4 Off" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B7Thresholds");


  /****************************************************************************/
  /* C1B8 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3350 Board 2 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B2C1_TrigThreshOn:Run", "C1_SIS3350_B2C1_TrigThreshOff:Run",
    "C1_SIS3350_B2C2_TrigThreshOn:Run", "C1_SIS3350_B2C2_TrigThreshOff:Run",
    "C1_SIS3350_B2C3_TrigThreshOn:Run", "C1_SIS3350_B2C3_TrigThreshOff:Run",
    "C1_SIS3350_B2C4_TrigThreshOn:Run", "C1_SIS3350_B2C4_TrigThreshOff:Run" };
  const char* leglabs[ngraphs] = { "Ch1 On", "Ch1 Off", "Ch2 On", "Ch2 Off",
				   "Ch3 On", "Ch3 Off", "Ch4 On", "Ch4 Off" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1B8Thresholds");


  /****************************************************************************/
  /* C1 Averaging */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Averaging;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 6;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B1_Averaging:Run", "C1_SIS3300_B2_Averaging:Run",
    "C1_SIS3300_B3_Averaging:Run", "C1_SIS3300_B4_Averaging:Run",
    "C1_SIS3300_B5_Averaging:Run", "C1_SIS3300_B6_Averaging:Run" };
  const char* leglabs[ngraphs] = { "B1", "B2", "B3", "B4", "B5", "B6" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1SIS3300Averaging");


  /****************************************************************************/
  /* C1 Stop Delay */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3300 Stop Delay;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 6;
  const char* grnames[ngraphs] = {
    "C1_SIS3300_B1_StopDelay:Run", "C1_SIS3300_B2_StopDelay:Run",
    "C1_SIS3300_B3_StopDelay:Run", "C1_SIS3300_B4_StopDelay:Run",
    "C1_SIS3300_B5_StopDelay:Run", "C1_SIS3300_B6_StopDelay:Run" };
  const char* leglabs[ngraphs] = { "B1", "B2", "B3", "B4", "B5", "B6" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1SIS3300StopDelay");


  /****************************************************************************/
  /* C1 Offset */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3350 Offsets;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1_Offset:Run", "C1_SIS3350_B1C2_Offset:Run",
    "C1_SIS3350_B1C3_Offset:Run", "C1_SIS3350_B1C4_Offset:Run",
    "C1_SIS3350_B2C1_Offset:Run", "C1_SIS3350_B2C2_Offset:Run",
    "C1_SIS3350_B2C3_Offset:Run", "C1_SIS3350_B2C4_Offset:Run" };
  const char* leglabs[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				   "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1SIS3350Offset");


  /****************************************************************************/
  /* C1 VGA */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3350 VGA;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);

  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1_VGA:Run", "C1_SIS3350_B1C2_VGA:Run",
    "C1_SIS3350_B1C3_VGA:Run", "C1_SIS3350_B1C4_VGA:Run",
    "C1_SIS3350_B2C1_VGA:Run", "C1_SIS3350_B2C2_VGA:Run",
    "C1_SIS3350_B2C3_VGA:Run", "C1_SIS3350_B2C4_VGA:Run" };
  const char* leglabs[ngraphs] = { "B1Ch1", "B1Ch2", "B1Ch3", "B1Ch4",
				   "B2Ch1", "B2Ch2", "B2Ch3", "B2Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1SIS3350VGA");

  /****************************************************************************/
  /* C1 PreSamps */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3350 Presamples;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 4;
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1C2_PreSamps:Run", "C1_SIS3350_B1C3C4_PreSamps:Run",
    "C1_SIS3350_B2C1C2_PreSamps:Run", "C1_SIS3350_B2C3C4_PreSamps:Run" };
  const char* leglabs[ngraphs] = { "B1Ch1+2", "B1Ch3+4", "B2Ch1+2", "B2Ch3+4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1SIS3350PreSamples");


  /****************************************************************************/
  /* C1 WF Length */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 1 SIS3350 WF Length;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 4;
  const char* grnames[ngraphs] = {
    "C1_SIS3350_B1C1C2_WFLen:Run", "C1_SIS3350_B1C3C4_WFLen:Run",
    "C1_SIS3350_B2C1C2_WFLen:Run", "C1_SIS3350_B2C3C4_WFLen:Run" };
  const char* leglabs[ngraphs] = { "B1Ch1+2", "B1Ch3+4", "B2Ch1+2", "B2Ch3+4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i/2+1);
    gr->SetMarkerStyle(2*(1+i%2));
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C1SIS3350WFLen");


  /****************************************************************************/
  /* C4B1 Thresholds */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 4 V1724 Thresholds;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);

  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C4_WFD_Ch0_TrigThresh:Run", "C4_WFD_Ch1_TrigThresh:Run",
    "C4_WFD_Ch2_TrigThresh:Run", "C4_WFD_Ch3_TrigThresh:Run",
    "C4_WFD_Ch4_TrigThresh:Run", "C4_WFD_Ch5_TrigThresh:Run",
    "C4_WFD_Ch6_TrigThresh:Run", "C4_WFD_Ch7_TrigThresh:Run" };
  const char* leglabs[ngraphs] = { "Ch0", "Ch1", "Ch2", "Ch3",
				   "Ch4", "Ch5", "Ch6", "Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C4WFDThresholds");
  

  /****************************************************************************/
  /* C7 Offset */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 7 DT5730 Ped Offset STDRun");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C7_Ch0_OffsetSTD:Run", "C7_Ch1_OffsetSTD:Run",
    "C7_Ch2_OffsetSTD:Run", "C7_Ch3_OffsetSTD:Run",
    "C7_Ch4_OffsetSTD:Run", "C7_Ch5_OffsetSTD:Run",
    "C7_Ch6_OffsetSTD:Run", "C7_Ch7_OffsetSTD:Run" };
  const char* leglabs[ngraphs] = { "Ch0", "Ch1", "Ch2", "Ch3",
				   "Ch4", "Ch5", "Ch6", "Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C7PedOffset");

  
  /****************************************************************************/
  /* C7 PedAvg */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 7 DT5730 Ped Avg NSamps;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C7_Ch0_PedestalAvgDPP:Run", "C7_Ch1_PedestalAvgDPP:Run",
    "C7_Ch2_PedestalAvgDPP:Run", "C7_Ch3_PedestalAvgDPP:Run",
    "C7_Ch4_PedestalAvgDPP:Run", "C7_Ch5_PedestalAvgDPP:Run",
    "C7_Ch6_PedestalAvgDPP:Run", "C7_Ch7_PedestalAvgDPP:Run" };
  const char* leglabs[ngraphs] = { "Ch0", "Ch1", "Ch2", "Ch3",
				   "Ch4", "Ch5", "Ch6", "Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C7PedAvg");
  

  /****************************************************************************/
  /* C7 PreTrig */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 7 DT5730 PreTrig NSamps;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C7_Ch0_PreTrigSizeDPP:Run", "C7_Ch1_PreTrigSizeDPP:Run",
    "C7_Ch2_PreTrigSizeDPP:Run", "C7_Ch3_PreTrigSizeDPP:Run",
    "C7_Ch4_PreTrigSizeDPP:Run", "C7_Ch5_PreTrigSizeDPP:Run",
    "C7_Ch6_PreTrigSizeDPP:Run", "C7_Ch7_PreTrigSizeDPP:Run" };
  const char* leglabs[ngraphs] = { "Ch0", "Ch1", "Ch2", "Ch3",
				   "Ch4", "Ch5", "Ch6", "Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C7PreTrig");
  

  /****************************************************************************/
  /* C7 Threshold*/
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 7 DT5730 Threshold;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C7_Ch0_SelfTrigThreshDPP:Run", "C7_Ch1_SelfTrigThreshDPP:Run",
    "C7_Ch2_SelfTrigThreshDPP:Run", "C7_Ch3_SelfTrigThreshDPP:Run",
    "C7_Ch4_SelfTrigThreshDPP:Run", "C7_Ch5_SelfTrigThreshDPP:Run",
    "C7_Ch6_SelfTrigThreshDPP:Run", "C7_Ch7_SelfTrigThreshDPP:Run" };
  const char* leglabs[ngraphs] = { "Ch0", "Ch1", "Ch2", "Ch3",
				   "Ch4", "Ch5", "Ch6", "Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  ;
  leg->Draw();
  can->Write("C7Thresh");


  /****************************************************************************/
  /* C7 WFLen */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate 7 DT5730 WF Length;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);

  const int ngraphs = 8;
  const char* grnames[ngraphs] = {
    "C7_Ch0_WFLenDPP:Run", "C7_Ch1_WFLenDPP:Run",
    "C7_Ch2_WFLenDPP:Run", "C7_Ch3_WFLenDPP:Run",
    "C7_Ch4_WFLenDPP:Run", "C7_Ch5_WFLenDPP:Run",
    "C7_Ch6_WFLenDPP:Run", "C7_Ch7_WFLenDPP:Run" };
  const char* leglabs[ngraphs] = { "Ch0", "Ch1", "Ch2", "Ch3",
				   "Ch4", "Ch5", "Ch6", "Ch4" };
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  leg->Draw();
  can->Write("C7WFLen");
  

  /****************************************************************************/
  /* CrateEnabled */
  /****************************************************************************/
  mgr = new TMultiGraph("graphs", "Crate Enabled;Run");
  leg = new TLegend(0.9, 0.7, 1.0, 0.9);
  
  const int ngraphs = 4;
  const char* grnames[ngraphs] = { "C1_Enabled:Run", "C4_Enabled:Run",
				   "C5_Enabled:Run", "C7_Enabled:Run"};
  const char* leglabs[ngraphs] = { "SIS", "V1724/V1290", "DT5720", "DT5730"};
  for (int i = 0; i < ngraphs; ++i) {
    trodb->Draw(grnames[i], "", "goff");
    gr = new TGraph(n, trodb->GetV2(), trodb->GetV1());
    gr->SetMarkerColor(i+1);
    gr->SetMarkerStyle(i+1);
    gr->SetFillStyle(0);
    leg->AddEntry(gr, leglabs[i], "P");
    mgr->Add(gr);
  }
  mgr->Draw("AP");
  bg->Draw();
  bgleg->Draw("F");
  leg->Draw();
  can->Write("CrateEnabled");
  


  
  
  ofile->Close();
}
