{
  // SiL1 in MC goes left-to-right. Don't know about experiment.
  TFile* fdat = new TFile("~/R15bTME/Si16b/out10362.root");
  TFile* fsim = new TFile("~/data/R15b/mc/R15bSi16bMuPreSiT_1.root");
  TTree* trdat = (TTree*)fdat->Get("TMETree/TMETree");
  TTree* trsim = (TTree*)fsim->Get("tree");

  long long ndat[16] = {
    trdat->GetEntries("@SiL1_1.size()==1  && -200<SiL1_1.tTME  && SiL1_1.tTME<200"),
    trdat->GetEntries("@SiL1_2.size()==1  && -200<SiL1_2.tTME  && SiL1_2.tTME<200"),
    trdat->GetEntries("@SiL1_3.size()==1  && -200<SiL1_3.tTME  && SiL1_3.tTME<200"),
    trdat->GetEntries("@SiL1_4.size()==1  && -200<SiL1_4.tTME  && SiL1_4.tTME<200"),
    trdat->GetEntries("@SiL1_5.size()==1  && -200<SiL1_5.tTME  && SiL1_5.tTME<200"),
    trdat->GetEntries("@SiL1_6.size()==1  && -200<SiL1_6.tTME  && SiL1_6.tTME<200"),
    trdat->GetEntries("@SiL1_7.size()==1  && -200<SiL1_7.tTME  && SiL1_7.tTME<200"),
    trdat->GetEntries("@SiL1_8.size()==1  && -200<SiL1_8.tTME  && SiL1_8.tTME<200"),
    trdat->GetEntries("@SiL1_9.size()==1  && -200<SiL1_9.tTME  && SiL1_9.tTME<200"),
    trdat->GetEntries("@SiL1_10.size()==1 && -200<SiL1_10.tTME && SiL1_10.tTME<200"),
    trdat->GetEntries("@SiL1_11.size()==1 && -200<SiL1_11.tTME && SiL1_11.tTME<200"),
    trdat->GetEntries("@SiL1_12.size()==1 && -200<SiL1_12.tTME && SiL1_12.tTME<200"),
    trdat->GetEntries("@SiL1_13.size()==1 && -200<SiL1_13.tTME && SiL1_13.tTME<200"),
    trdat->GetEntries("@SiL1_14.size()==1 && -200<SiL1_14.tTME && SiL1_14.tTME<200"),
    trdat->GetEntries("@SiL1_15.size()==1 && -200<SiL1_15.tTME && SiL1_15.tTME<200"),
    trdat->GetEntries("@SiL1_16.size()==1 && -200<SiL1_16.tTME && SiL1_16.tTME<200")
  };
  TH1* hdat = new TH1D("hdat", "hdat", 16, 0.5, 16.5);
  hdat->SetBinContent(1,  ndat[0]);  hdat->SetBinError(1,  std::sqrt(ndat[0]));
  hdat->SetBinContent(2,  ndat[1]);  hdat->SetBinError(2,  std::sqrt(ndat[1]));
  hdat->SetBinContent(3,  ndat[2]);  hdat->SetBinError(3,  std::sqrt(ndat[2]));
  hdat->SetBinContent(4,  ndat[3]);  hdat->SetBinError(4,  std::sqrt(ndat[3]));
  hdat->SetBinContent(5,  ndat[4]);  hdat->SetBinError(5,  std::sqrt(ndat[4]));
  hdat->SetBinContent(6,  ndat[5]);  hdat->SetBinError(6,  std::sqrt(ndat[5]));
  hdat->SetBinContent(7,  ndat[6]);  hdat->SetBinError(7,  std::sqrt(ndat[6]));
  hdat->SetBinContent(8,  ndat[7]);  hdat->SetBinError(8,  std::sqrt(ndat[7]));
  hdat->SetBinContent(9,  ndat[8]);  hdat->SetBinError(9,  std::sqrt(ndat[8]));
  hdat->SetBinContent(10, ndat[9]);  hdat->SetBinError(10, std::sqrt(ndat[9]));
  hdat->SetBinContent(11, ndat[10]); hdat->SetBinError(11, std::sqrt(ndat[10]));
  hdat->SetBinContent(12, ndat[11]); hdat->SetBinError(12, std::sqrt(ndat[11]));
  hdat->SetBinContent(13, ndat[12]); hdat->SetBinError(13, std::sqrt(ndat[12]));
  hdat->SetBinContent(14, ndat[13]); hdat->SetBinError(14, std::sqrt(ndat[13]));
  hdat->SetBinContent(15, ndat[14]); hdat->SetBinError(15, std::sqrt(ndat[14]));
  hdat->SetBinContent(16, ndat[15]); hdat->SetBinError(16, std::sqrt(ndat[15]));
  trsim->Draw("M_volID+1>>hsim(16,0.5,16.5)", "M_volName==\"SiL1\" && M_volID!=0 && M_volID!=15", "goff");

  hdat->SetLineColor(1);
  hsim->SetLineColor(2);
  hdat->SetMinimum(0);
  hsim->SetMinimum(0);

  hdat->SetTitle("SiL1 muon hit distribution;Segment;hit/mu");
  hsim->SetTitle("SiL1 muon hit distribution;Quadrant;hit/mu");

  TLegend* leg = new TLegend(0.7, 0.1, 0.9, 0.3);
  leg->AddEntry(hdat, "Data");
  leg->AddEntry(hsim, "MC");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hdat->DrawNormalized("SAME");
  hsim->DrawNormalized("SAME");
  leg->Draw();
  c->cd(2);
  hdat->DrawNormalized("SAME");
  hsim->DrawNormalized("SAME");
  leg->Draw();
  c->SaveAs("img/targ_pos_mc_data.png");
}
