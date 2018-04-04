void ActiveTargetAnalysis_TimeSlices() {
  
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_TwoRuns.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_test.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_noTemplates.root";
  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040_Range-0-100MeV_Binning-100keV_NewCalib.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  TH2F* hTimeEnergy = (TH2F*) file->Get("hTimeEnergy");
  TTree* cuttree = (TTree*) file->Get("cuts");

  int n_stopped_muons = 0;
  double min_muon_energy = 0;
  double max_muon_energy = 0;
  double muon_time_cut = 0;
  int i_stopped_muons = 0;
  cuttree->SetBranchAddress("n_stopped_muons", &i_stopped_muons);
  cuttree->SetBranchAddress("min_muon_energy", &min_muon_energy);
  cuttree->SetBranchAddress("max_muon_energy", &max_muon_energy);
  cuttree->SetBranchAddress("muon_time_cut", &muon_time_cut);
  for (int i_entry = 0; i_entry < cuttree->GetEntries(); ++i_entry) {
    cuttree->GetEntry(i_entry);
    n_stopped_muons += i_stopped_muons;
  }

  int rebin_factor = 1;
  
  double capture_fraction = 0.658;
  int n_captured_muons = capture_fraction*n_stopped_muons;

  TCanvas* c_time_slice = new TCanvas("c_time_slice", "c_time_slice");
  c_time_slice->Divide(2,1);
  c_time_slice->GetPad(1)->SetLogz();
  c_time_slice->GetPad(2)->SetLogy();
  
  c_time_slice->cd(1);
  hTimeEnergy->GetXaxis()->SetRangeUser(0,5000);
  hTimeEnergy->GetYaxis()->SetRangeUser(0,30000);
  hTimeEnergy->Draw("COLZ");

  double min_time = 1000;
  double max_time = 4000;
  const int n_slices = 6;
  double time_slice = (max_time - min_time) / n_slices;
  Int_t colours[n_slices] = {kBlue, kRed, kBlack, kMagenta, kSpring, kCyan};
  std::stringstream histname;
  int i_slice = 0;
  for (double i_min_time = min_time; i_min_time < max_time; i_min_time += time_slice) {
    //  double i_max_time = i_min_time + time_slice;
    double i_max_time = max_time;
    int i_min_time_bin = hTimeEnergy->GetXaxis()->FindBin(i_min_time);
    int i_max_time_bin = hTimeEnergy->GetXaxis()->FindBin(i_max_time);
    int i_colour = colours[i_slice];

    c_time_slice->cd(1);
    //    TLine* time_line_min = new TLine(i_min_time, hTimeEnergy->GetYaxis()->GetXmin(), i_min_time, hTimeEnergy->GetYaxis()->GetXmax());
    TLine* time_line_min = new TLine(i_min_time, hTimeEnergy->GetYaxis()->GetXmin(), i_min_time, 30000);
    time_line_min->SetLineWidth(2);
    time_line_min->SetLineColor(i_colour);
    time_line_min->Draw("LSAME");
    //    TLine* time_line_max = new TLine(i_max_time, hTimeEnergy->GetYaxis()->GetXmin(), i_max_time, hTimeEnergy->GetYaxis()->GetXmax());
    TLine* time_line_max = new TLine(i_max_time, hTimeEnergy->GetYaxis()->GetXmin(), i_max_time, 30000);
    time_line_max->SetLineWidth(2);
    time_line_max->SetLineColor(i_colour);
    time_line_max->Draw("LSAME");


    c_time_slice->cd(2);
    histname.str("");
    histname << "hTimeSlice_" << i_min_time;
    TH1D* hEnergy = hTimeEnergy->ProjectionY(histname.str().c_str(), i_min_time_bin, i_max_time_bin);
    hEnergy->SetLineColor(i_colour);
    hEnergy->Rebin(rebin_factor);
    hEnergy->Draw("HIST SAME");
    hEnergy->GetXaxis()->SetRangeUser(0, 15000);
    ++i_slice;
  }
}
