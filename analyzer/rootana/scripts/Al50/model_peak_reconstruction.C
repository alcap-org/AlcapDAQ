// Generated protons with different threshold energies, and therefore a peak
// in different spots. We see how well we can reconstruct that peak at
// different threshold energies.
{
  TChain* ch[3][8] = {
    {
      new TChain("PID_L"), new TChain("PID_L"), new TChain("PID_L"),
      new TChain("PID_L"), new TChain("PID_L"), new TChain("PID_L"),
      new TChain("PID_L"), new TChain("PID_L")
    },
    {
      new TChain("PID_R"), new TChain("PID_R"), new TChain("PID_R"),
      new TChain("PID_R"), new TChain("PID_R"), new TChain("PID_R"),
      new TChain("PID_R"), new TChain("PID_R")
    },
    {
      new TChain("tree"), new TChain("tree"), new TChain("tree"),
      new TChain("tree"), new TChain("tree"), new TChain("tree"),
      new TChain("tree"), new TChain("tree")
    }
  };
  ch[0][0]->Add("data/mc_0.5MeVThresh_*.root");
  ch[0][1]->Add("data/mc_1.0MeVThresh_*.root");
  ch[0][2]->Add("data/mc_1.5MeVThresh_*.root");
  ch[0][3]->Add("data/mc_2.0MeVThresh_*.root");
  ch[0][4]->Add("data/mc_2.5MeVThresh_*.root");
  ch[0][5]->Add("data/mc_3.0MeVThresh_*.root");
  ch[0][6]->Add("data/mc_3.5MeVThresh_*.root");
  ch[0][7]->Add("data/mc_4.0MeVThresh_*.root");
  ch[1][0]->Add("data/mc_0.5MeVThresh_*.root");
  ch[1][1]->Add("data/mc_1.0MeVThresh_*.root");
  ch[1][2]->Add("data/mc_1.5MeVThresh_*.root");
  ch[1][3]->Add("data/mc_2.0MeVThresh_*.root");
  ch[1][4]->Add("data/mc_2.5MeVThresh_*.root");
  ch[1][5]->Add("data/mc_3.0MeVThresh_*.root");
  ch[1][6]->Add("data/mc_3.5MeVThresh_*.root");
  ch[1][7]->Add("data/mc_4.0MeVThresh_*.root");
  ch[2][0]->Add("data/mc/R15bAl50_0.5MeVThresh_*.root");
  ch[2][1]->Add("data/mc/R15bAl50_1.0MeVThresh_*.root");
  ch[2][2]->Add("data/mc/R15bAl50_1.5MeVThresh_*.root");
  ch[2][3]->Add("data/mc/R15bAl50_2.0MeVThresh_*.root");
  ch[2][4]->Add("data/mc/R15bAl50_2.5MeVThresh_*.root");
  ch[2][5]->Add("data/mc/R15bAl50_3.0MeVThresh_*.root");
  ch[2][6]->Add("data/mc/R15bAl50_3.5MeVThresh_*.root");
  ch[2][7]->Add("data/mc/R15bAl50_4.0MeVThresh_*.root");

  TH1* heraw[2][8] = {
    {
      new TH1F("h500l",  "500 keV Peak Left;Energy [keV]",    30, 0, 15e3),
      new TH1F("h1000l", "1000 keV Peak Left;Energy [keV]",   30, 0, 15e3),
      new TH1F("h1500l", "1500 keV Peak Left;Energy [keV]",   30, 0, 15e3),
      new TH1F("h2000l", "2000 keV Peak Left;Energy [keV]",   30, 0, 15e3),
      new TH1F("h2500l", "2500 keV Peak Left;Energy [keV]",   30, 0, 15e3),
      new TH1F("h3000l", "3000 keV Peak Left;Energy [keV]",   30, 0, 15e3),
      new TH1F("h3500l", "3500 keV Peak Left;Energy [keV]",   30, 0, 15e3),
      new TH1F("h4000l", "4000 keV Peak Left;Energy [keV]",   30, 0, 15e3)
    },
    {
      new TH1F("h500r",  "500 keV Peak Right;Energy [keV]",  30, 0, 15e3),
      new TH1F("h1000r", "1000 keV Peak Right;Energy [keV]", 30, 0, 15e3),
      new TH1F("h1500r", "1500 keV Peak Right;Energy [keV]", 30, 0, 15e3),
      new TH1F("h2000r", "2000 keV Peak Right;Energy [keV]", 30, 0, 15e3),
      new TH1F("h2500r", "2500 keV Peak Right;Energy [keV]", 30, 0, 15e3),
      new TH1F("h3000r", "3000 keV Peak Right;Energy [keV]", 30, 0, 15e3),
      new TH1F("h3500r", "3500 keV Peak Right;Energy [keV]", 30, 0, 15e3),
      new TH1F("h4000r", "4000 keV Peak Right;Energy [keV]", 30, 0, 15e3)
    }
  };
  TH1* hetrue[8] = {
    new TH1F("h500",  "500 keV Peak True;Energy [keV]",  30, 0, 15e3),
    new TH1F("h1000", "1000 keV Peak True;Energy [keV]", 30, 0, 15e3),
    new TH1F("h1500", "1500 keV Peak True;Energy [keV]", 30, 0, 15e3),
    new TH1F("h2000", "2000 keV Peak True;Energy [keV]", 30, 0, 15e3),
    new TH1F("h2500", "2500 keV Peak True;Energy [keV]", 30, 0, 15e3),
    new TH1F("h3000", "3000 keV Peak True;Energy [keV]", 30, 0, 15e3),
    new TH1F("h3500", "3500 keV Peak True;Energy [keV]", 30, 0, 15e3),
    new TH1F("h4000", "4000 keV Peak True;Energy [keV]", 30, 0, 15e3),
  };

  ch[0][0]->Draw("e>>+h500l",  "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][1]->Draw("e>>+h1000l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][2]->Draw("e>>+h1500l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][3]->Draw("e>>+h2000l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][4]->Draw("e>>+h2500l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][5]->Draw("e>>+h3000l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][6]->Draw("e>>+h3500l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[0][7]->Draw("e>>+h4000l", "e2>0 && e0 < 15e3 && 1 < seg && seg < 16", "goff");
  ch[1][0]->Draw("e>>+h500r",  "e2>0 && e0 < 15e3", "goff");
  ch[1][1]->Draw("e>>+h1000r", "e2>0 && e0 < 15e3", "goff");
  ch[1][2]->Draw("e>>+h1500r", "e2>0 && e0 < 15e3", "goff");
  ch[1][3]->Draw("e>>+h2000r", "e2>0 && e0 < 15e3", "goff");
  ch[1][4]->Draw("e>>+h2500r", "e2>0 && e0 < 15e3", "goff");
  ch[1][5]->Draw("e>>+h3000r", "e2>0 && e0 < 15e3", "goff");
  ch[1][6]->Draw("e>>+h3500r", "e2>0 && e0 < 15e3", "goff");
  ch[1][7]->Draw("e>>+h4000r", "e2>0 && e0 < 15e3", "goff");
  ch[2][0]->Draw("1e6*M_ekin[0]>>+h500",  "M_volName[0]==\"Target\"", "goff");
  ch[2][1]->Draw("1e6*M_ekin[0]>>+h1000", "M_volName[0]==\"Target\"", "goff");
  ch[2][2]->Draw("1e6*M_ekin[0]>>+h1500", "M_volName[0]==\"Target\"", "goff");
  ch[2][3]->Draw("1e6*M_ekin[0]>>+h2000", "M_volName[0]==\"Target\"", "goff");
  ch[2][4]->Draw("1e6*M_ekin[0]>>+h2500", "M_volName[0]==\"Target\"", "goff");
  ch[2][5]->Draw("1e6*M_ekin[0]>>+h3000", "M_volName[0]==\"Target\"", "goff");
  ch[2][6]->Draw("1e6*M_ekin[0]>>+h3500", "M_volName[0]==\"Target\"", "goff");
  ch[2][7]->Draw("1e6*M_ekin[0]>>+h4000", "M_volName[0]==\"Target\"", "goff");

  TFile* ifile_tm = new TFile("data/tmal50p_frommudist.root", "READ");
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)ifile_tm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)ifile_tm->Get("SiR_TM") };
  for (int i = 0; i < 2; ++i) {
    resp[i]->Hresponse()->Rebin2D(5, 5);
    resp[i]->Htruth()   ->Rebin(5);
    resp[i]->Hmeasured()->Rebin(5);
  }
  RooUnfoldBayes* unfold[2][8];
  TH1* he[3][8];
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 8; ++j) {
      unfold[i][j] = new RooUnfoldBayes(resp[i], heraw[i][j], 4);
      he    [i][j] = unfold[i][j]->Hreco();
    }
  }
  for (int i = 0; i < 8; ++i) {
    he[2][i] = (TH1*)he[0][i]->Clone();
    he[2][i]->Add(he[1][i]);
    he[2][i]->Scale(0.5);
  }

  he[2][0]->SetTitle("T_{th}=500 keV;Energy [keV];Counts/500 keV");
  he[2][1]->SetTitle("T_{th}=1000 keV;Energy [keV];Counts/500 keV");
  he[2][2]->SetTitle("T_{th}=1500 keV;Energy [keV];Counts/500 keV");
  he[2][3]->SetTitle("T_{th}=2000 keV;Energy [keV];Counts/500 keV");
  he[2][4]->SetTitle("T_{th}=2500 keV;Energy [keV];Counts/500 keV");
  he[2][5]->SetTitle("T_{th}=3000 keV;Energy [keV];Counts/500 keV");
  he[2][6]->SetTitle("T_{th}=3500 keV;Energy [keV];Counts/500 keV");
  he[2][7]->SetTitle("T_{th}=4000 keV;Energy [keV];Counts/500 keV");
  he[2][0]->SetMaximum(1.35e6);
  he[2][1]->SetMaximum(1.6e6);
  he[2][2]->SetMaximum(1.3e6);
  he[2][3]->SetMaximum(1.2e6);
  he[2][4]->SetMaximum(1.2e6);
  he[2][5]->SetMaximum(1e6);
  he[2][6]->SetMaximum(1e6);
  he[2][7]->SetMaximum(1e6);
  for (int i = 0; i < 8; ++i) {
    he    [0][i]->SetLineColor(kRed);
    he    [1][i]->SetLineColor(kBlue);
    he    [2][i]->SetLineColor(kMagenta);
    hetrue[i]   ->SetLineColor(kBlack);
  }
  TLegend* leg[8] = {
    new TLegend(0.3, 0.7, 0.5, 0.9), new TLegend(0.3, 0.7, 0.5, 0.9),
    new TLegend(0.3, 0.7, 0.5, 0.9), new TLegend(0.3, 0.7, 0.5, 0.9),
    new TLegend(0.3, 0.7, 0.5, 0.9), new TLegend(0.1, 0.7, 0.3, 0.9),
    new TLegend(0.1, 0.7, 0.3, 0.9), new TLegend(0.1, 0.7, 0.3, 0.9)
  };
  for (int i = 0; i < 8; ++i) {
    leg[i]->AddEntry(he[0][i],  "Unfolded Left");
    leg[i]->AddEntry(he[1][i],  "Unfolded Right");
    leg[i]->AddEntry(he[2][i],  "Average");
    leg[i]->AddEntry(hetrue[i], "True Spectrum");
  }

  TF1* fitfnc[8] = {
    new TF1("fit500",  "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit1000", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit1500", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit2000", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit2500", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit3000", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit3500", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3),
    new TF1("fit4000", "[0]*((x>=[1])*(1-[1]/x))^[2]*exp(-x/[3])", 2e3, 15e3)
  };
  fitfnc[0]->SetParameters(2.5e6, 2000, 0.5, 6e3);
  fitfnc[1]->SetParameters(2.5e6, 2000, 0.5, 6e3);
  fitfnc[2]->SetParameters(2.5e6, 1500, 0.5, 6e3);
  fitfnc[3]->SetParameters(2.5e6, 2000, 0.5, 6e3);
  fitfnc[4]->SetParameters(2e6,   2500, 0.5, 6e3);
  fitfnc[5]->SetParameters(2e6,   2000, 0.5, 6e3);
  fitfnc[6]->SetParameters(2e6,   2000, 0.5, 6e3);
  fitfnc[7]->SetParameters(2e6,   2000, 0.5, 6e3);
  for (int i = 0; i < 8; ++i)
    fitfnc[i]->SetParNames("Scale", "E_{th}", "#alpha", "E_{temp}");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(20000);
  TCanvas* c[8];
  for (int i = 0; i < 8; ++i) {
    c[i] = new TCanvas();
    fitfnc[i]->SetLineColor(kMagenta);
    he[2][i]->Fit(fitfnc[i], "S");
    he[2][i] ->Draw("SAME");
    he[0][i] ->Draw("SAME");
    he[1][i] ->Draw("SAME");
    hetrue[i]->Draw("SAME");
    leg[i]->Draw();
  }
  c[0]->SaveAs("img/peak_reconstruction_validation_500keVThresh.png");
  c[1]->SaveAs("img/peak_reconstruction_validation_1000keVThresh.png");
  c[2]->SaveAs("img/peak_reconstruction_validation_1500keVThresh.png");
  c[3]->SaveAs("img/peak_reconstruction_validation_2000keVThresh.png");
  c[4]->SaveAs("img/peak_reconstruction_validation_2500keVThresh.png");
  c[5]->SaveAs("img/peak_reconstruction_validation_3000keVThresh.png");
  c[6]->SaveAs("img/peak_reconstruction_validation_3500keVThresh.png");
  c[7]->SaveAs("img/peak_reconstruction_validation_4000keVThresh.png");
}
