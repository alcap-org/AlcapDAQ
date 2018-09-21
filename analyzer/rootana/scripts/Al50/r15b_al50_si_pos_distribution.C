// Look at the hit position in SiL1 and SiR1. Needs to be updated since
// PID_L#P are all put into one tree, PID_LP, whith the segment # being replaced
// by the branch seg.

{
  TFile* f[2] = {
    new TFile("data/tmal50p.root"),
    new TFile("data/pselal50.root")
  };
  TTree* tr[2] = {
    (TTree*)f[0]->Get("SiL_TransTree"),
    (TTree*)f[0]->Get("SiR_TransTree")
  };
  tr[0]->Draw("y0:x0>>htargl");
  tr[1]->Draw("y0:x0>>htargr");
  TH2* htarg[2] = {
    (TH2*)gDirectory->Get("hltarg"),
    (TH2*)gDirectory->Get("hrtargr")
  };
  double nl[2][17] = {
    { 0,
      0,
      (double)tr[0]->Draw(">>evs", "detseg==2"),
      (double)tr[0]->Draw(">>evs", "detseg==3"),
      (double)tr[0]->Draw(">>evs", "detseg==4"),
      (double)tr[0]->Draw(">>evs", "detseg==5"),
      (double)tr[0]->Draw(">>evs", "detseg==6"),
      (double)tr[0]->Draw(">>evs", "detseg==7"),
      (double)tr[0]->Draw(">>evs", "detseg==8"),
      (double)tr[0]->Draw(">>evs", "detseg==9"),
      (double)tr[0]->Draw(">>evs", "detseg==10"),
      (double)tr[0]->Draw(">>evs", "detseg==11"),
      (double)tr[0]->Draw(">>evs", "detseg==12"),
      (double)tr[0]->Draw(">>evs", "detseg==13"),
      (double)tr[0]->Draw(">>evs", "detseg==14"),
      (double)tr[0]->Draw(">>evs", "detseg==15"),
      0
    }, {
      0,
      0,
      (double)((TTree*)f[1]->Get("PID_L2P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L3P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L4P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L5P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L6P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L7P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L8P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L9P")) ->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L10P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L11P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L12P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L13P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L14P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_L15P"))->GetEntries(),
      0
    }
  };

  double nr[2][5] = {
    { 0,
      (double)tr[1]->Draw(">>evs", "detseg==1"),
      (double)tr[1]->Draw(">>evs", "detseg==2"),
      (double)tr[1]->Draw(">>evs", "detseg==3"),
      (double)tr[1]->Draw(">>evs", "detseg==4")
    }, {
      0,
      (double)((TTree*)f[1]->Get("PID_R1P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_R2P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_R3P"))->GetEntries(),
      (double)((TTree*)f[1]->Get("PID_R4P"))->GetEntries()
    }
  };

  TH1* hl[2] = {
    new TH1F("hlmc",   "SiL Hit Distribution;Segment;Fraction of SiL Hits (%)",
             16, 0.5, 16.5),
    new TH1F("hldata", "SiL Hit Distribution;Segment;Fraction of SiL Hits (%)",
             16, 0.5, 16.5)
  };
  TH2* hr[2] = {
    new TH2F("hrmc",   "SiR Hit Distribution;Quadrant;Quadrant",
             2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrdata", "SiR Hit Distribution;Quadrant;Quadrant",
             2, 0.5, 2.5, 2, 0.5, 2.5)
  };
  int col[2] = { kRed, kBlack };
  for (int i = 0; i < 2; ++i) {
    for (int j = 1; j <= 16; ++j) {
      nl[i][0] += nl[i][j];
      hl[i]->SetBinContent(j, nl[i][j]);
    }
    nr[i][0] += nr[i][1] + nr[i][2] + nr[i][3] + nr[i][4];
    hr[i]->SetBinContent(1, 1, nr[i][1]);
    hr[i]->SetBinContent(1, 2, nr[i][2]);
    hr[i]->SetBinContent(2, 2, nr[i][3]);
    hr[i]->SetBinContent(2, 1, nr[i][4]);
    hl[i]->SetEntries(nl[i][0]);
    hl[i]->Scale(100/nl[i][0]);
    hr[i]->SetEntries(nr[i][0]);
    hr[i]->Scale(100/nr[i][0]);
    hl[i]->SetMinimum(0);
    hr[i]->SetMinimum(0);
    hr[i]->SetMaximum(100);
    hl[i]->SetLineColor(col[i]);
    hr[i]->SetLineColor(col[i]);
  }

  gStyle->SetOptStat(0);

  TText* txt1[2] = {
    new TText(0.75, 1, TString::Format("%.1f%%", 100*nr[0][1]/nr[0][0])),
    new TText(1,    1, TString::Format("%.1f%%", 100*nr[1][1]/nr[1][0]))
  };
  TText* txt2[2] = {
    new TText(0.75, 2, TString::Format("%.1f%%", 100*nr[0][2]/nr[0][0])),
    new TText(1,    2, TString::Format("%.1f%%", 100*nr[1][2]/nr[1][0]))
  };
  TText* txt3[2] = {
    new TText(1.75, 2, TString::Format("%.1f%%", 100*nr[0][3]/nr[0][0])),
    new TText(2,    2, TString::Format("%.1f%%", 100*nr[1][3]/nr[1][0]))
  };
  TText* txt4[2] = {
    new TText(1.75, 1, TString::Format("%.1f%%", 100*nr[0][4]/nr[0][0])),
    new TText(2,    1, TString::Format("%.1f%%", 100*nr[1][4]/nr[1][0]))
  };
  txt1[0]->SetTextColor(kRed);
  txt2[0]->SetTextColor(kRed);
  txt3[0]->SetTextColor(kRed);
  txt4[0]->SetTextColor(kRed);

  TLegend* leg = new TLegend(0.7, 0.1, 0.9, 0.3);
  leg->AddEntry(hr[0], "MC");
  leg->AddEntry(hr[1], "Data");

  TCanvas* c = new TCanvas("c", "Detector Distribution", 1400, 500);
  c->Divide(2);
  c->cd(1); hl[1]->Draw();      hl[0]->Draw("SAME");     leg->Draw();
  c->cd(2); hr[1]->Draw("BOX"); hr[0]->Draw("BOX SAME");
  txt1[0]->Draw(); txt2[0]->Draw(); txt3[0]->Draw(); txt4[0]->Draw();
  txt1[1]->Draw(); txt2[1]->Draw(); txt3[1]->Draw(); txt4[1]->Draw();
  c->SaveAs("img/si_pos_dist.png");
}
