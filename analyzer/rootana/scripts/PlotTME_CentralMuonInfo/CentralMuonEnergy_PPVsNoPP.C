// To use (from rootana/):
// > root -l
// [0] .x scripts/PlotTME_CentralMuonInfo/CentralMuonEnergy_PPVsNoPP.C("file.root")

void CentralMuonEnergy_PPVsNoPP(std::string filename) {
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Couldn't open file " << filename << std::endl;
    return;
  }
TH1D* hNoPP = ((TH2F*) file->Get("PlotTME_CentralMuonInfo/hCentralMuonEnergyTime"))->ProjectionY("noPP");
  TH1D* hPP = ((TH2F*) file->Get("PlotTME_CentralMuonInfo/hCentralMuonEnergyTime_PP"))->ProjectionY("PP");

  hPP->SetLineColor(kRed);

  hNoPP->SetTitle("Energy of the Central Muon in the TME");
  hNoPP->Draw("HIST E");
  hPP->Draw("HIST E SAMES");    

  TLegend *leg = new TLegend(0.5,0.8,0.7,0.6);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(hNoPP, "No PP", "l");
  leg->AddEntry(hPP, "PP", "l");
  leg->Draw();
}
