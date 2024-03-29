void SiL3_NormalisationCrossCheck() {

  std::string infilename = "~/data/results/SiL3_active/normalisation.root";
  TFile* file = new TFile(infilename.c_str(), "READ");

  //  std::string direct_count_treename = "direct_cross_check";
  //  std::string indirect_count_treename = "indirect_cross_check";
  //  std::string wsname = "ws_cross_check";

  std::string direct_count_treename = "direct_count";
  std::string indirect_count_treename = "indirect_count";
  std::string wsname = "ws";

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

  ///////////////////////////////////////////////////
  // Target Spectrum
  TCanvas* c_direct = new TCanvas("c_direct", "c_direct");
  TH1F* hTarget_Spectrum = (TH1F*) file->Get("hTarget_Spectrum");
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
  latex_text << "N_{stop} = (" << std::fixed << std::setprecision(3) << direct_count/1e6 << " #pm " << direct_count_error/1e6 << ") x 10^{6}";
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
  latex_text << "#splitline{  #varepsilon = (" << std::setprecision(2) << xray_efficiency*1e4 << " #pm " << xray_efficiency_error*1e4 << ") x 10^{-4}}{#rightarrow N_{stop} = (" << std::fixed << std::setprecision(1) << indirect_count/1e6 << " #pm " << indirect_count_error/1e6 << ") x 10^{6}}";
  latex->DrawLatexNDC(0.3, 0.75, latex_text.str().c_str());

  latex_text.str("");
  latex_text << "#splitline{  N_{x-ray} = " << std::fixed << std::setprecision(0) << xray_count << " #pm " << xray_count_error << "}{#rightarrow #varepsilon = (" << std::setprecision(2) << calculated_efficiency*1e4 << "#pm" << calculated_efficiency_error*1e4 << ") x 10^{-4}}";
  latex->DrawLatexNDC(0.75, 0.75, latex_text.str().c_str());
}
