// Reproducing elog:Analysis+R2015b/216 for Al50 MC
{
  TFile* ifile = new TFile("data/mc_pfrommudist.root");

  TH2* h_all    = new TH2F("h_all",    "SiR All hits;E_{1}+E_{2};E_{1}",  2000, 0, 20e3, 300, 0, 3e3);
  TH2* h_r3veto = new TH2F("h_r3veto", "SiR E_{3} = 0;E_{1}+E_{2};E_{1}", 2000, 0, 20e3, 300, 0, 3e3);
  PID_R->Draw("e1:(e1+e2)>>+h_all",    "e2 > 0",            "goff");
  PID_R->Draw("e1:(e1+e2)>>+h_r3veto", "e2 > 0 && e3 == 0", "goff");

  int n_all = 0, n_veto = 0;
  TCut ecut("0 < e1 && e1 < 300 && 9e3 < (e1+e2) && (e1+e2) < 11e3");
  n_all  += PID_R->Draw(">>evts", ecut);
  n_veto += PID_R->Draw(">>evts", ecut && "0 == e3");

  TBox* box = new TBox(9000, 0, 11000, 300);
  box->SetLineColor(kRed);
  box->SetLineWidth(3);
  box->SetLineStyle(1);
  box->SetFillStyle(0);
  char str[32];
  sprintf(str, "%d entries", n_all);
  TText* txt_all = new TText(7000, 400, str);
  sprintf(str, "%d entries", n_veto);
  TText* txt_veto = new TText(7000, 400, str);
  txt_all ->SetTextColor(kRed);
  txt_veto->SetTextColor(kRed);
  txt_all ->SetTextAlign(22);
  txt_veto->SetTextAlign(22);

  TCanvas* c_all = new TCanvas();
  h_all  ->Draw("COLZ");
  box    ->Draw();
  txt_all->Draw();
  c_all->SaveAs("img/sir3_ineff_deve_all_mc.png");
  TCanvas* c_veto = new TCanvas();
  h_r3veto->Draw("COLZ");
  box     ->Draw();
  txt_veto->Draw();
  c_veto->SaveAs("img/sir3_ineff_deve_veto_mc.png");

  TH1* h_raw    = new TH1F("h_raw",    "SiR Raw Proton;E",    200, 0, 20e3);
  TH1* h_scaled = new TH1F("h_scaled", "SiR Scaled Proton;E", 200, 0, 20e3);
  PID_R->Draw("e>>+h_raw",    "",       "goff");
  PID_R->Draw("e>>+h_scaled", "e3 > 0 && e > 15.5e3", "goff");
  h_scaled->Scale(1./(1.-n_veto/(double)n_all));
  PID_R->Draw("e>>+h_scaled", "e3 == 0", "goff");

  TCanvas* c_raw = new TCanvas();
  h_raw->Draw();
  c_raw->SaveAs("img/sir3_ineff_raw_prot_mc.png");
  TCanvas* c_scaled = new TCanvas();
  h_scaled->Draw();
  c_scaled->SaveAs("img/sir3_ineff_corr_prot_mc.png");

  std::cout << "SiR3 relative efficiency: " << 1.-n_veto/(double)n_all << std::endl;
}
