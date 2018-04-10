void CorrectedSpectrum() {

  //////////////////////////////
  // User parameters
  std::string filename = "~/data/results/SiL3_active/EventSelection.root";
  std::string lifetime_filename = "~/data/results/SiL3_active/LifetimeFit.root";
  std::string mc_filename = "~/data/mc/SiL3_active/corrections.root";
  std::string outfilename = "~/data/results/SiL3_active/corrected-spectrum.root";


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
  
  double capture_fraction = 0.658;
  int n_captured_muons = capture_fraction*n_stopped_muons;


  TFile* lifetime_file = new TFile(lifetime_filename.c_str(), "READ");
  TF1* lifetime_fit = (TF1*) lifetime_file->Get("lifetime_fit");
  
  TCanvas* c_time_slice = new TCanvas("c_time_slice", "c_time_slice");
  c_time_slice->SetLogy();

  TLegend* leg = new TLegend(0.64,0.54,0.79,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  TFile* mc_file = new TFile(mc_filename.c_str(), "READ");
  TH1D* hDecayElectronEDep = (TH1D*) mc_file->Get("hEDep_muplus");

  
  double min_time = 1000;
  double max_time = 4000;
  const int n_slices = 6;
  double time_slice = (max_time - min_time) / n_slices;
  Int_t colours[n_slices] = {kBlue, kRed, kBlack, kMagenta, kSpring, kGray};
  std::stringstream histname, leglabel;
  int i_slice = 0;
  TH1D* hTimeSlices[n_slices];
  TH1D* hDecayElectronCorrections[n_slices];
  double overall_max_bin_content = 0;
  int rebin_factor = 2;
  for (double i_min_time = min_time; i_min_time < max_time; i_min_time += time_slice) {
    double i_max_time = i_min_time + time_slice;
    int i_min_time_bin = hTimeEnergy->GetXaxis()->FindBin(i_min_time);
    int i_max_time_bin = hTimeEnergy->GetXaxis()->FindBin(i_max_time);
    int i_colour = colours[i_slice];

    double fraction_muons_captured_in_time_slice = (lifetime_fit->Eval(i_min_time) - lifetime_fit->Eval(i_max_time)) / lifetime_fit->GetParameter(0);
    double n_captured_muons_time_slice = fraction_muons_captured_in_time_slice*n_captured_muons;
    std::cout << "AE: " << i_min_time << " -- " << i_max_time << " ns = " << lifetime_fit->Eval(i_max_time) << ", " << lifetime_fit->Eval(i_min_time)  << std::endl;
    std::cout << "AE: " << i_min_time << " -- " << i_max_time << " ns = " << n_captured_muons_time_slice << std::endl;
    histname.str("");
    histname << "hTimeSlice_" << i_min_time;
    TH1D* hEnergy = hTimeEnergy->ProjectionY(histname.str().c_str(), i_min_time_bin, i_max_time_bin);
    hEnergy->Sumw2();
    hEnergy->SetStats(false);
    hEnergy->SetLineColor(i_colour);
    hEnergy->Rebin(rebin_factor);
    hEnergy->GetXaxis()->SetRangeUser(0, 15000);
    hEnergy->SetYTitle("Counts / Captured Muon / keV");
    hTimeSlices[i_slice] = hEnergy;

    histname << "_DecayCorr";
    hDecayElectronCorrections[i_slice] = (TH1D*) hDecayElectronEDep->Clone(histname.str().c_str());
    hDecayElectronCorrections[i_slice]->Scale( (1 - capture_fraction)*fraction_muons_captured_in_time_slice*n_stopped_muons );
    hDecayElectronCorrections[i_slice]->Rebin(rebin_factor);
    //    hDecayElectronCorrections[i_slice]->Draw("HIST E SAME");

    hTimeSlices[i_slice]->Add(hDecayElectronCorrections[i_slice], -1);
    hTimeSlices[i_slice]->Scale(1.0 / n_captured_muons_time_slice);
    hTimeSlices[i_slice]->Scale(1.0 / hTimeSlices[i_slice]->GetXaxis()->GetBinWidth(1));
    hTimeSlices[i_slice]->Draw("HIST E SAME");
    
    leglabel.str("");
    leglabel << i_min_time << " -- " << i_max_time << " ns";
    leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");

    if (hEnergy->GetMaximum() > overall_max_bin_content) {
      overall_max_bin_content = hEnergy->GetMaximum();
    }

    ++i_slice;
  }

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    hTimeSlices[i_slice]->SetMaximum(overall_max_bin_content*1.5);
    hTimeSlices[i_slice]->Write();
  }
  outfile->Write();
  outfile->Close();

  leg->Draw();
}
