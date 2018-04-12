{
  const char IFNAME[] = "~/data/R15b/evdeal50.root";
  TFile* ifile = new TFile(IFNAME);
  TH3* evde_r0 = (TH3*)ifile->Get("evde_r0");
  TH3* evde_l0 = (TH3*)ifile->Get("evde_l0");

  gStyle->SetOptStat("e");
  TH1* hrall = evde_r0->Project3D("yxe");
  TH1* hlall = evde_l0->Project3D("yxe");
  evde_r0->GetZaxis()->SetRange(2, 5);
  evde_l0->GetZaxis()->SetRange(2, 5);
  TH1* hrcut = evde_r0->Project3D("yxecut");
  TH1* hlcut = evde_l0->Project3D("yxecut");
  hrcut->SetMaximum(hrall->GetMaximum());
  hlcut->SetMaximum(hlall->GetMaximum());
  std::cout << evde_r0->GetEntries() << " " << evde_l0->GetEntries() << " "
            << hrall->GetEntries()   << " " << (int)hlall->GetEntries()
            << std::endl;
  hrall->SetTitle("E vs dE Right (All);E_{SiR1}+E_{SiR2}[keV];E_{SiR1}[keV]");
  hlall->SetTitle("E vs dE Left (All);E_{SiL1}+E_{SiL3}[keV];E_{SiL1}[keV]");
  hrcut->SetTitle("E vs dE Right (>400 ns);E_{SiR1}+E_{SiR2}[keV];E_{SiR1}[keV]");
  hlcut->SetTitle("E vs dE Left (>400 ns);E_{SiL1}+E_{SiL3}[keV];E_{SiL1}[keV]");
  hrall->GetYaxis()->SetTitleOffset(1.4);
  hlall->GetYaxis()->SetTitleOffset(1.4);
  hrcut->GetYaxis()->SetTitleOffset(1.4);
  hlcut->GetYaxis()->SetTitleOffset(1.4);
  TCanvas* crall = new TCanvas();
  hrall->Draw("COL");
  crall->SetLogz();
  crall->SaveAs("img/data_evde_r_all.png");
  TCanvas* clall = new TCanvas();
  hlall->Draw("COL");
  clall->SetLogz();
  clall->SaveAs("img/data_evde_l_all.png");
  TCanvas* crcut = new TCanvas();
  hrcut->Draw("COL");
  crcut->SetLogz();
  crcut->SaveAs("img/data_evde_r_cut.png");
  TCanvas* clcut = new TCanvas();
  hlcut->Draw("COL");
  clcut->SetLogz();
  clcut->SaveAs("img/data_evde_l_cut.png");
}
