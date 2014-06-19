{
  gROOT->ProcessLine(".L libAnalysis.so");

  TString data_path = "/home/nam/work/RunPSI2013/data/root/dq3_rootanahist/";
  int runNo = 3763;
  TString data_file = data_path + Form("ranahist%.5d.root", runNo);

  TFile *rinf;
  TString histS_dir, histF_dir;
  histS_dir = "muSc_SiR2-S";
  histF_dir = "muSc_SiR2-F";
  TH2F *h2_SiR2S, *h2_SiR2F;

  rinf = TFile::Open(data_file);
  rinf->cd(histS_dir);
  h2_SiR2S = (TH2F*)gDirectory->Get("hmuSc-SiR2-S_AmpVsTDiff_Fine");

  rinf->cd();
  rinf->cd(histF_dir);
  h2_SiR2F = (TH2F*)gDirectory->Get("hmuSc-SiR2-F_AmpVsTDiff_Fine");

  for (runNo = 3764; runNo <= 3770; ++runNo)
  {
    data_file = data_path + Form("ranahist%.5d.root", runNo);
    rinf = TFile::Open(data_file);
    rinf->cd(histS_dir);
    h2_SiR2S->Add((TH2F*)gDirectory->Get("hmuSc-SiR2-S_AmpVsTDiff_Fine"));

    rinf->cd();
    rinf->cd(histF_dir);
    h2_SiR2F->Add((TH2F*)gDirectory->Get("hmuSc-SiR2-F_AmpVsTDiff_Fine"));
  }

  //TCanvas * c =new TCanvas();
  //c->Divide(2,1);
  //c->cd(1);
  //h2_SiR2S->Draw("colz");
  //c->cd(2);
  h2_SiR2F->Draw("colz");

  const int npoints = 11;
  double t[npoints];
  for (int i = 0; i < npoints; ++i)
  {
    t[i] = 2000. + i*1000;
  }

  double nhit[npoints];
  for (int i = 0; i < npoints; ++i)
  {
    int xlow = h2_SiR2F->GetXaxis()->FindBin(t[i]);
    int xhigh = h2_SiR2F->GetXaxis()->FindBin(t[i+1]);
    nhit[i] = h2_SiR2F->Integral(xlow, xhigh, 1, 2500);
  }

  //TGraph *gr = new TGraph(npoints-1, t, nhit);
  //gr->Draw();

}
