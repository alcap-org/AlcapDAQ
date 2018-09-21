{
  gROOT->ProcessLine
  TFile* fmw = new TFile("data/m-wong/al100.root");
  TFile* fjq = new TFile("data/Al100/psel.2layer.root");

  TH1* sil_mw = (TH1*)fmw->Get("SiL_proton_E");
  TH1* sir_mw = (TH1*)fmw->Get("SiR_proton_E");
  TTree* tr_l_jq = (TTree*)fjq->Get("PID_LP");
  TTree* tr_r_jq = (TTree*)fjq->Get("PID_RP");
  tr_l_jq->Draw("e/1e3>>sil_jq");
  tr_r_jq->Draw("e/1e3>>sir_jq");

}
