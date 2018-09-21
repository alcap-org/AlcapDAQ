// Compare TM from different sections of the target.

{
  TFile* f = new TFile("data/unfold_targsec.root", "READ");

  TH1* h1data[2] = {
    (TH1*)f->Get("h1ldata"), (TH1*)f->Get("h1rdata")
  };
  TH1* h1latpos[2][5] = {
    {
      (TH1*)f->Get("h1llatpos_1"), (TH1*)f->Get("h1llatpos_2"),
      (TH1*)f->Get("h1llatpos_3"), (TH1*)f->Get("h1llatpos_4"),
      (TH1*)f->Get("h1llatpos_5")
    },
    {
      (TH1*)f->Get("h1rlatpos_1"), (TH1*)f->Get("h1rlatpos_2"),
      (TH1*)f->Get("h1rlatpos_3"), (TH1*)f->Get("h1rlatpos_4"),
      (TH1*)f->Get("h1rlatpos_5")
    }
  };
  TH1* h1deppos[2][5] = {
    {
      (TH1*)f->Get("h1ldeppos_1"), (TH1*)f->Get("h1ldeppos_2"),
      (TH1*)f->Get("h1ldeppos_3"), (TH1*)f->Get("h1ldeppos_4"),
      (TH1*)f->Get("h1ldeppos_5")
    },
    {
      (TH1*)f->Get("h1rdeppos_1"), (TH1*)f->Get("h1rdeppos_2"),
      (TH1*)f->Get("h1rdeppos_3"), (TH1*)f->Get("h1rdeppos_4"),
      (TH1*)f->Get("h1rdeppos_5")
    }
  };
  TH2* h2latpos[2][5] = {
    {
      (TH2*)f->Get("h2llatpos_1"), (TH2*)f->Get("h2llatpos_2"),
      (TH2*)f->Get("h2llatpos_3"), (TH2*)f->Get("h2llatpos_4"),
      (TH2*)f->Get("h2llatpos_5")
    },
    {
      (TH2*)f->Get("h2rlatpos_1"), (TH2*)f->Get("h2rlatpos_2"),
      (TH2*)f->Get("h2rlatpos_3"), (TH2*)f->Get("h2rlatpos_4"),
      (TH2*)f->Get("h2rlatpos_5")
    }
  };
  TH2* h2deppos[2][5] = {
    {
      (TH2*)f->Get("h2ldeppos_1"), (TH2*)f->Get("h2ldeppos_2"),
      (TH2*)f->Get("h2ldeppos_3"), (TH2*)f->Get("h2ldeppos_4"),
      (TH2*)f->Get("h2ldeppos_5")
    },
    {
      (TH2*)f->Get("h2rdeppos_1"), (TH2*)f->Get("h2rdeppos_2"),
      (TH2*)f->Get("h2rdeppos_3"), (TH2*)f->Get("h2rdeppos_4"),
      (TH2*)f->Get("h2rdeppos_5")
    }
  };

  RooUnfoldResponse* latposresp[2][5] = {
    {
      new RooUnfoldResponse(nullptr, h1latpos[0][0], h2latpos[0][0]),
      new RooUnfoldResponse(nullptr, h1latpos[0][1], h2latpos[0][1]),
      new RooUnfoldResponse(nullptr, h1latpos[0][2], h2latpos[0][2]),
      new RooUnfoldResponse(nullptr, h1latpos[0][3], h2latpos[0][3]),
      new RooUnfoldResponse(nullptr, h1latpos[0][4], h2latpos[0][4])
    },
    {
      new RooUnfoldResponse(nullptr, h1latpos[1][0], h2latpos[1][0]),
      new RooUnfoldResponse(nullptr, h1latpos[1][1], h2latpos[1][1]),
      new RooUnfoldResponse(nullptr, h1latpos[1][2], h2latpos[1][2]),
      new RooUnfoldResponse(nullptr, h1latpos[1][3], h2latpos[1][3]),
      new RooUnfoldResponse(nullptr, h1latpos[1][4], h2latpos[1][4])
    }
  };
  RooUnfoldResponse* depposresp[2][5] = {
    {
      new RooUnfoldResponse(nullptr, h1deppos[0][0], h2deppos[0][0]),
      new RooUnfoldResponse(nullptr, h1deppos[0][1], h2deppos[0][1]),
      new RooUnfoldResponse(nullptr, h1deppos[0][2], h2deppos[0][2]),
      new RooUnfoldResponse(nullptr, h1deppos[0][3], h2deppos[0][3]),
      new RooUnfoldResponse(nullptr, h1deppos[0][4], h2deppos[0][4])
    },
    {
      new RooUnfoldResponse(nullptr, h1deppos[1][0], h2deppos[1][0]),
      new RooUnfoldResponse(nullptr, h1deppos[1][1], h2deppos[1][1]),
      new RooUnfoldResponse(nullptr, h1deppos[1][2], h2deppos[1][2]),
      new RooUnfoldResponse(nullptr, h1deppos[1][3], h2deppos[1][3]),
      new RooUnfoldResponse(nullptr, h1deppos[1][4], h2deppos[1][4])
    }
  };
  RooUnfoldBayes* latposunf[2][5] = {
    {
      new RooUnfoldBayes(latposresp[0][0], h1data[0], 4),
      new RooUnfoldBayes(latposresp[0][1], h1data[0], 4),
      new RooUnfoldBayes(latposresp[0][2], h1data[0], 4),
      new RooUnfoldBayes(latposresp[0][3], h1data[0], 4),
      new RooUnfoldBayes(latposresp[0][4], h1data[0], 4)
    },
    {
      new RooUnfoldBayes(latposresp[1][0], h1data[1], 4),
      new RooUnfoldBayes(latposresp[1][1], h1data[1], 4),
      new RooUnfoldBayes(latposresp[1][2], h1data[1], 4),
      new RooUnfoldBayes(latposresp[1][3], h1data[1], 4),
      new RooUnfoldBayes(latposresp[1][4], h1data[1], 4)
    }
  };
  RooUnfoldBayes* depposunf[2][5] = {
    {
      new RooUnfoldBayes(depposresp[0][0], h1data[0], 4),
      new RooUnfoldBayes(depposresp[0][1], h1data[0], 4),
      new RooUnfoldBayes(depposresp[0][2], h1data[0], 4),
      new RooUnfoldBayes(depposresp[0][3], h1data[0], 4),
      new RooUnfoldBayes(depposresp[0][4], h1data[0], 4)
    },
    {
      new RooUnfoldBayes(depposresp[1][0], h1data[1], 4),
      new RooUnfoldBayes(depposresp[1][1], h1data[1], 4),
      new RooUnfoldBayes(depposresp[1][2], h1data[1], 4),
      new RooUnfoldBayes(depposresp[1][3], h1data[1], 4),
      new RooUnfoldBayes(depposresp[1][4], h1data[1], 4)
    }
  };
  TH1* latposen[3][5] = {
    {
      latposunf[0][0]->Hreco(), latposunf[0][1]->Hreco(),
      latposunf[0][2]->Hreco(), latposunf[0][3]->Hreco(),
      latposunf[0][4]->Hreco()
    },
    {
      latposunf[1][0]->Hreco(), latposunf[1][1]->Hreco(),
      latposunf[1][2]->Hreco(), latposunf[1][3]->Hreco(),
      latposunf[1][4]->Hreco()
    },
    {
      nullptr, nullptr, nullptr, nullptr, nullptr
    }
  };
  TH1* depposen[3][5] = {
    {
      depposunf[0][0]->Hreco(), depposunf[0][1]->Hreco(),
      depposunf[0][2]->Hreco(), depposunf[0][3]->Hreco(),
      depposunf[0][4]->Hreco()
    },
    {
      depposunf[1][0]->Hreco(), depposunf[1][1]->Hreco(),
      depposunf[1][2]->Hreco(), depposunf[1][3]->Hreco(),
      depposunf[1][4]->Hreco()
    },
    {
      nullptr, nullptr, nullptr, nullptr, nullptr
    }
  };
  for (int i = 0; i < 5; ++i) {
    char hlatname[32], hdepname[32];
    sprintf(hlatname, "latavg%d", i);
    sprintf(hdepname, "depavg%d", i);
    latposen[2][i] = (TH1*)latposen[0][i]->Clone(hlatname);
    depposen[2][i] = (TH1*)depposen[0][i]->Clone(hdepname);
    latposen[2][i]->Add(latposen[1][i]);
    depposen[2][i]->Add(depposen[1][i]);
    latposen[2][i]->Scale(0.5);
    depposen[2][i]->Scale(0.5);
  }
  int col[5] = {
    TColor::GetColor((Float_t)1.0, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.8, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.6, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.4, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.2, (Float_t)0,   (Float_t)0)
  };
  latposen[0][0]->SetLineColor(col[0]); latposen[1][0]->SetLineColor(col[0]); latposen[2][0]->SetLineColor(col[0]);
  latposen[0][1]->SetLineColor(col[1]); latposen[1][1]->SetLineColor(col[1]); latposen[2][1]->SetLineColor(col[1]);
  latposen[0][2]->SetLineColor(col[2]); latposen[1][2]->SetLineColor(col[2]); latposen[2][2]->SetLineColor(col[2]);
  latposen[0][3]->SetLineColor(col[3]); latposen[1][3]->SetLineColor(col[3]); latposen[2][3]->SetLineColor(col[3]);
  latposen[0][4]->SetLineColor(col[4]); latposen[1][4]->SetLineColor(col[4]); latposen[2][4]->SetLineColor(col[4]);
  depposen[0][0]->SetLineColor(col[0]); depposen[1][0]->SetLineColor(col[0]); depposen[2][0]->SetLineColor(col[0]);
  depposen[0][1]->SetLineColor(col[1]); depposen[1][1]->SetLineColor(col[1]); depposen[2][1]->SetLineColor(col[1]);
  depposen[0][2]->SetLineColor(col[2]); depposen[1][2]->SetLineColor(col[2]); depposen[2][2]->SetLineColor(col[2]);
  depposen[0][3]->SetLineColor(col[3]); depposen[1][3]->SetLineColor(col[3]); depposen[2][3]->SetLineColor(col[3]);
  depposen[0][4]->SetLineColor(col[4]); depposen[1][4]->SetLineColor(col[4]); depposen[2][4]->SetLineColor(col[4]);

  latposen[0][0]->SetTitle("Transfer Matrix from Laterally Sliced Target (SiL);Energy [keV]");
  latposen[1][4]->SetTitle("Transfer Matrix from Laterally Sliced Target (SiR);Energy [keV]");
  latposen[2][0]->SetTitle("Transfer Matrix from Laterally Sliced Target (Avg);Energy [keV]");
  depposen[0][0]->SetTitle("Transfer Matrix from Depth Sliced Target (SiL);Energy [keV]");
  depposen[1][0]->SetTitle("Transfer Matrix from Depth Sliced Target (SiR);Energy [keV]");
  depposen[2][4]->SetTitle("Transfer Matrix from Depth Sliced Target (Avg);Energy [keV]");

  h1data[0]->Scale(200.e3/h1data[0]->GetMaximum());
  h1data[1]->Scale(200.e3/h1data[1]->GetMaximum());
  depposen[1][0]->SetMaximum(400e3);

  TLegend* latleg = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* depleg = new TLegend(0.7, 0.7, 0.9, 0.9);
  latleg->AddEntry(h1data[0],      "Raw (Arb. Scale)",     "l");
  latleg->AddEntry(latposen[0][0], "Left Side of Target",  "l");
  latleg->AddEntry(latposen[0][4], "Right Side of Target", "l");
  depleg->AddEntry(h1data[0],      "Raw (Arb. Scale)",     "l");
  depleg->AddEntry(depposen[0][0], "Front of Target",      "l");
  depleg->AddEntry(depposen[0][4], "Back of Target",       "l");

  gStyle->SetOptStat(0);
  TCanvas* cen = new TCanvas("cen", "Unfolded Energies", 2100, 1000);
  cen->Divide(3, 2);
  cen->cd(1);
  latposen[0][0]->Draw("SAME");
  latposen[0][1]->Draw("SAME");
  latposen[0][2]->Draw("SAME");
  latposen[0][3]->Draw("SAME");
  latposen[0][4]->Draw("SAME");
  h1data  [0]   ->Draw("SAME");
  latleg->Draw();
  cen->cd(2);
  latposen[1][4]->Draw("SAME");
  latposen[1][3]->Draw("SAME");
  latposen[1][2]->Draw("SAME");
  latposen[1][1]->Draw("SAME");
  latposen[1][0]->Draw("SAME");
  h1data  [1]   ->Draw("SAME");
  latleg->Draw();
  cen->cd(3);
  latposen[2][0]->Draw("SAME");
  latposen[2][1]->Draw("SAME");
  latposen[2][2]->Draw("SAME");
  latposen[2][3]->Draw("SAME");
  latposen[2][4]->Draw("SAME");
  latleg->Draw();
  cen->cd(4);
  depposen[0][0]->Draw("SAME");
  depposen[0][1]->Draw("SAME");
  depposen[0][2]->Draw("SAME");
  depposen[0][3]->Draw("SAME");
  depposen[0][4]->Draw("SAME");
  h1data  [0]   ->Draw("SAME");
  depleg->Draw();
  cen->cd(5);
  depposen[1][0]->Draw("SAME");
  depposen[1][1]->Draw("SAME");
  depposen[1][2]->Draw("SAME");
  depposen[1][3]->Draw("SAME");
  depposen[1][4]->Draw("SAME");
  h1data  [1]   ->Draw("SAME");
  depleg->Draw();
  cen->cd(6);
  depposen[2][4]->Draw("SAME");
  depposen[2][3]->Draw("SAME");
  depposen[2][2]->Draw("SAME");
  depposen[2][1]->Draw("SAME");
  depposen[2][0]->Draw("SAME");
  depleg->Draw();
  cen->SaveAs("img/unfold_targsec_energies.png");

  TCanvas* cltmlat = new TCanvas("cltmlat", "Left Lateral Tranfer Matrices", 2100, 1000);
  cltmlat->Divide(3, 2);
  cltmlat->cd(1);
  h2latpos[0][0]->Draw("COL");
  cltmlat->cd(2);
  h2latpos[0][1]->Draw("COL");
  cltmlat->cd(3);
  h2latpos[0][2]->Draw("COL");
  cltmlat->cd(4);
  h2latpos[0][3]->Draw("COL");
  cltmlat->cd(5);
  h2latpos[0][4]->Draw("COL");
  cltmlat->SaveAs("img/unfold_targsec_tm_l_lat.png");
  TCanvas* crtmlat = new TCanvas("crtmlat", "Right Lateral Tranfer Matrices", 2100, 1000);
  crtmlat->Divide(3, 2);
  crtmlat->cd(1);
  h2latpos[1][0]->Draw("COL");
  crtmlat->cd(2);
  h2latpos[1][1]->Draw("COL");
  crtmlat->cd(3);
  h2latpos[1][2]->Draw("COL");
  crtmlat->cd(4);
  h2latpos[1][3]->Draw("COL");
  crtmlat->cd(5);
  h2latpos[1][4]->Draw("COL");
  crtmlat->SaveAs("img/unfold_targsec_tm_r_lat.png");
  TCanvas* cltmdep = new TCanvas("cltmdep", "Left Depth Tranfer Matrices", 2100, 1000);
  cltmdep->Divide(3, 2);
  cltmdep->cd(1);
  h2deppos[0][0]->Draw("COL");
  cltmdep->cd(2);
  h2deppos[0][1]->Draw("COL");
  cltmdep->cd(3);
  h2deppos[0][2]->Draw("COL");
  cltmdep->cd(4);
  h2deppos[0][3]->Draw("COL");
  cltmdep->cd(5);
  h2deppos[0][4]->Draw("COL");
  cltmdep->SaveAs("img/unfold_targsec_tm_l_dep.png");
  TCanvas* crtmdep = new TCanvas("crtmdep", "Right Depth Tranfer Matrices", 2100, 1000);
  crtmdep->Divide(3, 2);
  crtmdep->cd(1);
  h2deppos[1][0]->Draw("COL");
  crtmdep->cd(2);
  h2deppos[1][1]->Draw("COL");
  crtmdep->cd(3);
  h2deppos[1][2]->Draw("COL");
  crtmdep->cd(4);
  h2deppos[1][3]->Draw("COL");
  crtmdep->cd(5);
  h2deppos[1][4]->Draw("COL");
  crtmdep->SaveAs("img/unfold_targsec_tm_r_dep.png");
}
