{
  TFile* fsim = new TFile("data/Al50/tm.p.2layer.mudist.root");
  TFile* fdat = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* trsim[2] = { (TTree*)fsim->Get("SiL_TransTree"),
                      (TTree*)fsim->Get("SiR_TransTree") };
  TTree* trdat[2] = { (TTree*)fdat->Get("PID_LP"),
                      (TTree*)fdat->Get("PID_RP") };
  trdat[0]->SetBranchStatus("*",     false);
  trdat[1]->SetBranchStatus("*",     false);
  trsim[0]->SetBranchStatus("*",     false);
  trsim[1]->SetBranchStatus("*",     false);
  trdat[0]->SetBranchStatus("e1",     true);
  trdat[0]->SetBranchStatus("e2",     true);
  trdat[1]->SetBranchStatus("e1",     true);
  trdat[1]->SetBranchStatus("e2",     true);
  trsim[0]->SetBranchStatus("demeas", true);
  trsim[0]->SetBranchStatus("emeas",  true);
  trsim[1]->SetBranchStatus("demeas", true);
  trsim[1]->SetBranchStatus("emeas",  true);
  TGraph* gsim[2] = {
    new TGraph(trdat[0]->GetEntries()),
    new TGraph(trdat[1]->GetEntries())
  };
  TGraph* gdat[2] = {
    new TGraph(trdat[0]->GetEntries()),
    new TGraph(trdat[1]->GetEntries())
  };
  for (int lr = 0; lr < 2; ++lr) {
    double e1dat, e2dat, desim, esim;
    trdat[lr]->SetBranchAddress("e1",     &e1dat);
    trdat[lr]->SetBranchAddress("e2",     &e2dat);
    trsim[lr]->SetBranchAddress("demeas", &desim);
    trsim[lr]->SetBranchAddress("emeas",  &esim);
    for (int i = 0; i < trdat[lr]->GetEntries(); ++i) {
      trdat[lr]->GetEntry(i);
      gdat[lr]->SetPoint(i, e1dat+e2dat, e1dat);
    }
    for (int i = 0; i < trsim[lr]->GetEntries()/100; ++i) {
      trsim[lr]->GetEntry(i);
      gsim[lr]->SetPoint(i, esim, desim);
    }
    trdat[lr]->ResetBranchAddresses();
    trsim[lr]->ResetBranchAddresses();
  }

  gdat[0]->SetMarkerColor(kBlue+2);
  gdat[1]->SetMarkerColor(kBlue+2);
  gsim[0]->SetMarkerColor(kMagenta);
  gsim[1]->SetMarkerColor(kMagenta);
  gdat[0]->SetMarkerStyle(1);
  gdat[1]->SetMarkerStyle(1);
  gsim[0]->SetMarkerStyle(1);
  gsim[1]->SetMarkerStyle(1);

  gdat[0]->SetTitle("SiL Proton Band;E [keV];dE [keV]");
  gdat[1]->SetTitle("SiR Proton Band;E [keV];dE [keV]");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(gdat[0], "Data", "")->SetTextColor(kBlue+2);
  leg->AddEntry(gsim[0], "MC",   "")->SetTextColor(kMagenta);

  TCanvas* cg = new TCanvas("cg", "cg", 1400, 500);
  cg->Divide(2);
  cg->cd(1); gdat[0]->Draw("AP"); gsim[0]->Draw("P SAME"); leg->Draw();
  cg->cd(2); gdat[1]->Draw("AP"); gsim[1]->Draw("P SAME"); leg->Draw();
  cg->SaveAs("img/deve_sim_data.png");

}
