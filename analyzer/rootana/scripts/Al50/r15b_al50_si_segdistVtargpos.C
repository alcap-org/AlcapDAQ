// Looking at SiL1 and SiR1 distribution as a function of
// depth and lateral position of generated proton in the target.

{
  TFile* f[2] = {
    new TFile("data/pselal50.root"),
    new TFile("data/tmal50p_uniformintarg.root")
  };
  TTree* tr[2][2] = { // [RL,MC][L,R]
    {
      (TTree*)f[0]->Get("PID_LP"),
      (TTree*)f[0]->Get("PID_RP")
    }, {
      (TTree*)f[1]->Get("SiL_TransTree"),
      (TTree*)f[1]->Get("SiR_TransTree")
    }
  };
  TCut volcuts[11] = {
    "",
    "                x0 < -2.61",   // Left side of target
    "-2.61   < x0 && x0 < -0.87",
    "-0.87   < x0 && x0 < 0.87",
    "0.87    < x0 && x0 < 2.61",
    "2.61    < x0",                 // Right side of target
    "                z0 < -0.0015", // Front of target
    "-0.0015 < z0 && z0 < -0.0005",
    "-0.0005 < z0 && z0 < 0.0005",
    "0.0005  < z0 && z0 < 0.0015",
    "0.0015  < z0"                  // Back of target
  };
  // // Get all the number of entries for different cuts.
  // // Indexed by n[mc target volume cut][detector segment] with
  // // special indecies
  // // mctargvolcut == 0 means DATA, not MC
  // // detseg       == 0 means sum over all segments (only for that mctargvolcut)
  // // mctargvolcut in [1, 5] are lateral cuts, [6, 11] are depth cuts
  double nl[11][17] = {0}, nr[11][5] = {0};
  for (int j = 2; j <= 15; ++j) {
    TCut detsegcut = TString::Format("seg == %d", j).Data();
    nl[0][j] = tr[0][0]->Draw(">>evs", detsegcut);
    nl[0][0] += nl[0][j];
  }
  nr[0][1] = tr[0][1]->Draw(">>evs", "seg==4");
  nr[0][2] = tr[0][1]->Draw(">>evs", "seg==3");
  nr[0][3] = tr[0][1]->Draw(">>evs", "seg==2");
  nr[0][4] = tr[0][1]->Draw(">>evs", "seg==1");
  nr[0][0] = nr[0][1] + nr[0][2] + nr[0][3] + nr[0][4];
  for (int i = 1; i <= 10; ++i) {
    for (int j = 2; j <= 15; ++j) {
      TCut detsegcut = TString::Format("detseg == %d", j).Data();
      nl[i][j]       = tr[1][0]->Draw(">>evs", volcuts[i] && detsegcut);
      nl[i][0]      += nl[i][j];
    }
    nr[i][1] = tr[1][1]->Draw(">>evs", volcuts[i] && "detseg==1");
    nr[i][2] = tr[1][1]->Draw(">>evs", volcuts[i] && "detseg==4");
    nr[i][3] = tr[1][1]->Draw(">>evs", volcuts[i] && "detseg==3");
    nr[i][4] = tr[1][1]->Draw(">>evs", volcuts[i] && "detseg==2");
    nr[i][0] = nr[i][1] + nr[i][2] + nr[i][3] + nr[i][4];
  }
  TH1F* hl[11] = {
    new TH1F("hldata", "SiL Hit Distribution;Segment;Fraction of SiL Hits (%)",
             16, 0.5, 16.5),
    new TH1F("hlmclat1", "", 16, 0.5, 16.5),
    new TH1F("hlmclat2", "", 16, 0.5, 16.5),
    new TH1F("hlmclat3", "", 16, 0.5, 16.5),
    new TH1F("hlmclat4", "", 16, 0.5, 16.5),
    new TH1F("hlmclat5", "", 16, 0.5, 16.5),
    new TH1F("hlmcdep1", "", 16, 0.5, 16.5),
    new TH1F("hlmcdep2", "", 16, 0.5, 16.5),
    new TH1F("hlmcdep3", "", 16, 0.5, 16.5),
    new TH1F("hlmcdep4", "", 16, 0.5, 16.5),
    new TH1F("hlmcdep5", "", 16, 0.5, 16.5),
  };
  TH2F* hr[11] = {
    new TH2F("hrdata",   "SiR Hit Distribution;Quadrant;Quadrant",
             2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmclat1", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmclat2", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmclat3", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmclat4", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmclat5", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmcdep1", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmcdep2", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmcdep3", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmcdep4", "", 2, 0.5, 2.5, 2, 0.5, 2.5),
    new TH2F("hrmcdep5", "", 2, 0.5, 2.5, 2, 0.5, 2.5)
  };
  int col[11] = {
    TColor::GetColor((Float_t)0.0, (Float_t)0.8, (Float_t)0),
    TColor::GetColor((Float_t)1.0, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.8, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.6, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.4, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.2, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)1.0, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.8, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.6, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.4, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.2, (Float_t)0,   (Float_t)0)
  };
  for (int i = 0; i < 11; ++i) {
    for (int j = 2; j <= 15; ++j)
      hl[i]->SetBinContent(j, nl[i][j]);
    hr[i]->SetBinContent(1, 1, nr[i][1]);
    hr[i]->SetBinContent(1, 2, nr[i][2]);
    hr[i]->SetBinContent(2, 2, nr[i][3]);
    hr[i]->SetBinContent(2, 1, nr[i][4]);
    hl[i]->SetEntries(nl[i][0]);
    hl[i]->Scale(100/nl[i][0]);
    hr[i]->SetEntries(nr[i][0]);
    hr[i]->Scale(100/nr[i][0]);
    hl[i]->SetMinimum(5);
    hr[i]->SetMinimum(23);
    hr[i]->SetMaximum(27);
    hl[i]->SetLineColor(col[i]);
    hr[i]->SetLineColor(col[i]);
  }

  gStyle->SetOptStat(0);
  TText* txt[11][4] = {
    {
      new TText(0.9, 1.1, TString::Format("%.1f%%", 100*nr[0][1]/nr[0][0])),
      new TText(1.9, 1.1, TString::Format("%.1f%%", 100*nr[0][2]/nr[0][0])),
      new TText(1.9, 2.1, TString::Format("%.1f%%", 100*nr[0][3]/nr[0][0])),
      new TText(0.9, 2.1, TString::Format("%.1f%%", 100*nr[0][4]/nr[0][0]))
    }, {
      new TText(0.7, 1.0, TString::Format("%.1f%%", 100*nr[1][1]/nr[1][0])),
      new TText(1.7, 1.0, TString::Format("%.1f%%", 100*nr[1][2]/nr[1][0])),
      new TText(1.7, 2.0, TString::Format("%.1f%%", 100*nr[1][3]/nr[1][0])),
      new TText(0.7, 2.0, TString::Format("%.1f%%", 100*nr[1][4]/nr[1][0]))
    }, {
      new TText(0.9, 1.0, TString::Format("%.1f%%", 100*nr[2][1]/nr[2][0])),
      new TText(1.9, 1.0, TString::Format("%.1f%%", 100*nr[2][2]/nr[2][0])),
      new TText(1.9, 2.0, TString::Format("%.1f%%", 100*nr[2][3]/nr[2][0])),
      new TText(0.9, 2.0, TString::Format("%.1f%%", 100*nr[2][4]/nr[2][0]))
    }, {
      new TText(1.1, 1.0, TString::Format("%.1f%%", 100*nr[3][1]/nr[3][0])),
      new TText(2.1, 1.0, TString::Format("%.1f%%", 100*nr[3][2]/nr[3][0])),
      new TText(2.1, 2.0, TString::Format("%.1f%%", 100*nr[3][3]/nr[3][0])),
      new TText(1.1, 2.0, TString::Format("%.1f%%", 100*nr[3][4]/nr[3][0]))
    }, {
      new TText(0.8, 0.9, TString::Format("%.1f%%", 100*nr[4][1]/nr[4][0])),
      new TText(1.8, 0.9, TString::Format("%.1f%%", 100*nr[4][2]/nr[4][0])),
      new TText(1.8, 1.9, TString::Format("%.1f%%", 100*nr[4][3]/nr[4][0])),
      new TText(0.8, 1.9, TString::Format("%.1f%%", 100*nr[4][4]/nr[4][0]))
    }, {
      new TText(1.0, 0.9, TString::Format("%.1f%%", 100*nr[5][1]/nr[5][0])),
      new TText(2.0, 0.9, TString::Format("%.1f%%", 100*nr[5][2]/nr[5][0])),
      new TText(2.0, 1.9, TString::Format("%.1f%%", 100*nr[5][3]/nr[5][0])),
      new TText(1.0, 1.9, TString::Format("%.1f%%", 100*nr[5][4]/nr[5][0]))
    }, {
      new TText(0.7, 1.0, TString::Format("%.1f%%", 100*nr[6][1]/nr[6][0])),
      new TText(1.7, 1.0, TString::Format("%.1f%%", 100*nr[6][2]/nr[6][0])),
      new TText(1.7, 2.0, TString::Format("%.1f%%", 100*nr[6][3]/nr[6][0])),
      new TText(0.7, 2.0, TString::Format("%.1f%%", 100*nr[6][4]/nr[6][0]))
    }, {
      new TText(0.9, 1.0, TString::Format("%.1f%%", 100*nr[7][1]/nr[7][0])),
      new TText(1.9, 1.0, TString::Format("%.1f%%", 100*nr[7][2]/nr[7][0])),
      new TText(1.9, 2.0, TString::Format("%.1f%%", 100*nr[7][3]/nr[7][0])),
      new TText(0.9, 2.0, TString::Format("%.1f%%", 100*nr[7][4]/nr[7][0]))
    }, {
      new TText(1.1, 1.0, TString::Format("%.1f%%", 100*nr[8][1]/nr[8][0])),
      new TText(2.1, 1.0, TString::Format("%.1f%%", 100*nr[8][2]/nr[8][0])),
      new TText(2.1, 2.0, TString::Format("%.1f%%", 100*nr[8][3]/nr[8][0])),
      new TText(1.1, 2.0, TString::Format("%.1f%%", 100*nr[8][4]/nr[8][0]))
    }, {
      new TText(0.8, 0.9, TString::Format("%.1f%%", 100*nr[9][1]/nr[9][0])),
      new TText(1.8, 0.9, TString::Format("%.1f%%", 100*nr[9][2]/nr[9][0])),
      new TText(1.8, 1.9, TString::Format("%.1f%%", 100*nr[9][3]/nr[9][0])),
      new TText(0.8, 1.9, TString::Format("%.1f%%", 100*nr[9][4]/nr[9][0]))
    }, {
      new TText(1.0, 0.9, TString::Format("%.1f%%", 100*nr[10][1]/nr[10][0])),
      new TText(2.0, 0.9, TString::Format("%.1f%%", 100*nr[10][2]/nr[10][0])),
      new TText(2.0, 1.9, TString::Format("%.1f%%", 100*nr[10][3]/nr[10][0])),
      new TText(1.0, 1.9, TString::Format("%.1f%%", 100*nr[10][4]/nr[10][0]))
    }
  };
  for (int i = 0; i < 11; ++i)
    for (int j = 0; j < 4; ++j)
      txt[i][j]->SetTextColor(col[i]);
  // txt1[0]->SetTextColor(kRed);
  // txt2[0]->SetTextColor(kRed);
  // txt3[0]->SetTextColor(kRed);
  // txt4[0]->SetTextColor(kRed);

  TLegend* latleg = new TLegend(0.42, 0.6, 0.54, 0.8);
  TLegend* depleg = new TLegend(0.42, 0.1, 0.54, 0.3);
  latleg->AddEntry(hr[0],  "Data");
  latleg->AddEntry(hr[1],  "MC left side of target");
  latleg->AddEntry(hr[5],  "MC right side of target");
  depleg->AddEntry(hr[0],  "Data");
  depleg->AddEntry(hr[6],  "MC front of target");
  depleg->AddEntry(hr[10], "MC back of target");


  TCanvas* c = new TCanvas("c", "Detector Distribution", 1400, 1000);
  c->Divide(2, 2);
  c->cd(1);
  hl[0]->Draw("L");
  hl[1]->Draw("SAME");
  hl[2]->Draw("SAME");
  hl[3]->Draw("SAME");
  hl[4]->Draw("SAME");
  hl[5]->Draw("SAME");
  c->cd(2);
  hr[0]->Draw("BOX");
  hr[1]->Draw("BOX SAME");
  hr[2]->Draw("BOX SAME");
  hr[3]->Draw("BOX SAME");
  hr[4]->Draw("BOX SAME");
  hr[5]->Draw("BOX SAME");
  txt[0][0]->Draw(); txt[0][1]->Draw(); txt[0][2]->Draw(); txt[0][3]->Draw();
  txt[1][0]->Draw(); txt[1][1]->Draw(); txt[1][2]->Draw(); txt[1][3]->Draw();
  txt[2][0]->Draw(); txt[2][1]->Draw(); txt[2][2]->Draw(); txt[2][3]->Draw();
  txt[3][0]->Draw(); txt[3][1]->Draw(); txt[3][2]->Draw(); txt[3][3]->Draw();
  txt[4][0]->Draw(); txt[4][1]->Draw(); txt[4][2]->Draw(); txt[4][3]->Draw();
  txt[5][0]->Draw(); txt[5][1]->Draw(); txt[5][2]->Draw(); txt[5][3]->Draw();
  c->cd(3);
  hl[0] ->Draw("L");
  hl[6] ->Draw("SAME");
  hl[7] ->Draw("SAME");
  hl[8] ->Draw("SAME");
  hl[9] ->Draw("SAME");
  hl[10]->Draw("SAME");
  c->cd(4);
  hr[0] ->Draw("BOX");
  hr[6] ->Draw("BOX SAME");
  hr[7] ->Draw("BOX SAME");
  hr[8] ->Draw("BOX SAME");
  hr[9] ->Draw("BOX SAME");
  hr[10]->Draw("BOX SAME");
  txt[0][0] ->Draw(); txt[0][1] ->Draw(); txt[0][2] ->Draw(); txt[0][3] ->Draw();
  txt[6][0] ->Draw(); txt[6][1] ->Draw(); txt[6][2] ->Draw(); txt[6][3] ->Draw();
  txt[7][0] ->Draw(); txt[7][1] ->Draw(); txt[7][2] ->Draw(); txt[7][3] ->Draw();
  txt[8][0] ->Draw(); txt[8][1] ->Draw(); txt[8][2] ->Draw(); txt[8][3] ->Draw();
  txt[9][0] ->Draw(); txt[9][1] ->Draw(); txt[9][2] ->Draw(); txt[9][3] ->Draw();
  txt[10][0]->Draw(); txt[10][1]->Draw(); txt[10][2]->Draw(); txt[10][3]->Draw();

  // Draw legend by overlaying an invisible pad
  c->cd();
  TPad* legpad = new TPad("legpad", "", 0, 0, 1, 1);
  legpad->SetFillStyle(4000);
  legpad->Draw();
  legpad->cd();
  latleg->Draw();
  depleg->Draw();
  c->SaveAs("img/si_segdistVtargpos.png");
}
