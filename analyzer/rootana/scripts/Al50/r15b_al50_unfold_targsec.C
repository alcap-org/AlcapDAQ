// Compare TM from different sections of the target.

#include "util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TStyle.h"

#include <stdio.h>
#include <utility>

void r15b_al50_unfold_targsec() {
  TFile* f = new TFile("data/Al50/unfold_targsec.root", "READ");
  TFile* fcanon = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root");


  //////////////////////////////////////////////////////////////////////////////
  // Get all the histograms from the files
  //////////////////////////////////////////////////////////////////////////////
  TH1* h1data[2] = {
    (TH1*)f->Get("h1ldata"), (TH1*)f->Get("h1rdata")
  };
  TH1* h1canon[3] = {
    (TH1*)fcanon->Get("hl_e_reco_bay"), (TH1*)fcanon->Get("hr_e_reco_bay"),
    nullptr
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


  //////////////////////////////////////////////////////////////////////////////
  // Unfold everything
  //////////////////////////////////////////////////////////////////////////////
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
  // Make the L/R average histograms
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
  h1canon[2] = (TH1*)h1canon[0]->Clone("canonavg");
  h1canon[2]->Add(h1canon[1]);
  h1canon[2]->Scale(0.5);


  //////////////////////////////////////////////////////////////////////////////
  // Calculate normalization and scale
  //////////////////////////////////////////////////////////////////////////////
  double nmu_hi = 137e6;  // All Al50
  double caprate = 0.609;
  TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  ftime->SetParameters(52.7, 864.);
  double tcuteff = ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6); // Al50
  double norm = 1./(nmu_hi*caprate*tcuteff);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      latposen[i][j]->Scale(norm);
      depposen[i][j]->Scale(norm);
    }
    h1canon[i]->Scale(norm);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Determine the rates, or at least just gross numbers at this point
  //////////////////////////////////////////////////////////////////////////////
  std::pair<double,double> nlatpos35[3][5] = {
    {
      HistUtils::IntegralByUserLimits(latposen[0][0], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[0][1], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[0][2], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[0][3], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[0][4], 3.5e3, 10e3)
    },
    {
      HistUtils::IntegralByUserLimits(latposen[1][0], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[1][1], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[1][2], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[1][3], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[1][4], 3.5e3, 10e3)
    },
    {
      HistUtils::IntegralByUserLimits(latposen[2][0], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[2][1], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[2][2], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[2][3], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(latposen[2][4], 3.5e3, 10e3)
    }
  };
  std::pair<double,double> ndeppos35[3][5] = {
    {
      HistUtils::IntegralByUserLimits(depposen[0][0], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[0][1], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[0][2], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[0][3], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[0][4], 3.5e3, 10e3)
    },
    {
      HistUtils::IntegralByUserLimits(depposen[1][0], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[1][1], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[1][2], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[1][3], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[1][4], 3.5e3, 10e3)
    },
    {
      HistUtils::IntegralByUserLimits(depposen[2][0], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[2][1], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[2][2], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[2][3], 3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(depposen[2][4], 3.5e3, 10e3)
    }
  };
  std::pair<double,double> ncanon35[3] = {
    HistUtils::IntegralByUserLimits(h1canon[0], 3.5e3, 10e3),
    HistUtils::IntegralByUserLimits(h1canon[1], 3.5e3, 10e3),
    HistUtils::IntegralByUserLimits(h1canon[2], 3.5e3, 10e3),
  };
  std::pair<double,double> nlatpos40[3][5] = {
    {
      HistUtils::IntegralByUserLimits(latposen[0][0], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[0][1], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[0][2], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[0][3], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[0][4], 4e3, 8e3)
    },
    {
      HistUtils::IntegralByUserLimits(latposen[1][0], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[1][1], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[1][2], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[1][3], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[1][4], 4e3, 8e3)
    },
    {
      HistUtils::IntegralByUserLimits(latposen[2][0], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[2][1], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[2][2], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[2][3], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(latposen[2][4], 4e3, 8e3)
    }
  };
  std::pair<double,double> ndeppos40[3][5] = {
    {
      HistUtils::IntegralByUserLimits(depposen[0][0], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[0][1], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[0][2], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[0][3], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[0][4], 4e3, 8e3)
    },
    {
      HistUtils::IntegralByUserLimits(depposen[1][0], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[1][1], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[1][2], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[1][3], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[1][4], 4e3, 8e3)
    },
    {
      HistUtils::IntegralByUserLimits(depposen[2][0], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[2][1], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[2][2], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[2][3], 4e3, 8e3),
      HistUtils::IntegralByUserLimits(depposen[2][4], 4e3, 8e3)
    }
  };
  std::pair<double,double> ncanon40[3] = {
    HistUtils::IntegralByUserLimits(h1canon[0], 4e3, 8e3),
    HistUtils::IntegralByUserLimits(h1canon[1], 4e3, 8e3),
    HistUtils::IntegralByUserLimits(h1canon[2], 4e3, 8e3),
  };


  //////////////////////////////////////////////////////////////////////////////
  // Start prepping everything to draw
  //////////////////////////////////////////////////////////////////////////////
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

  // h1data[0]->Scale(200.e3/h1data[0]->GetMaximum());
  // h1data[1]->Scale(200.e3/h1data[1]->GetMaximum());
  // depposen[1][0]->SetMaximum(400e3);

  TLegend* latleg = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* depleg = new TLegend(0.7, 0.7, 0.9, 0.9);
  // latleg->AddEntry(h1data[0],      "Raw (Arb. Scale)",     "l");
  latleg->AddEntry(h1canon[0],     "Canonical",            "l");
  latleg->AddEntry(latposen[0][0], "Left Side of Target",  "l");
  latleg->AddEntry(latposen[0][4], "Right Side of Target", "l");
  // depleg->AddEntry(h1data[0],      "Raw (Arb. Scale)",     "l");
  depleg->AddEntry(h1canon[0],     "Canonical",            "l");
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
  // h1data  [0]   ->Draw("SAME");
  h1canon [0]   ->Draw("SAME");
  latleg->Draw();
  cen->cd(2);
  latposen[1][4]->Draw("SAME");
  latposen[1][3]->Draw("SAME");
  latposen[1][2]->Draw("SAME");
  latposen[1][1]->Draw("SAME");
  latposen[1][0]->Draw("SAME");
  // h1data  [1]   ->Draw("SAME");
  h1canon [1]   ->Draw("SAME");
  latleg->Draw();
  cen->cd(3);
  latposen[2][0]->Draw("SAME");
  latposen[2][1]->Draw("SAME");
  latposen[2][2]->Draw("SAME");
  latposen[2][3]->Draw("SAME");
  latposen[2][4]->Draw("SAME");
  h1canon [2]   ->Draw("SAME");
  latleg->Draw();
  cen->cd(4);
  depposen[0][0]->Draw("SAME");
  depposen[0][1]->Draw("SAME");
  depposen[0][2]->Draw("SAME");
  depposen[0][3]->Draw("SAME");
  depposen[0][4]->Draw("SAME");
  // h1data  [0]   ->Draw("SAME");
  h1canon [0]   ->Draw("SAME");
  depleg->Draw();
  cen->cd(5);
  depposen[1][0]->Draw("SAME");
  depposen[1][1]->Draw("SAME");
  depposen[1][2]->Draw("SAME");
  depposen[1][3]->Draw("SAME");
  depposen[1][4]->Draw("SAME");
  // h1data  [1]   ->Draw("SAME");
  h1canon [1]   ->Draw("SAME");
  depleg->Draw();
  cen->cd(6);
  depposen[2][4]->Draw("SAME");
  depposen[2][3]->Draw("SAME");
  depposen[2][2]->Draw("SAME");
  depposen[2][1]->Draw("SAME");
  depposen[2][0]->Draw("SAME");
  h1canon [2]   ->Draw("SAME");
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

  //////////////////////////////////////////////////////////////////////////////
  // Print rates and graph
  //////////////////////////////////////////////////////////////////////////////
  printf("==================================================\n");
  printf("3.5-10 MeV Lateral L/R/Avg (from stops in left side of target to right):\n");
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos35[0][0].first, nlatpos35[0][0].second, nlatpos35[1][0].first, nlatpos35[1][0].second, nlatpos35[2][0].first, nlatpos35[2][0].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos35[0][1].first, nlatpos35[0][1].second, nlatpos35[1][1].first, nlatpos35[1][1].second, nlatpos35[2][1].first, nlatpos35[2][1].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos35[0][2].first, nlatpos35[0][2].second, nlatpos35[1][2].first, nlatpos35[1][2].second, nlatpos35[2][2].first, nlatpos35[2][2].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos35[0][3].first, nlatpos35[0][3].second, nlatpos35[1][3].first, nlatpos35[1][3].second, nlatpos35[2][3].first, nlatpos35[2][3].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos35[0][4].first, nlatpos35[0][4].second, nlatpos35[1][4].first, nlatpos35[1][4].second, nlatpos35[2][4].first, nlatpos35[2][4].second);
  printf("==================================================\n");
  printf("3.5-10 MeV Depth L/R/Avg (from stops in the front of target to back):\n");
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos35[0][0].first, ndeppos35[0][0].second, ndeppos35[1][0].first, ndeppos35[1][0].second, ndeppos35[2][0].first, ndeppos35[2][0].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos35[0][1].first, ndeppos35[0][1].second, ndeppos35[1][1].first, ndeppos35[1][1].second, ndeppos35[2][1].first, ndeppos35[2][1].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos35[0][2].first, ndeppos35[0][2].second, ndeppos35[1][2].first, ndeppos35[1][2].second, ndeppos35[2][2].first, ndeppos35[2][2].second);
  printf("%g +/- %g\t%g  +/- %g\t%g +/- %g\n", ndeppos35[0][3].first, ndeppos35[0][3].second, ndeppos35[1][3].first, ndeppos35[1][3].second, ndeppos35[2][3].first, ndeppos35[2][3].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos35[0][4].first, ndeppos35[0][4].second, ndeppos35[1][4].first, ndeppos35[1][4].second, ndeppos35[2][4].first, ndeppos35[2][4].second);
  printf("==================================================\n");
  printf("3.5-10 MeV Canonical L/R/Avg:\n");
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ncanon35[0].first, ncanon35[0].second, ncanon35[1].first, ncanon35[1].second, ncanon35[2].first, ncanon35[2].second);
  printf("==================================================\n");
  printf("4-8 MeV Lateral L/R/Avg (from stops in left side of target to right):\n");
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos40[0][0].first, nlatpos40[0][0].second, nlatpos40[1][0].first, nlatpos40[1][0].second, nlatpos40[2][0].first, nlatpos40[2][0].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos40[0][1].first, nlatpos40[0][1].second, nlatpos40[1][1].first, nlatpos40[1][1].second, nlatpos40[2][1].first, nlatpos40[2][1].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos40[0][2].first, nlatpos40[0][2].second, nlatpos40[1][2].first, nlatpos40[1][2].second, nlatpos40[2][2].first, nlatpos40[2][2].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos40[0][3].first, nlatpos40[0][3].second, nlatpos40[1][3].first, nlatpos40[1][3].second, nlatpos40[2][3].first, nlatpos40[2][3].second);
  printf("%g +/ -%g\t%g +/- %g\t%g +/- %g\n", nlatpos40[0][4].first, nlatpos40[0][4].second, nlatpos40[1][4].first, nlatpos40[1][4].second, nlatpos40[2][4].first, nlatpos40[2][4].second);
  printf("==================================================\n");
  printf("4-8 MeV Depth L/R/Avg (from stops in the front of target to back):\n");
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos40[0][0].first, ndeppos40[0][0].second, ndeppos40[1][0].first, ndeppos40[1][0].second, ndeppos40[2][0].first, ndeppos40[2][0].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos40[0][1].first, ndeppos40[0][1].second, ndeppos40[1][1].first, ndeppos40[1][1].second, ndeppos40[2][1].first, ndeppos40[2][1].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos40[0][2].first, ndeppos40[0][2].second, ndeppos40[1][2].first, ndeppos40[1][2].second, ndeppos40[2][2].first, ndeppos40[2][2].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos40[0][3].first, ndeppos40[0][3].second, ndeppos40[1][3].first, ndeppos40[1][3].second, ndeppos40[2][3].first, ndeppos40[2][3].second);
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ndeppos40[0][4].first, ndeppos40[0][4].second, ndeppos40[1][4].first, ndeppos40[1][4].second, ndeppos40[2][4].first, ndeppos40[2][4].second);
  printf("==================================================\n");
  printf("4-8 MeV Canonical L/R/Avg:\n");
  printf("%g +/- %g\t%g +/- %g\t%g +/- %g\n", ncanon40[0].first, ncanon40[0].second, ncanon40[1].first, ncanon40[1].second, ncanon40[2].first, ncanon40[2].second);

  Double_t xlat[5] = { -3.48, -1.74, 0, 1.74, 3.48 };
  Double_t xdep[5] = { -2,     -1,   0, 1,    2    };
  Double_t ylat35l[5] = { nlatpos35[0][0].first,  nlatpos35[0][1].first,  nlatpos35[0][2].first,  nlatpos35[0][3].first,  nlatpos35[0][4].first };
  Double_t ylat35r[5] = { nlatpos35[1][0].first,  nlatpos35[1][1].first,  nlatpos35[1][2].first,  nlatpos35[1][3].first,  nlatpos35[1][4].first };
  Double_t ylat35a[5] = { nlatpos35[2][0].first,  nlatpos35[2][1].first,  nlatpos35[2][2].first,  nlatpos35[2][3].first,  nlatpos35[2][4].first };
  Double_t elat35l[5] = { nlatpos35[0][0].second, nlatpos35[0][1].second, nlatpos35[0][2].second, nlatpos35[0][3].second, nlatpos35[0][4].second };
  Double_t elat35r[5] = { nlatpos35[1][0].second, nlatpos35[1][1].second, nlatpos35[1][2].second, nlatpos35[1][3].second, nlatpos35[1][4].second };
  Double_t elat35a[5] = { nlatpos35[2][0].second, nlatpos35[2][1].second, nlatpos35[2][2].second, nlatpos35[2][3].second, nlatpos35[2][4].second };
  Double_t ydep35l[5] = { ndeppos35[0][0].first,  ndeppos35[0][1].first,  ndeppos35[0][2].first,  ndeppos35[0][3].first,  ndeppos35[0][4].first };
  Double_t ydep35r[5] = { ndeppos35[1][0].first,  ndeppos35[1][1].first,  ndeppos35[1][2].first,  ndeppos35[1][3].first,  ndeppos35[1][4].first };
  Double_t ydep35a[5] = { ndeppos35[2][0].first,  ndeppos35[2][1].first,  ndeppos35[2][2].first,  ndeppos35[2][3].first,  ndeppos35[2][4].first };
  Double_t edep35l[5] = { ndeppos35[0][0].second, ndeppos35[0][1].second, ndeppos35[0][2].second, ndeppos35[0][3].second, ndeppos35[0][4].second };
  Double_t edep35r[5] = { ndeppos35[1][0].second, ndeppos35[1][1].second, ndeppos35[1][2].second, ndeppos35[1][3].second, ndeppos35[1][4].second };
  Double_t edep35a[5] = { ndeppos35[2][0].second, ndeppos35[2][1].second, ndeppos35[2][2].second, ndeppos35[2][3].second, ndeppos35[2][4].second };
  Double_t ylat40l[5] = { nlatpos40[0][0].first,  nlatpos40[0][1].first,  nlatpos40[0][2].first,  nlatpos40[0][3].first,  nlatpos40[0][4].first };
  Double_t ylat40r[5] = { nlatpos40[1][0].first,  nlatpos40[1][1].first,  nlatpos40[1][2].first,  nlatpos40[1][3].first,  nlatpos40[1][4].first };
  Double_t ylat40a[5] = { nlatpos40[2][0].first,  nlatpos40[2][1].first,  nlatpos40[2][2].first,  nlatpos40[2][3].first,  nlatpos40[2][4].first };
  Double_t elat40l[5] = { nlatpos40[0][0].second, nlatpos40[0][1].second, nlatpos40[0][2].second, nlatpos40[0][3].second, nlatpos40[0][4].second };
  Double_t elat40r[5] = { nlatpos40[1][0].second, nlatpos40[1][1].second, nlatpos40[1][2].second, nlatpos40[1][3].second, nlatpos40[1][4].second };
  Double_t elat40a[5] = { nlatpos40[2][0].second, nlatpos40[2][1].second, nlatpos40[2][2].second, nlatpos40[2][3].second, nlatpos40[2][4].second };
  Double_t ydep40l[5] = { ndeppos40[0][0].first,  ndeppos40[0][1].first,  ndeppos40[0][2].first,  ndeppos40[0][3].first,  ndeppos40[0][4].first };
  Double_t ydep40r[5] = { ndeppos40[1][0].first,  ndeppos40[1][1].first,  ndeppos40[1][2].first,  ndeppos40[1][3].first,  ndeppos40[1][4].first };
  Double_t ydep40a[5] = { ndeppos40[2][0].first,  ndeppos40[2][1].first,  ndeppos40[2][2].first,  ndeppos40[2][3].first,  ndeppos40[2][4].first };
  Double_t edep40l[5] = { ndeppos40[0][0].second, ndeppos40[0][1].second, ndeppos40[0][2].second, ndeppos40[0][3].second, ndeppos40[0][4].second };
  Double_t edep40r[5] = { ndeppos40[1][0].second, ndeppos40[1][1].second, ndeppos40[1][2].second, ndeppos40[1][3].second, ndeppos40[1][4].second };
  Double_t edep40a[5] = { ndeppos40[2][0].second, ndeppos40[2][1].second, ndeppos40[2][2].second, ndeppos40[2][3].second, ndeppos40[2][4].second };
  TMultiGraph* mglat35 = new TMultiGraph("mglat35", "3.5-10 MeV rates as function of muon stop position;Lateral position;Rate");
  TMultiGraph* mgdep35 = new TMultiGraph("mgdep35", "3.5-10 MeV rates as function of muon stop position;Depth;Rate");
  TMultiGraph* mglat40 = new TMultiGraph("mglat40", "4-8 MeV rates as function of muon stop position;Lateral position;Rate");
  TMultiGraph* mgdep40 = new TMultiGraph("mgdep40", "4-8 MeV rates as function of muon stop position;Depth;Rate");
  TGraphErrors* glat35l = new TGraphErrors(5, xlat, ylat35l, nullptr, elat35l);
  TGraphErrors* glat35r = new TGraphErrors(5, xlat, ylat35r, nullptr, elat35r);
  TGraphErrors* glat35a = new TGraphErrors(5, xlat, ylat35a, nullptr, elat35a);
  TGraphErrors* gdep35l = new TGraphErrors(5, xdep, ydep35l, nullptr, edep35l);
  TGraphErrors* gdep35r = new TGraphErrors(5, xdep, ydep35r, nullptr, edep35r);
  TGraphErrors* gdep35a = new TGraphErrors(5, xdep, ydep35a, nullptr, edep35a);
  TGraphErrors* glat40l = new TGraphErrors(5, xlat, ylat40l, nullptr, elat40l);
  TGraphErrors* glat40r = new TGraphErrors(5, xlat, ylat40r, nullptr, elat40r);
  TGraphErrors* glat40a = new TGraphErrors(5, xlat, ylat40a, nullptr, elat40a);
  TGraphErrors* gdep40l = new TGraphErrors(5, xdep, ydep40l, nullptr, edep40l);
  TGraphErrors* gdep40r = new TGraphErrors(5, xdep, ydep40r, nullptr, edep40r);
  TGraphErrors* gdep40a = new TGraphErrors(5, xdep, ydep40a, nullptr, edep40a);
  glat35l->SetLineColor(kBlue); glat35r->SetLineColor(kRed); glat35a->SetLineColor(kBlack);
  gdep35l->SetLineColor(kBlue); gdep35r->SetLineColor(kRed); gdep35a->SetLineColor(kBlack);
  glat40l->SetLineColor(kBlue); glat40r->SetLineColor(kRed); glat40a->SetLineColor(kBlack);
  gdep40l->SetLineColor(kBlue); gdep40r->SetLineColor(kRed); gdep40a->SetLineColor(kBlack);
  glat35l->SetTitle("SiL");    glat35r->SetTitle("SiR");  glat35a->SetTitle("Avg");
  gdep35l->SetTitle("SiL");    gdep35r->SetTitle("SiR");  gdep35a->SetTitle("Avg");
  glat40l->SetTitle("SiL");    glat40r->SetTitle("SiR");  glat40a->SetTitle("Avg");
  gdep40l->SetTitle("SiL");    gdep40r->SetTitle("SiR");  gdep40a->SetTitle("Avg");

  mglat35->Add(glat35l); mglat35->Add(glat35r); mglat35->Add(glat35a);
  mgdep35->Add(gdep35l); mgdep35->Add(gdep35r); mgdep35->Add(gdep35a);
  mglat40->Add(glat40l); mglat40->Add(glat40r); mglat40->Add(glat40a);
  mgdep40->Add(gdep40l); mgdep40->Add(gdep40r); mgdep40->Add(gdep40a);

  TLine* llatcanon35l = new TLine(xlat[0], ncanon35[0].first, xlat[4], ncanon35[0].first);
  TLine* llatcanon35r = new TLine(xlat[0], ncanon35[1].first, xlat[4], ncanon35[1].first);
  TLine* llatcanon35a = new TLine(xlat[0], ncanon35[2].first, xlat[4], ncanon35[2].first);
  TLine* ldepcanon35l = new TLine(xdep[0], ncanon35[0].first, xdep[4], ncanon35[0].first);
  TLine* ldepcanon35r = new TLine(xdep[0], ncanon35[1].first, xdep[4], ncanon35[1].first);
  TLine* ldepcanon35a = new TLine(xdep[0], ncanon35[2].first, xdep[4], ncanon35[2].first);
  TLine* llatcanon40l = new TLine(xlat[0], ncanon40[0].first, xlat[4], ncanon40[0].first);
  TLine* llatcanon40r = new TLine(xlat[0], ncanon40[1].first, xlat[4], ncanon40[1].first);
  TLine* llatcanon40a = new TLine(xlat[0], ncanon40[2].first, xlat[4], ncanon40[2].first);
  TLine* ldepcanon40l = new TLine(xdep[0], ncanon40[0].first, xdep[4], ncanon40[0].first);
  TLine* ldepcanon40r = new TLine(xdep[0], ncanon40[1].first, xdep[4], ncanon40[1].first);
  TLine* ldepcanon40a = new TLine(xdep[0], ncanon40[2].first, xdep[4], ncanon40[2].first);
  llatcanon35l->SetLineColor(kBlue);  llatcanon35l->SetLineStyle(9);
  llatcanon35r->SetLineColor(kRed);   llatcanon35r->SetLineStyle(9);
  llatcanon35a->SetLineColor(kBlack); llatcanon35a->SetLineStyle(9);
  ldepcanon35l->SetLineColor(kBlue);  ldepcanon35l->SetLineStyle(9);
  ldepcanon35r->SetLineColor(kRed);   ldepcanon35r->SetLineStyle(9);
  ldepcanon35a->SetLineColor(kBlack); ldepcanon35a->SetLineStyle(9);
  llatcanon40l->SetLineColor(kBlue);  llatcanon40l->SetLineStyle(9);
  llatcanon40r->SetLineColor(kRed);   llatcanon40r->SetLineStyle(9);
  llatcanon40a->SetLineColor(kBlack); llatcanon40a->SetLineStyle(9);
  ldepcanon40l->SetLineColor(kBlue);  ldepcanon40l->SetLineStyle(9);
  ldepcanon40r->SetLineColor(kRed);   ldepcanon40r->SetLineStyle(9);
  ldepcanon40a->SetLineColor(kBlack); ldepcanon40a->SetLineStyle(9);

  TLegend* lmg = new TLegend(0.7, 0.7, 0.9, 0.9);
  lmg->AddEntry(glat35l,      "Study SiL");
  lmg->AddEntry(glat35r,      "Study SiR");
  lmg->AddEntry(glat35a,      "Study Avg");
  lmg->AddEntry(llatcanon35a, "Canonical Avg");

  TCanvas* cmg = new TCanvas("cmg", "Rates", 1400, 1000);
  cmg->Divide(2, 2);
  cmg->cd(1); mglat35->Draw("APL"); llatcanon35l->Draw("SAME");  llatcanon35r->Draw("SAME");  llatcanon35a->Draw("SAME"); lmg->Draw();
  cmg->cd(2); mgdep35->Draw("APL"); ldepcanon35l->Draw("SAME");  ldepcanon35r->Draw("SAME");  ldepcanon35a->Draw("SAME");
  cmg->cd(3); mglat40->Draw("APL"); llatcanon40l->Draw("SAME");  llatcanon40r->Draw("SAME");  llatcanon40a->Draw("SAME");
  cmg->cd(4); mgdep40->Draw("APL"); ldepcanon40l->Draw("SAME");  ldepcanon40r->Draw("SAME");  ldepcanon40a->Draw("SAME");
  cmg->SaveAs("img/unfold_targsec_rates.png");

}
