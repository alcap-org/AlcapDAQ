{
  const char IFNAME[] = "~/data/R15b/evdeal50.root";

  TFile* ifile = new TFile(IFNAME);
  gStyle->SetOptStat("e");
  TH1* hrall = evde_r0->Project3D("yxe");
  TH1* hlall = evde_l0->Project3D("yxe");
  printf("%d %d %d %d\n", evde_r0->GetEntries(), evde_l0->GetEntries(), hrall->GetEntries(), hlall->GetEntries());
  hrall->SetTitle("E vs dE SiR (w/o veto);E_{SiR1}+E_{SiR2}[keV];E_{SiR1}[keV]");
  hlall->SetTitle("E vs dE SiL (w/o veto);E_{SiL1}+E_{SiL3}[keV];E_{SiL1}[keV]");
  hrall->GetYaxis()->SetTitleOffset(1.4);
  hlall->GetYaxis()->SetTitleOffset(1.4);
  TCanvas* cr = new TCanvas();
  hrall->Draw("COL");
  cr->SaveAs("img/data_evde_r.png");
  TCanvas* cl = new TCanvas();
  hlall->Draw("COL");
  cl->SaveAs("img/data_evde_l.png");
}
