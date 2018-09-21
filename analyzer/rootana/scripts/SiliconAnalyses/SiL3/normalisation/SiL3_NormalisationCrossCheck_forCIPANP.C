void SiL3_NormalisationCrossCheck_forCIPANP() {

  std::string infilename = "~/data/results/SiL3_active/normalisation.root";
  //  std::string infilename = "~/data/results/SiL3_active_no-templates/normalisation.root";
  TFile* file = new TFile(infilename.c_str(), "READ");

  double min_slice = 3000;
  double max_slice = 6000;
  double slice_step = 3000;
  const int n_slices = 1;
  double muon_slices[n_slices];
  double muon_slice_errors[n_slices];
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    muon_slices[i_slice] = min_slice + (i_slice+0.5)*slice_step;
    muon_slice_errors[i_slice] = slice_step/2;
  }
  double calc_effs[n_slices];
  double calc_eff_errs[n_slices];

  std::stringstream muon_slice;
  for (double i_muon_slice_start = min_slice; i_muon_slice_start < max_slice; i_muon_slice_start += slice_step) {
    int i_slice = (i_muon_slice_start - min_slice) / slice_step;
    double i_muon_slice_end = i_muon_slice_start + slice_step;
    muon_slice.str("");
    muon_slice << "MuonSlice" << (int)i_muon_slice_start << "_" << (int)i_muon_slice_end;

    std::string direct_histname = "hTarget_" + muon_slice.str();
    std::string direct_count_treename = "hTarget_wSiL3_count_" + muon_slice.str();
    std::string indirect_count_treename = "hXRaySpectrum_count_wSiL3Coinc_" + muon_slice.str();
    std::string wsname = "ws_hXRaySpectrum_count_wSiL3Coinc_" + muon_slice.str();

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

    calc_effs[i_slice] = calculated_efficiency;
    calc_eff_errs[i_slice] = calculated_efficiency_error;

    std::stringstream latex_text;
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    ///////////////////////////////////////////////////
    // Target Spectrum
    TCanvas* c_direct = new TCanvas("c_direct", "c_direct");
    TH1F* hTarget_Spectrum = (TH1F*) file->Get(direct_histname.c_str());
    hTarget_Spectrum->SetTitle("Target Spectrum");
    hTarget_Spectrum->SetStats(false);
    hTarget_Spectrum->SetLineColor(kBlue);
    hTarget_Spectrum->GetXaxis()->SetRangeUser(0, 10000);
    hTarget_Spectrum->Draw();

    latex_text << "#splitline{   " << min_muon_time << " < t < " << max_muon_time << " ns}{" << min_muon_energy << " < E < " << max_muon_energy << " keV}";
    latex->DrawLatexNDC(0.74, 0.6, latex_text.str().c_str());
    
    TLine* min_muon_energy_line = new TLine(min_muon_energy, hTarget_Spectrum->GetMinimum(), min_muon_energy, hTarget_Spectrum->GetMaximum());
    min_muon_energy_line->SetLineWidth(2);
    min_muon_energy_line->Draw("LSAME");
    
    TLine* max_muon_energy_line = new TLine(max_muon_energy, hTarget_Spectrum->GetMinimum(), max_muon_energy, hTarget_Spectrum->GetMaximum());
    max_muon_energy_line->SetLineWidth(2);
    max_muon_energy_line->Draw("LSAME");

    latex->DrawLatexNDC(0.74, 0.7, "AlCap Preliminary");
    
    //    latex_text.str("");
    //    latex_text << "N_{stop} = (" << std::fixed << std::setprecision(3) << direct_count/1e6 << " #pm " << direct_count_error/1e6 << ") x 10^{6}";
    //    latex->DrawLatexNDC(0.5, 0.4, latex_text.str().c_str());
    
    
    ///////////////////////////////////////////////////
    // X-Ray Spectrum
    TCanvas* c_indirect = new TCanvas("c_indirect", "c_indirect");
    RooWorkspace* ws = (RooWorkspace*) file->Get(wsname.c_str());
    ws->Print();
    
    // Draw the fit
    RooPlot* Eframe = (ws->var("edep"))->frame();
    Eframe->SetTitle("X-Ray Spectrum w/ Coincident Muon Hit in Target");
    Eframe->GetXaxis()->SetTitle("Energy [keV]");
    //  Eframe->SetMinimum(3000);
    (ws->data("data"))->plotOn(Eframe);
    (ws->pdf("sum"))->plotOn(Eframe);
    Eframe->Draw();
    
    latex_text.str("");
    latex_text << std::fixed << std::setprecision(3);
    latex_text << "#varepsilon_{Eu} = (" << std::setprecision(2) << xray_efficiency*1e4 << "#pm" << xray_efficiency_error*1e4 << ") x 10^{-4}";
    //    latex->DrawLatexNDC(0.73, 0.70, latex_text.str().c_str());
    
    latex_text.str("");
    latex_text << "#varepsilon_{#frac{N_{x-ray}/I}{N_{stop}}} = (" << std::setprecision(2) << calculated_efficiency*1e4 << "#pm" << calculated_efficiency_error*1e4 << ") x 10^{-4}";
    //    latex->DrawLatexNDC(0.3, 0.68, latex_text.str().c_str());

    latex->DrawLatexNDC(0.3, 0.77, "AlCap Preliminary");

    /* = " << std::fixed << std::setprecision(0) << xray_count << " #pm " << xray_count_error << "}*/
    
    //    std::string pdfname = "~/plots/2018-05-08/AlCapData_NormalisationCrossCheck_SiL3DatasetNoTemplates_" + muon_slice.str();
    //    std::string direct_pdfname = pdfname + "_CrossCheckTargetSpectrum.pdf";
    //    std::string indirect_pdfname = pdfname + "_CrossCheckXRaySpectrum.pdf";
    //    c_direct->SaveAs(direct_pdfname.c_str());
    //    c_indirect->SaveAs(indirect_pdfname.c_str());
  }

}
