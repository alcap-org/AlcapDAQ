void ResponseMatrix() {

  /////////////////////////////////////
  // User parameters
  std::string filename = "~/data/results/Si16b_passive/unfolded.root";
  std::string histname = "hResponseMatrix";

  TFile* file = new TFile(filename.c_str(), "READ");
    
  TH2F* hResponseMatrix = (TH2F*) file->Get(histname.c_str());
  hResponseMatrix->Draw("COLZ");
}
