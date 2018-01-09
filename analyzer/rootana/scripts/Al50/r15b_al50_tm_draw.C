{
  //////////////////////////////////////////////////////////////////////////////
  // USER DEFINED VARIABLES
  const char IFNAME_P[]     = "~/data/R15b/al50ptm.root";
  const char IFNAME_D[]     = "~/data/R15b/al50dtm.root";
  const char IFNAME_T[]     = "~/data/R15b/al50ttm.root";
  const char IFNAME_A[]     = "~/data/R15b/al50atm.root";
  //////////////////////////////////////////////////////////////////////////////

  TFile* fp = new TFile(IFNAME_P);
  TFile* fd = new TFile(IFNAME_D);
  TFile* ft = new TFile(IFNAME_T);
  TFile* fa = new TFile(IFNAME_A);
  RooUnfoldResponse* resp_pl = (RooUnfoldResponse*)fp->Get("SiL_TM");
  RooUnfoldResponse* resp_pr = (RooUnfoldResponse*)fp->Get("SiR_TM");
  RooUnfoldResponse* resp_dl = (RooUnfoldResponse*)fd->Get("SiL_TM");
  RooUnfoldResponse* resp_dr = (RooUnfoldResponse*)fd->Get("SiR_TM");
  RooUnfoldResponse* resp_tl = (RooUnfoldResponse*)ft->Get("SiL_TM");
  RooUnfoldResponse* resp_tr = (RooUnfoldResponse*)ft->Get("SiR_TM");
  RooUnfoldResponse* resp_al = (RooUnfoldResponse*)fa->Get("SiL_TM");
  RooUnfoldResponse* resp_ar = (RooUnfoldResponse*)fa->Get("SiR_TM");
  TH2* h_pl = resp_pl->Hresponse(); TH2* h_pr = resp_pr->Hresponse();
  TH2* h_dl = resp_dl->Hresponse(); TH2* h_dr = resp_dr->Hresponse();
  TH2* h_tl = resp_tl->Hresponse(); TH2* h_tr = resp_tr->Hresponse();
  TH2* h_al = resp_al->Hresponse(); TH2* h_ar = resp_ar->Hresponse();
  h_pl->SetTitle("SiL Proton Response;E_{meas} [keV];E_{true} [keV]");
  h_pr->SetTitle("SiR Proton Response;E_{meas} [keV];E_{true} [keV]");
  h_dl->SetTitle("SiL Deuteron Response;E_{meas} [keV];E_{true} [keV]");
  h_dr->SetTitle("SiR Deuteron Response;E_{meas} [keV];E_{true} [keV]");
  h_tl->SetTitle("SiL Triton Response;E_{meas} [keV];E_{true} [keV]");
  h_tr->SetTitle("SiR Triton Response;E_{meas} [keV];E_{true} [keV]");
  h_al->SetTitle("SiL Alpha Response;E_{meas} [keV];E_{true} [keV]");
  h_ar->SetTitle("SiR Alpha Response;E_{meas} [keV];E_{true} [keV]");
  h_pl->GetXaxis()->SetRangeUser(0, 15e3); h_pl->GetYaxis()->SetRangeUser(0, 15e3);
  h_pr->GetXaxis()->SetRangeUser(0, 15e3); h_pr->GetYaxis()->SetRangeUser(0, 15e3);
  h_dl->GetXaxis()->SetRangeUser(0, 15e3); h_dl->GetYaxis()->SetRangeUser(0, 15e3);
  h_dr->GetXaxis()->SetRangeUser(0, 15e3); h_dr->GetYaxis()->SetRangeUser(0, 15e3);
  h_tl->GetXaxis()->SetRangeUser(0, 15e3); h_tl->GetYaxis()->SetRangeUser(0, 15e3);
  h_tr->GetXaxis()->SetRangeUser(0, 15e3); h_tr->GetYaxis()->SetRangeUser(0, 15e3);
  h_al->GetXaxis()->SetRangeUser(0, 15e3); h_al->GetYaxis()->SetRangeUser(0, 15e3);
  h_ar->GetXaxis()->SetRangeUser(0, 15e3); h_ar->GetYaxis()->SetRangeUser(0, 15e3);
  h_pl->GetYaxis()->SetTitleOffset(1.4);   h_pr->GetYaxis()->SetTitleOffset(1.4);
  h_dl->GetYaxis()->SetTitleOffset(1.4);   h_dr->GetYaxis()->SetTitleOffset(1.4);
  h_tl->GetYaxis()->SetTitleOffset(1.4);   h_tr->GetYaxis()->SetTitleOffset(1.4);
  h_al->GetYaxis()->SetTitleOffset(1.4);   h_ar->GetYaxis()->SetTitleOffset(1.4);
  gStyle->SetOptStat("e");
  TCanvas* c = new TCanvas();
  h_pl->Draw("COL"); c->SaveAs("SiL_TM_p.png");
  h_pr->Draw("COL"); c->SaveAs("SiR_TM_p.png");
  h_dl->Draw("COL"); c->SaveAs("SiL_TM_d.png");
  h_dr->Draw("COL"); c->SaveAs("SiR_TM_d.png");
  h_tl->Draw("COL"); c->SaveAs("SiL_TM_t.png");
  h_tr->Draw("COL"); c->SaveAs("SiR_TM_t.png");
  h_al->Draw("COL"); c->SaveAs("SiL_TM_a.png");
  h_ar->Draw("COL"); c->SaveAs("SiR_TM_a.png");
}
