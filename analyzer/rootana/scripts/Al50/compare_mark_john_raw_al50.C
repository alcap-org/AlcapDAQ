{
  // Al50
  // const char* MW_IFNAME    = "data/m-wong/al50.root";
  // const char* JQ_IFNAME    = "data/Al50/psel.3layer.root";
  // const char* DEVE_TITLE_L = "Al50 Left e1:e1+e2 {!e3};E_{tot} [keV];dE [keV]";
  // const char* DEVE_TITLE_R = "Al50 Right e1:e1+e2 {!e3};E_{tot} [keV];dE [keV]";
  // const char* E_TITLE_L    = "Al50 Left e1+e2 {!e3};E_{tot} [keV]";
  // const char* E_TITLE_R    = "Al50 Right e1+e2 {!e3};E_{tot} [keV]";
  // Al100
  const char* MW_IFNAME    = "data/m-wong/al100.root";
  const char* JQ_IFNAME    = "data/Al100/psel.3layer.root";
  const char* DEVE_TITLE_L = "Al100 Left e1:e1+e2 {!e3};E_{tot} [keV];dE [keV]";
  const char* DEVE_TITLE_R = "Al100 Right e1:e1+e2 {!e3};E_{tot} [keV];dE [keV]";
  const char* E_TITLE_L    = "Al100 Left e1+e2 {!e3};E_{tot} [keV]";
  const char* E_TITLE_R    = "Al100 Right e1+e2 {!e3};E_{tot} [keV]";


  TFile* f_mw    = new TFile(MW_IFNAME);
  TFile* f_jq    = new TFile(JQ_IFNAME);
  TTree* tr_mw   = (TTree*)f_mw->Get("tree");
  TTree* tr_jq[] = { (TTree*)f_jq->Get("PID_LP"), (TTree*)f_jq->Get("PID_RP") };
  tr_mw->Draw("1e3*e1:1e3*(e1+e2)>>hdeve_mw_l(1000,0,20e3,250,0,2.5e3)", "pid==\"proton\" && channel.Contains(\"SiL1\") && TMath::IsNaN(e3)", "goff");
  tr_mw->Draw("1e3*e1:1e3*(e1+e2)>>hdeve_mw_r(1000,0,20e3,250,0,2.5e3)", "pid==\"proton\" && channel.Contains(\"SiR1\") && TMath::IsNaN(e3)", "goff");
  tr_jq[0]->Draw("e1:e1+e2>>hdeve_jq_l(1000,0,20e3,250,0,2.5e3)", "e3 == 0", "goff");
  tr_jq[1]->Draw("e1:e1+e2>>hdeve_jq_r(1000,0,20e3,250,0,2.5e3)", "e3 == 0", "goff");

  TH1* he_mw_l = hdeve_mw_l->ProjectionX("he_mw_l");
  TH1* he_mw_r = hdeve_mw_r->ProjectionX("he_mw_r");
  TH1* he_jq_l = hdeve_jq_l->ProjectionX("he_jq_l");
  TH1* he_jq_r = hdeve_jq_r->ProjectionX("he_jq_r");

  hdeve_jq_l->SetTitle(DEVE_TITLE_L);
  hdeve_jq_r->SetTitle(DEVE_TITLE_R);
  he_jq_l->SetTitle(E_TITLE_L);
  he_jq_r->SetTitle(E_TITLE_R);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(he_jq_l, "JQ");
  leg->AddEntry(he_mw_l, "MW");

  hdeve_mw_l->SetMarkerColor(6);
  hdeve_mw_r->SetMarkerColor(6);
  hdeve_jq_l->SetMarkerColor(7);
  hdeve_jq_r->SetMarkerColor(7);
  he_mw_l->SetLineColor(6);
  he_mw_r->SetLineColor(6);
  he_jq_l->SetLineColor(7);
  he_jq_r->SetLineColor(7);
  TCanvas* ce = new TCanvas("ce", "ce", 1400, 500);
  ce->Divide(2);
  ce->cd(1); he_jq_l->Draw(); he_mw_l->Draw("SAME"); leg->Draw();
  ce->cd(2); he_jq_r->Draw(); he_mw_r->Draw("SAME"); leg->Draw();
  TCanvas* cdeve = new TCanvas("cdeve", "cdeve", 1400, 500);
  cdeve->Divide(2);
  cdeve->cd(1); hdeve_jq_l->Draw(); hdeve_mw_l->Draw("SAME"); leg->Draw();
  cdeve->cd(2); hdeve_jq_r->Draw(); hdeve_mw_r->Draw("SAME"); leg->Draw();

  int n_mw_l = tr_mw->GetEntries("pid==\"proton\" && channel.Contains(\"SiL1\") && TMath::IsNaN(e3)");
  int n_mw_r = tr_mw->GetEntries("pid==\"proton\" && channel.Contains(\"SiR1\") && TMath::IsNaN(e3)");
  int n_jq_l = tr_jq[0]->GetEntries("e3==0");
  int n_jq_r = tr_jq[1]->GetEntries("e3==0");
  printf("\tMW\tJQ\n");
  printf("Left\t%d\t%d\n", n_mw_l, n_jq_l);
  printf("Right\t%d\t%d\n", n_mw_r, n_jq_r);
}
