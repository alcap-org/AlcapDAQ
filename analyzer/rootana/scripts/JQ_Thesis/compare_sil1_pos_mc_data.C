{
  // SiL1 in MC goes left-to-right. Don't know about experiment. Who cares.
  // SiR1 CW from UL in RL, MC in CW from LL. So add one to MC SiR1 volID.
  TFile* fdat = new TFile("~/data/R15b/Al50/psel.3layer.root");
  TFile* fsim = new TFile("~/data/R15b/mc/R15bAl50ProtTargFromMuDist_1.root");
  TTree* trdat[2] = { (TTree*)fdat->Get("PID_LP"),
                      (TTree*)fdat->Get("PID_RP") };
  TTree* trsim = (TTree*)fsim->Get("tree");

  trdat[0]->Draw("seg>>hldat(16,0.5,16.5)", "", "goff");
  trdat[1]->Draw("seg>>hrdat(4,0.5,4.5)", "", "goff");
  trsim   ->Draw("M_volID+1>>hlsim(16,0.5,16.5)", "M_volName==\"SiL1\" && M_volID!=0 && M_volID!=15", "goff");
  trsim   ->Draw("(M_volID+2)%4>>hrsim(4,0.5,4.5)", "M_volName==\"SiR1\"", "goff");

  TH1* hldat = (TH1*)gDirectory->Get("hldat");
  TH1* hrdat = (TH1*)gDirectory->Get("hrdat");
  TH1* hlsim = (TH1*)gDirectory->Get("hlsim");
  TH1* hrsim = (TH1*)gDirectory->Get("hrsim");

  hldat->SetLineColor(1);
  hrdat->SetLineColor(1);
  hlsim->SetLineColor(2);
  hrsim->SetLineColor(2);

  hldat->SetMinimum(0);
  hrdat->SetMinimum(0);
  hlsim->SetMinimum(0);
  hrsim->SetMinimum(0);

  hldat->SetTitle("SiL1 proton hit distribution;Segment;hit/proton");
  hrdat->SetTitle("SiR1 proton hit distribution;Segment;hit/proton");
  hlsim->SetTitle("SiL1 proton hit distribution;Quadrant;hit/proton");
  hrsim->SetTitle("SiR1 proton hit distribution;Quadrant;hit/proton");

  TLegend* leg = new TLegend(0.7, 0.1, 0.9, 0.3);
  leg->AddEntry(hldat, "Data");
  leg->AddEntry(hlsim, "MC");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hldat->DrawNormalized("SAME");
  hlsim->DrawNormalized("SAME");
  leg->Draw();
  c->cd(2);
  hrdat->DrawNormalized("SAME");
  hrsim->DrawNormalized("SAME");
  leg->Draw();
  c->SaveAs("img/compare_si1_pos_mc_data.png");
}
