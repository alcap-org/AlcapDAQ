void TimeSlices() {

  //////////////////////////////
  // User parameters
  std::string filename = "~/data/results/SiL3_active/EventSelection.root";
  double min_energy_range = 0;
  double max_energy_range = 15000;
  
  
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* hTimeEnergy = (TH2F*) file->Get("hTimeEnergy");
  hTimeEnergy->GetXaxis()->SetRangeUser(-500, 6000);
  hTimeEnergy->GetYaxis()->SetRangeUser(min_energy_range, max_energy_range);

  TCanvas* c_time_slice = new TCanvas("c_time_slice", "c_time_slice");
  c_time_slice->SetLogz();

  hTimeEnergy->Draw("COLZ");
  
  double min_time = 1000;
  double max_time = 4000;
  const int n_slices = 6;
  double time_slice = (max_time - min_time) / n_slices;
  Int_t colours[n_slices] = {kBlue, kRed, kBlack, kMagenta, kSpring, kGray};
  int i_slice = 0;  
  for (double i_min_time = min_time; i_min_time < max_time; i_min_time += time_slice) {
    double i_max_time = i_min_time + time_slice;

    TLine* min_time_line = new TLine(i_min_time, min_energy_range, i_min_time, max_energy_range);
    min_time_line->SetLineWidth(2);
    min_time_line->SetLineColor(colours[i_slice]);
    min_time_line->Draw("LSAME");

    TLine* max_time_line = new TLine(i_max_time, min_energy_range, i_max_time, max_energy_range);
    max_time_line->SetLineWidth(2);
    max_time_line->SetLineColor(colours[i_slice]);
    max_time_line->Draw("LSAME");

    
    ++i_slice;
  }

}
