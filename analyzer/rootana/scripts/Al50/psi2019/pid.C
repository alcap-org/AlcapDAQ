{
  TFile* fcut  = new TFile("~/Documents/psi2019/al50-cuts.root");
  TFile* fdata = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TCutG* gcut = (TCutG*)fcut ->Get("hLg_SiR_EvDeltaE_proton_3sigma");
  TChain* tr = new TChain();
  tr->Add("data/Al50/psel.3layer.pidpow2.root/PID_RP");
  tr->Add("data/Al50/psel.3layer.pidpow2.root/PID_RD");
  tr->Add("data/Al50/psel.3layer.pidpow2.root/PID_RT");
  tr->Add("data/Al50/psel.3layer.pidpow2.root/PID_RA");
  tr->Add("data/Al50/psel.3layer.pidpow2.root/PID_RU");

  double e1, e2, e3, t1, t2;
  tr->SetBranchAddress("e1", &e1);
  tr->SetBranchAddress("e2", &e2);
  tr->SetBranchAddress("e3", &e3);
  tr->SetBranchAddress("t1", &t1);
  tr->SetBranchAddress("t2", &t2);
  TGraph* gr = new TGraph();
  int n = 0;
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (e3 == 0 && abs(t1-t2)<200 && 400<t2 && e1<3e3 && e1+e2<14e3)
      gr->SetPoint(n++, (e1+e2)/1e3, e1/1e3);
  }

  static const int IDX[4] = { 4, 5, 110, 111 };
  double CUT[2][5] = {
    {
      gcut->GetX()[IDX[0]],
      gcut->GetX()[IDX[1]],
      gcut->GetX()[IDX[2]],
      gcut->GetX()[IDX[3]],
      gcut->GetX()[IDX[0]]
    }, {
      gcut->GetY()[IDX[0]],
      gcut->GetY()[IDX[1]],
      gcut->GetY()[IDX[2]],
      gcut->GetY()[IDX[3]],
      gcut->GetY()[IDX[0]]
    }
  };
  TCutG* glincut = new TCutG("gexcut", 5, CUT[0], CUT[1]);
  glincut->SetVarX("(e1+e2)/1e3");
  glincut->SetVarY("e1/1e3");
  glincut->SetLineColor(kBlue);
  glincut->SetLineStyle(2);
  glincut->SetLineWidth(4);

  TH1* hexcut = new TH1D("hexcut", "Rotated Log-Log Slice;(log(E)+log(dE))/#sqrt{2};Count", 100, 0.31, 0.49);
  tr->Draw("(log10(e1/1e3+e2/1e3)+log10(e1/1e3))/sqrt(2)>>hexcut", "gexcut && abs(t1-t2) && 400<t2 && e3==0", "goff");
  hexcut->SetLineColor(kBlack);
  hexcut->SetMinimum(0);
  hexcut->SetMaximum(66);
  TLine* lexcut[2] = {
    new TLine(0.323, 0, 0.323, 66), new TLine(0.475, 0, 0.475, 66)
  };
  lexcut[0]->SetLineColor(kBlue);
  lexcut[1]->SetLineColor(kBlue);

  TF1* fexfit = new TF1("exfit", "gaus(0)", 0, 1);
  fexfit->SetLineColor(kRed);
  hexcut->Fit(fexfit);

  gr->SetMarkerStyle(kDot);
  gr->SetMarkerColor(kBlack);
  gcut->SetLineColor(kRed);
  gr->SetTitle("SiR Proton Cut;E [MeV];dE [MeV]");
  gr->GetXaxis()->SetRangeUser(0, 14);
  gr->GetYaxis()->SetRangeUser(0, 3);

  gStyle->SetOptStat(0);

  gr->GetYaxis()->SetLabelSize(0.05);
  gr->GetYaxis()->SetTitleSize(0.05);
  gr->GetYaxis()->SetTitleOffset(0.9);
  gr->GetXaxis()->SetLabelSize(0.05);
  gr->GetXaxis()->SetTitleSize(0.05);
  gr->GetXaxis()->SetTitleOffset(0.90);
  hexcut->GetYaxis()->SetLabelSize(0.05);
  hexcut->GetYaxis()->SetTitleSize(0.05);
  hexcut->GetYaxis()->SetTitleOffset(0.9);
  hexcut->GetXaxis()->SetLabelSize(0.05);
  hexcut->GetXaxis()->SetTitleSize(0.05);
  hexcut->GetXaxis()->SetTitleOffset(0.90);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  gr     ->Draw("AP");
  gcut   ->Draw("SAME");
  glincut->Draw("SAME");
  TPad* p = new TPad("p", "p", 0.4, 0.4, 0.88, 0.88);
  p->SetRightMargin(0.01);
  p->Draw();
  p->cd();
  hexcut->Draw();
  lexcut[0]->Draw();
  lexcut[1]->Draw();
  c->SaveAs("img/psi2019/deve.png");
}
