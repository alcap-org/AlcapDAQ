{
  TChain* ch[2] = { new TChain("trl", "trl"), new TChain("trr", "trr") };
  ch[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LP");
  ch[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LD");
  ch[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LT");
  ch[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LA");
  ch[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LU");
  ch[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RP");
  ch[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RD");
  ch[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RT");
  ch[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RA");
  ch[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RU");
  ch[0]->Draw("de:e>>hl(2000,0,20e3,300,0,3e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  ch[1]->Draw("de:e>>hr(2000,0,20e3,300,0,3e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");

  TF1* fl_new[2] = {
    new TF1("fll_new", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", 1.9e3, 17e3),
    new TF1("flh_new", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", 1.9e3, 17e3)
  };
  TF1* fr_new[2] = {
    new TF1("frl_new", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", 1.9e3, 17e3),
    new TF1("frh_new", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", 1.9e3, 17e3)
  };
  TF1* fl_old[2] = {
    new TF1("fll_old", "[0]*x^[1]", 1.9e3, 17e3),
    new TF1("flh_old", "[0]*x^[1]", 1.9e3, 17e3)
  };
  TF1* fr_old[2] = {
    new TF1("frl_old", "[0]*x^[1]", 1.9e3, 17e3),
    new TF1("frh_old", "[0]*x^[1]", 1.9e3, 17e3)
  };
  // double P1[2] = {2400, 2100}, P2[2] = {4000, 1200}, P3[2] = {14000, 250};
  // double b = std::log(P1[1]/P2[1])/std::log(P1[0]/P2[0]);
  // double A = P1[1]/std::pow(P1[0], b);
  fl_new[0]->SetParameters(4e3, 35e5, -1.02, -0.85);
  fl_new[1]->SetParameters(4e3, 44e5, -1.00, -0.78);
  fr_new[0]->SetParameters(4e3, 60e5, -1.09, -0.95);
  fr_new[1]->SetParameters(4e3, 11e6, -1.10, -0.85);
  fl_old[0]->SetParameters(85e4, -0.85);
  fl_old[1]->SetParameters(15e5, -0.85);
  fr_old[0]->SetParameters(12e5, -0.90);
  fr_old[1]->SetParameters(22e5, -0.90);

  TLine* ll_old[2] = {
    new TLine(1.9e3, fl_old[0]->Eval(1.9e3), 1.9e3, fl_old[1]->Eval(1.9e3)),
    new TLine(17e3,  fl_old[0]->Eval(17e3),  17e3,  fl_old[1]->Eval(17e3))
  };
  TLine* lr_old[2] = {
    new TLine(1.9e3, fr_old[0]->Eval(1.9e3), 1.9e3, fr_old[1]->Eval(1.9e3)),
    new TLine(17e3,  fr_old[0]->Eval(17e3),  17e3,  fr_old[1]->Eval(17e3))
  };
  TLine* ll_new[2] = {
    new TLine(1.9e3, fl_new[0]->Eval(1.9e3), 1.9e3, fl_new[1]->Eval(1.9e3)),
    new TLine(17e3,  fl_new[0]->Eval(17e3),  17e3,  fl_new[1]->Eval(17e3))
  };
  TLine* lr_new[2] = {
    new TLine(1.9e3, fr_new[0]->Eval(1.9e3), 1.9e3, fr_new[1]->Eval(1.9e3)),
    new TLine(17e3,  fr_new[0]->Eval(17e3),  17e3,  fr_new[1]->Eval(17e3))
  };


  ll_new[0]->SetLineColor(3);
  ll_new[1]->SetLineColor(3);
  lr_new[0]->SetLineColor(3);
  lr_new[1]->SetLineColor(3);
  ll_old[0]->SetLineColor(2);
  ll_old[1]->SetLineColor(2);
  lr_old[0]->SetLineColor(2);
  lr_old[1]->SetLineColor(2);
  fl_new[0]->SetLineColor(3);
  fl_new[1]->SetLineColor(3);
  fr_new[0]->SetLineColor(3);
  fr_new[1]->SetLineColor(3);
  fl_old[0]->SetLineColor(2);
  fl_old[1]->SetLineColor(2);
  fr_old[0]->SetLineColor(2);
  fr_old[1]->SetLineColor(2);

  hl->SetTitle("SiL New PID (Standard cuts);E [keV];dE [keV]");
  hr->SetTitle("SiR New PID (Standard cuts);E [keV];dE [keV]");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(ll_old[0], "Old", "l");
  leg->AddEntry(ll_new[0], "New", "l");

  gStyle->SetOptStat(0);
  TCanvas* cdeve = new TCanvas("cdeve", "cdeve", 1400, 500);
  cdeve->Divide(2);
  cdeve->cd(1);
  hl->Draw();
  fl_old[0]->Draw("SAME");
  fl_old[1]->Draw("SAME");
  fl_new[0]->Draw("SAME");
  fl_new[1]->Draw("SAME");
  ll_old[0]->Draw("SAME");
  ll_old[1]->Draw("SAME");
  ll_new[0]->Draw("SAME");
  ll_new[1]->Draw("SAME");
  leg      ->Draw("SAME");
  cdeve->cd(2);
  hr->Draw();
  fr_old[0]->Draw("SAME");
  fr_old[1]->Draw("SAME");
  fr_new[0]->Draw("SAME");
  fr_new[1]->Draw("SAME");
  lr_old[0]->Draw("SAME");
  lr_old[1]->Draw("SAME");
  lr_new[0]->Draw("SAME");
  lr_new[1]->Draw("SAME");
  leg      ->Draw("SAME");
  cdeve->SaveAs("img/new_pid_cut_lin_standardcuts.png");


  //////////////////////////////////////////////////////////////////////////////
  TFile* f_old = new TFile("data/Al50/psel.3layer.pidpow1.root");
  TFile* f_new = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* tr_old[2] = {
    (TTree*)f_old->Get("PID_LP"), (TTree*)f_old->Get("PID_RP")
  };
  TTree* tr_new[2] = {
    (TTree*)f_new->Get("PID_LP"), (TTree*)f_new->Get("PID_RP")
  };

  tr_old[0]->Draw("e>>hl_old(135,1.5e3,15e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  tr_old[1]->Draw("e>>hr_old(135,1.5e3,15e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  tr_new[0]->Draw("e>>hl_new(135,1.5e3,15e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  tr_new[1]->Draw("e>>hr_new(135,1.5e3,15e3)", "e3==0 && abs(dt)<200 && 400<t && t<10e3", "goff");
  hl_old->SetLineColor(2);
  hr_old->SetLineColor(2);
  hl_new->SetLineColor(3);
  hr_new->SetLineColor(3);
  hl_old->SetTitle("SiL Proton Energy;E [keV];Counts/100 keV");
  hr_old->SetTitle("SiR Proton Energy;E [keV];Counts/100 keV");
  hl_new->SetTitle("SiL Proton Energy;E [keV];Counts/100 keV");
  hr_new->SetTitle("SiR Proton Energy;E [keV];Counts/100 keV");

  TCanvas* ce = new TCanvas("ce", "ce", 1400, 500);
  ce->Divide(2);
  ce->cd(1);
  hl_old->Draw("SAME");
  hl_new->Draw("SAME");
  leg   ->Draw("SAME");
  ce->cd(2);
  hr_old->Draw("SAME");
  hr_new->Draw("SAME");
  leg   ->Draw("SAME");
  ce->SaveAs("img/new_pid_cut_energy.png");
}
