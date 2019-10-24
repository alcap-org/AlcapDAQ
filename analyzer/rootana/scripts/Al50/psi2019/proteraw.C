{
	Int_t kACBlue = TColor::GetColor("#67a9cf");
  Int_t kACRed  = TColor::GetColor("#ef8a62");

  TFile* fcut   = new TFile("~/Documents/psi2019/al50-cuts.root");
  TFile* fdata  = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TChain* tr[2] = { new TChain(), new TChain() };
  tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LP");
  tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LD");
  tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LT");
  tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LA");
  tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LU");
  tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RP");
  tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RD");
  tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RT");
  tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RA");
  tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RU");
  TCutG* gcut[2]  = {
    (TCutG*)fcut->Get("hLg_SiL_EvDeltaE_proton_3sigma"),
    (TCutG*)fcut->Get("hLg_SiR_EvDeltaE_proton_3sigma")
  };
  gcut[0]->SetVarX("e/1e3");
  gcut[0]->SetVarY("de/1e3");
  gcut[0]->SetName("glcut");
  gcut[1]->SetVarX("e/1e3");
  gcut[1]->SetVarY("de/1e3");
  gcut[1]->SetName("grcut");
  TH1* he[2] = {
    new TH1D("hle", "Measured Proton Energy;E [keV];Protons/100 keV", 120, 2e3, 14e3),
    new TH1D("hre", "Measured Proton Energy;E [keV];Protons/100 keV", 120, 2e3, 14e3)
  };
  tr[0]->Draw("e>>hle", "glcut", "goff");
  tr[1]->Draw("e>>hre", "grcut", "goff");

  he[0]->SetLineColor(kACBlue);
  he[1]->SetLineColor(kACRed);

  TLegend* leg = new TLegend(0.75, 0.75, 0.95, 0.95);
  leg->AddEntry(he[0], "SiL");
  leg->AddEntry(he[1], "SiR");

  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  he[1]->Draw();
  he[0]->Draw("SAME");
  leg  ->Draw();
  c->SaveAs("img/psi2019/prote.png");
}
