{
  gROOT->ProcessLine(".L libAnalysis.so");
  gStyle->SetPalette(55);

  TString data_path = "/home/nam/work/RunPSI2013/data/root/dq3_rootanahist/";
  int firstRun = 2091;
  int lastRun = 2172;
  int runNo = firstRun;
  TString data_file = data_path + Form("ranahist%.5d.root", runNo);

  TFile *rinf;
  TString histS_dir, histF_dir;
  histS_dir = "Tdiff_muSc_SiR2S";
  histF_dir = "Tdiff_muSc_SiR2F";
  histSF_dir = "Tdiff_SiR2F_SiR2S";

  TH2F *h2_SiR2S, *h2_SiR2F, *h2_SiR2FS;

  rinf = TFile::Open(data_file);
  rinf->cd(histS_dir);
  h2_SiR2S = (TH2F*)gDirectory->Get("hmuSc-SiR2-S_AmpVsTDiff_Fine");

  rinf->cd();
  rinf->cd(histF_dir);
  h2_SiR2F = (TH2F*)gDirectory->Get("hmuSc-SiR2-F_AmpVsTDiff_Fine");

  rinf->cd();
  rinf->cd(histSF_dir);
  h2_SiR2FS = (TH2F*)gDirectory->Get("hSiR2-F-SiR2-S_AmpVsTDiff_Fine");

  for (runNo = firstRun + 1; runNo <= lastRun; ++runNo)
  {
    data_file = data_path + Form("ranahist%.5d.root", runNo);

    if (!gSystem->AccessPathName(data_file))
    {
      rinf = TFile::Open(data_file);
      std::cout<<"Processing run "<< runNo <<std::endl;
      rinf->cd(histS_dir);
      h2_SiR2S->Add((TH2F*)gDirectory->Get("hmuSc-SiR2-S_AmpVsTDiff_Fine"));

      rinf->cd();
      rinf->cd(histF_dir);
      h2_SiR2F->Add((TH2F*)gDirectory->Get("hmuSc-SiR2-F_AmpVsTDiff_Fine"));

      rinf->cd();
      rinf->cd(histSF_dir);
      h2_SiR2FS->Add((TH2F*)gDirectory->Get("hSiR2-F-SiR2-S_AmpVsTDiff_Fine"));
    }
    else
    {
      std::cout<<"Zombie file, skipped run "<<runNo<<" ..."<<std::endl;
    }
  }

  TCanvas * c =new TCanvas();
  c->SetLogz();
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

  TCanvas * c2 = new TCanvas();
  c2->SetLogz();
  TGraph *gr = new TGraph(npoints-1, t, nhit);
  //gr->Draw();

  h2_SiR2FS->Draw("colz");
}
