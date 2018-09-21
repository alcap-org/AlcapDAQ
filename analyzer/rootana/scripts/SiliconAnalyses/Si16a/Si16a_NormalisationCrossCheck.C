void Si16a_NormalisationCrossCheck() {

  //  const int n_runs = 8;
  //  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};
  //  double min_slice = 0;
  //  double max_slice = 2000;
  //  double slice_step = 400;
  //  const int n_slices = 4;//(max_slice - min_slice) / slice_step;

  const int n_runs = 1;
  int run_numbers[n_runs] = {9740};
  double min_slice = 500;
  double max_slice = 1500;
  double slice_step = 1000;
  const int n_slices = 1;//(max_slice - min_slice) / slice_step;

  std::stringstream run_str, muon_slice;
  double muon_slices[n_slices];
  double muon_slice_errors[n_slices];
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    muon_slices[i_slice] = min_slice + (i_slice+0.5)*slice_step;
    muon_slice_errors[i_slice] = slice_step/2;
  }
  
  const int n_dets = 2;
  std::string detnames[n_dets] = {"hTarget", "hThick"};
  std::string cutname[n_dets] = {"woutSiL3", "wSiL1"};
  std::string coincnames[n_dets] = {"wSiL1Coinc", "wSiL3Coinc"};

  double calc_effs[n_dets][n_runs][n_slices];
  double calc_eff_errs[n_dets][n_runs][n_slices];
  for (double min_muon_slice = min_slice; min_muon_slice < max_slice; min_muon_slice += slice_step) {
  
    double max_muon_slice = min_muon_slice+slice_step;
    int i_slice = (min_muon_slice - min_slice) / slice_step;
    for (int i_run = 0; i_run < n_runs; ++i_run) {

      run_str.str("");
      run_str << "0" << run_numbers[i_run];

      muon_slice.str("");
      muon_slice << "MuonSlice" << (int) min_muon_slice << "_" << (int) max_muon_slice;
      
      std::string infilename = "~/data/results/Si16a_useful/normalisation" + run_str.str() + ".root";
      TFile* file = new TFile(infilename.c_str(), "READ");

      for (int i_det = 0; i_det < n_dets; ++i_det) {
	//    std::string direct_count_histname = "hTarget_woutSiL3_MuonSlice700_1000";
	//    std::string direct_count_treename = "target_woutSiL3_count_MuonSlice700_1000";
	//    std::string indirect_count_treename = "xray_count_wSiL1Coinc_GeLoGain_MuonSlice700_1000";
	//    std::string wsname = "ws_xray_count_wSiL1Coinc_GeLoGain_MuonSlice700_1000";
	
	std::string direct_count_histname = detnames[i_det] + "_" + cutname[i_det] + "_" + muon_slice.str();
	std::string direct_count_treename = detnames[i_det] + "_" + cutname[i_det] + "_count_" + muon_slice.str();
	std::string indirect_count_treename = "hXRaySpectrum_count_" + coincnames[i_det] + "_GeLoGain_" + muon_slice.str();
	std::string wsname = "ws_hXRaySpectrum_count_" + coincnames[i_det] + "_GeLoGain_" + muon_slice.str();
	
    
	std::cout << "Trees:" << std::endl;
	std::cout << "\tDirect Count = " << direct_count_treename << std::endl;
	std::cout << "\tIndirect Count = " << indirect_count_treename << std::endl;
      
	TTree* direct_count_tree = (TTree*) file->Get(direct_count_treename.c_str());
	double direct_count = 0;
	double direct_count_error = 0;
	double min_muon_time = 0;
	double max_muon_time = 0;
	double min_muon_energy = 0;
	double max_muon_energy = 0;
	direct_count_tree->SetBranchAddress("n_stopped_muons", &direct_count);
	direct_count_tree->SetBranchAddress("n_stopped_muons_error", &direct_count_error);
	direct_count_tree->SetBranchAddress("min_muon_time", &min_muon_time);
	direct_count_tree->SetBranchAddress("max_muon_time", &max_muon_time);
	direct_count_tree->SetBranchAddress("min_muon_energy", &min_muon_energy);
	direct_count_tree->SetBranchAddress("max_muon_energy", &max_muon_energy);
	direct_count_tree->GetEntry(0);  
	std::cout << "Direct Count = " << direct_count << " +/- " << direct_count_error << " (" << direct_count - direct_count_error << " -- " << direct_count + direct_count_error << ")" << std::endl;
      
	TTree* indirect_count_tree = (TTree*) file->Get(indirect_count_treename.c_str());
	double indirect_count = 0;
	double indirect_count_error = 0;
	double xray_count = 0;
	double xray_count_error = 0;
	double xray_intensity = 0;
	double xray_intensity_error = 0;
	double xray_efficiency = 0;
	double xray_efficiency_error = 0;
	indirect_count_tree->SetBranchAddress("n_stopped_muons", &indirect_count);
	indirect_count_tree->SetBranchAddress("n_stopped_muons_error", &indirect_count_error);
	indirect_count_tree->SetBranchAddress("xray_count", &xray_count);
	indirect_count_tree->SetBranchAddress("xray_count_error", &xray_count_error);
	indirect_count_tree->SetBranchAddress("xray_intensity", &xray_intensity);
	indirect_count_tree->SetBranchAddress("xray_intensity_error", &xray_intensity_error);
	indirect_count_tree->SetBranchAddress("xray_efficiency", &xray_efficiency);
	indirect_count_tree->SetBranchAddress("xray_efficiency_error", &xray_efficiency_error);
	indirect_count_tree->GetEntry(0);
	std::cout << "Indirect Count = " << indirect_count << " +/- " << indirect_count_error << " (" << indirect_count - indirect_count_error << " -- " << indirect_count + indirect_count_error << ")" << std::endl;
	std::cout << "XRay Count = " << xray_count << " +/- " << xray_count_error << std::endl;
      
	double calculated_efficiency = (xray_count / (xray_intensity * direct_count));
	double calculated_efficiency_error = ( (xray_count_error*xray_count_error)/(xray_count*xray_count) ) + ( (xray_intensity_error*xray_intensity_error)/(xray_intensity*xray_intensity) ) + ( (direct_count_error*direct_count_error)/(direct_count*direct_count) );
	calculated_efficiency_error = calculated_efficiency*std::sqrt(calculated_efficiency_error);
	std::cout << "Original Eff = (" << xray_efficiency*1e4 << " +/- " << xray_efficiency_error*1e4 << ") x 10^{-4}" << std::endl;
	std::cout << "Calculated Eff = (" << calculated_efficiency*1e4 << " +/- " << calculated_efficiency_error*1e4 << ") x 10^{-4}" << std::endl;

	calc_effs[i_det][i_run][i_slice] = calculated_efficiency;
	calc_eff_errs[i_det][i_run][i_slice] = calculated_efficiency_error;
	
	///////////////////////////////////////////////////
	// Target Spectrum
	TCanvas* c_direct = new TCanvas("c_direct", "c_direct");
	TH1F* hTarget_Spectrum = (TH1F*) file->Get(direct_count_histname.c_str());
	hTarget_Spectrum->SetTitle("Target Spectrum");
	hTarget_Spectrum->SetStats(false);
	hTarget_Spectrum->Draw();
      
	std::stringstream latex_text;
	latex_text << "#splitline{" << min_muon_time << " < t < " << max_muon_time << " ns}{" << min_muon_energy << " < E < " << max_muon_energy << " keV}";
	TLatex* latex = new TLatex();
	latex->SetTextAlign(22);
	latex->DrawLatexNDC(0.5, 0.75, latex_text.str().c_str());
      
	TLine* min_muon_energy_line = new TLine(min_muon_energy, hTarget_Spectrum->GetMinimum(), min_muon_energy, hTarget_Spectrum->GetMaximum());
	min_muon_energy_line->SetLineWidth(2);
	min_muon_energy_line->Draw("LSAME");
      
	TLine* max_muon_energy_line = new TLine(max_muon_energy, hTarget_Spectrum->GetMinimum(), max_muon_energy, hTarget_Spectrum->GetMaximum());
	max_muon_energy_line->SetLineWidth(2);
	max_muon_energy_line->Draw("LSAME");
      
	latex_text.str("");
	latex_text << "N_{stop} = (" << std::fixed << std::setprecision(3) << direct_count/1e3 << " #pm " << direct_count_error/1e3 << ") x 10^{3}";
	latex->DrawLatexNDC(0.5, 0.4, latex_text.str().c_str());
      
      
	///////////////////////////////////////////////////
	// X-Ray Spectrum
	TCanvas* c_indirect = new TCanvas("c_indirect", "c_indirect");
	RooWorkspace* ws = (RooWorkspace*) file->Get(wsname.c_str());
	ws->Print();
      
	// Draw the fit
	RooPlot* Eframe = (ws->var("edep"))->frame();
	Eframe->SetTitle("X-Ray Spectrum");
	Eframe->GetXaxis()->SetTitle("Energy [keV]");
	//  Eframe->SetMinimum(3000);
	(ws->data("data"))->plotOn(Eframe);
	(ws->pdf("sum"))->plotOn(Eframe);
	Eframe->Draw();
      
	latex_text.str("");
	latex_text << "#splitline{  #varepsilon = (" << std::setprecision(2) << xray_efficiency*1e4 << " #pm " << xray_efficiency_error*1e4 << ") x 10^{-4}}{#rightarrow N_{stop} = (" << std::fixed << std::setprecision(1) << indirect_count/1e3 << " #pm " << indirect_count_error/1e3 << ") x 10^{3}}";
	latex->DrawLatexNDC(0.3, 0.75, latex_text.str().c_str());
      
	latex_text.str("");
	latex_text << "#splitline{  N_{x-ray} = " << std::fixed << std::setprecision(0) << xray_count << " #pm " << xray_count_error << "}{#rightarrow #varepsilon = (" << std::setprecision(2) << calculated_efficiency*1e4 << "#pm" << calculated_efficiency_error*1e4 << ") x 10^{-4}}";
	latex->DrawLatexNDC(0.75, 0.75, latex_text.str().c_str());
      
	std::string pdfname = "~/plots/2018-04-23/AlCapData_NormalisationCrossCheck_Si16aDataset-Run" + run_str.str() + "_" + muon_slice.str() + "_" + detnames[i_det];
	std::string direct_pdfname = pdfname + "_CrossCheckTargetSpectrum.pdf";
	std::string indirect_pdfname = pdfname + "_CrossCheckXRaySpectrum.pdf";
	//	c_direct->SaveAs(direct_pdfname.c_str());
	//	c_indirect->SaveAs(indirect_pdfname.c_str());
      }
    }
  }

  TCanvas* c_gres = new TCanvas("c_gres", "c_gres");
  for (int i_det = 1; i_det < n_dets; ++i_det) {
    for (int i_run = 0; i_run < n_runs; ++i_run) {
      TGraphErrors* gre = new TGraphErrors(n_slices, muon_slices, calc_effs[i_det][i_run], muon_slice_errors, calc_eff_errs[i_det][i_run]);
      if (i_run == 0) {
	gre->Draw("A*E");
	gre->GetYaxis()->SetRangeUser(-20e-4, 20e-4);
      }
      else {
	gre->Draw("SAME");
      }
    }
  }

  //  c_gres->SaveAs("~/plots/2018-04-24/AlCapData_NormalisationCrossCheck_Si16aDataset_SiL3.pdf");
}
