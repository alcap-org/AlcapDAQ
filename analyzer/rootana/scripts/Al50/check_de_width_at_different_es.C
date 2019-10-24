{
  TFile* f = new TFile("data/Al50/psel.3layer.root");
  TTree* tr[2] = { (TTree*)f->Get("PID_LP"), (TTree*)f->Get("PID_RP") };
  TH1* h[2][16];
  int col[16] = { 2, 3, 4, 5, 6, 7, 8, 9, 41, 42, 43, 44, 45, 46, 47, 48 };
  for (int i = 0; i < 16; ++i) {
    int elo = 2000 + 500 * i;
    char name[128], title[128], cmd[128], cut[128];
    sprintf(name,  "hl%d",    elo);
    sprintf(title, "hl%d",    elo);
    sprintf(cmd,   "de>>+%s", name);
    sprintf(cut,   "%d < e && e < %d && e3 == 0", elo, elo + 500);
    h[0][i] = new TH1D(name, title, 150, 0, 3e3);
    tr[0]->Draw(cmd, cut, "goff");
    h[0][i]->SetLineColor(col[i]);
    h[0][i]->SetTitle("Left;dE [keV];/500 keV");
    sprintf(name,  "hr%d", elo);
    sprintf(title, "hr%d", elo);
    sprintf(cmd,   "de>>+%s", name);
    h[1][i] = new TH1D(name, title, 150, 0, 3e3);
    tr[1]->Draw(cmd, cut, "goff");
    h[1][i]->SetLineColor(col[i]);
    h[1][i]->SetTitle("Right;dE [keV];/500 keV");
  }

  TLegend* leg = new TLegend(0.6, 0.3, 0.9, 0.9);
  leg->SetHeader("E Slice");
  leg->AddEntry(h[0][0],  "2-2.5 MeV");
  leg->AddEntry(h[0][1],  "2.5-3 MeV");
  leg->AddEntry(h[0][2],  "3-3.5 MeV");
  leg->AddEntry(h[0][3],  "3.5-4 MeV");
  leg->AddEntry(h[0][4],  "4-4.5 MeV");
  leg->AddEntry(h[0][5],  "4.5-5 MeV");
  leg->AddEntry(h[0][6],  "5-5.5 MeV");
  leg->AddEntry(h[0][7],  "5.5-6 MeV");
  leg->AddEntry(h[0][8],  "6-6.5 MeV");
  leg->AddEntry(h[0][9],  "6.5-7 MeV");
  leg->AddEntry(h[0][10], "7-7.5 MeV");
  leg->AddEntry(h[0][11], "7.5-8 MeV");
  leg->AddEntry(h[0][12], "8-8.5 MeV");
  leg->AddEntry(h[0][13], "8.5-9 MeV");
  leg->AddEntry(h[0][14], "9-9.5 MeV");
  leg->AddEntry(h[0][15], "9.5-10 MeV");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  h[0][0]->SetMaximum(400);
  h[1][0]->SetMaximum(400);
  c->Divide(2);
  for (int i = 0; i < 16; ++i) {
    c->cd(1);
    h[0][i]->Draw("SAME");
    c->cd(2);
    h[1][i]->Draw("SAME");
  }
  c->cd(1);
  leg->Draw();
  c->cd(2);
  leg->Draw();
  c->SaveAs("img/check_de_width_at_different_es.png");
}
