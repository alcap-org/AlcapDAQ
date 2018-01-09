{
  const char IFNAME[] = "~/data/R15b/evdeal50.root";

  TFile* ifile = new TFile(IFNAME);
  gStyle->SetOptStat("e");
  evde_r0->SetTitle("E vs dE SiR (w/o veto);E_{SiR1}+E_{SiR2}[keV];E_{SiR1}[keV]");
  evde_l0->SetTitle("E vs dE SiL (w/o veto);E_{SiL1}+E_{SiL3}[keV];E_{SiL1}[keV]");
  evde_r0->GetYaxis()->SetTitleOffset(1.4);
  evde_l0->GetYaxis()->SetTitleOffset(1.4);
  TCanvas* cr = new TCanvas();
  evde_r0->Draw("COL");
  cr->SaveAs("data_evde_r.png");
  TCanvas* cl = new TCanvas();
  evde_l0->Draw("COL");
  cr->SaveAs("data_evde_l.png");
}
