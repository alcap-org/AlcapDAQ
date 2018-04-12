void SiL3_NormalisationCrossCheck() {

  std::string infilename = "~/data/results/SiL3_active/normalisation.root";
  TFile* file = new TFile(infilename.c_str(), "READ");

  TTree* direct_count_tree = (TTree*) file->Get("direct_count");
  double direct_count = 0;
  double direct_count_error = 0;
  direct_count_tree->SetBranchAddress("n_stopped_muons", &direct_count);
  direct_count_tree->SetBranchAddress("n_stopped_muons_error", &direct_count_error);
  direct_count_tree->GetEntry(0);  
  std::cout << "Direct Count = " << direct_count << " +/- " << direct_count_error << " (" << direct_count - direct_count_error << " -- " << direct_count + direct_count_error << ")" << std::endl;
  
  TTree* indirect_count_tree = (TTree*) file->Get("indirect_count");
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

  double calculated_efficiency = (xray_count / (xray_intensity * direct_count));
  std::cout << "Original Eff = " << xray_efficiency << " +/- " << xray_efficiency_error << std::endl;
  std::cout << "Calculated Eff = " << calculated_efficiency << std::endl;
}
