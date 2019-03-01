void SiL3_RoughPlots_wTgtMuon() {

  gStyle->SetOptFit(1);

  std::string noTgtMuon_filename = "~/data/results/SiL3/subtrees.root";
  TFile* noTgtMuon_file = new TFile(noTgtMuon_filename.c_str(), "READ");
  TTree* noTgtMuon_tree = (TTree*) noTgtMuon_file->Get("siBlockTree_Target");
  TH1F* noTgtMuon_hist = new TH1F("noTgtMuon_hist", "", 300,0,30000);
  noTgtMuon_hist->SetLineColor(kBlack);
  noTgtMuon_tree->Draw("thick_energy>>noTgtMuon_hist", "thick_tpi_id>=0 && thick_time>0 && thick_time<4000", "goff");

  std::string wTgtMuon_filename = "~/data/results/SiL3/subtrees_wTgtMuon.root";
  TFile* wTgtMuon_file = new TFile(wTgtMuon_filename.c_str(), "READ");
  TTree* wTgtMuon_tree = (TTree*) wTgtMuon_file->Get("siBlockTree_Target");
  TH1F* wTgtMuon_hist = new TH1F("wTgtMuon_hist", "", 300,0,30000);
  wTgtMuon_hist->SetLineColor(kRed);
  wTgtMuon_tree->Draw("thick_energy>>wTgtMuon_hist", "thick_tpi_id>=0 && thick_time>0 && thick_time<4000", "goff");

  noTgtMuon_hist->Draw("HIST E");
  wTgtMuon_hist->Draw("HIST E SAMES");
}
