{
  Int_t kACBlue = TColor::GetColor("#67a9cf");
  Int_t kACRed  = TColor::GetColor("#ef8a62");

  TFile* fmw = new TFile("~m-wong/data/transfer/transfer.sf1.02.al50.proton.root");
  TFile* fjq = new TFile("~jrquirk/data/R15b/Al50/tms/tm.p.2layer.mudist.root");
  RooUnfoldResponse* respmw[2] = {
    (RooUnfoldResponse*)fmw->Get("SiL500_TM"),
    (RooUnfoldResponse*)fmw->Get("SiR500_TM")
  };
  RooUnfoldResponse* respjq[2] = {
    (RooUnfoldResponse*)fjq->Get("SiL_TM"),
    (RooUnfoldResponse*)fjq->Get("SiR_TM")
  };
  TH2* hrespmw[2] = { respmw[0]->Hresponse(),       respmw[1]->Hresponse()      };
  TH2* hrespjq[2] = { respjq[0]->Hresponse(),       respjq[1]->Hresponse()      };
  TH1* hgenmw [2] = { respmw[0]->Htruth(),          respmw[1]->Htruth()         };
  TH1* hgenjq [2] = { respjq[0]->Htruth(),          respjq[1]->Htruth()         };
  TH1* hmeasmw[2] = { respmw[0]->Hmeasured(),       respmw[1]->Hmeasured()      };
  TH1* hmeasjq[2] = { respjq[0]->Hmeasured(),       respjq[1]->Hmeasured()      };
  hrespjq[0]->Rebin2D(5, 5); hrespjq[1]->Rebin2D(5, 5);
  hgenjq [0]->Rebin(5);      hgenjq [1]->Rebin(5);
  hmeasjq[0]->Rebin(5);      hmeasjq[1]->Rebin(5);
  hgenmw[0]->Sumw2(); hgenmw[1]->Sumw2();
  hgenjq[0]->Sumw2(); hgenjq[1]->Sumw2();
  hmeasmw[0]->Sumw2(); hmeasmw[1]->Sumw2();
  hmeasjq[0]->Sumw2(); hmeasjq[1]->Sumw2();
  TH1* heffmw [2] = { (TH1*)hmeasmw[0]->Clone("heffmwl"), (TH1*)hmeasmw[1]->Clone("heffmwr") };
  TH1* heffjq [2] = { (TH1*)hmeasjq[0]->Clone("heffjql"), (TH1*)hmeasjq[1]->Clone("heffjqr") };

  heffmw[0]->Divide(hgenmw[0]); heffmw[1]->Divide(hgenmw[1]);
  heffjq[0]->Divide(hgenjq[0]); heffjq[1]->Divide(hgenjq[1]);
  // heffjq [0]->Rebin(5);    heffjq [1]->Rebin(5);

  hrespmw[0]->SetTitle("SiL Response (MW)");
  hrespmw[1]->SetTitle("SiR Response (MW)");
  hrespjq[0]->SetTitle("SiL Response (JQ)");
  hrespjq[1]->SetTitle("SiR Response (JQ)");

  hgenmw [0]->SetLineColor(kBlack);  hgenmw [1]->SetLineColor(kBlack);
  hgenjq [0]->SetLineColor(kBlack);  hgenjq [1]->SetLineColor(kBlack);
  hmeasmw[0]->SetLineColor(kACBlue); hmeasmw[1]->SetLineColor(kACRed);
  hmeasjq[0]->SetLineColor(kACBlue); hmeasjq[1]->SetLineColor(kACRed);
  heffmw [0]->SetLineColor(kACBlue); heffmw [1]->SetLineColor(kACRed);
  heffjq [0]->SetLineColor(kACBlue); heffjq [1]->SetLineColor(kACRed);

  hgenjq [0]->SetLineStyle(2); hgenjq [1]->SetLineStyle(2);
  hmeasjq[0]->SetLineStyle(2); hmeasjq[1]->SetLineStyle(2);
  heffjq [0]->SetLineStyle(2); heffjq [1]->SetLineStyle(2);

  hgenmw[0]->SetMinimum(0);
  hgenjq[0]->SetMinimum(0);

  hgenmw [0]->SetTitle("MC Truth (MW);Energy [keV]");
  hmeasmw[1]->SetTitle("MC Measured (MW);Energy [keV]");
  hgenjq [0]->SetTitle("MC Truth (JQ);Energy [keV]");
  hmeasjq[1]->SetTitle("MC Measured (JQ);Energy [keV]");
  heffmw [0]->SetTitle("MC Efficiency (SiL);Energy [keV]");
  heffmw [1]->SetTitle("MC Efficiency (SiR);Energy [keV]");

  TLegend* leg_lrmw = new TLegend(0.7, 0.3, 0.9, 0.5);
  leg_lrmw->AddEntry(hmeasmw[0], "SiL");
  leg_lrmw->AddEntry(hmeasmw[1], "SiR");
  TLegend* leg_lrjq = new TLegend(0.7, 0.3, 0.9, 0.5);
  leg_lrjq->AddEntry(hmeasjq[0], "SiL");
  leg_lrjq->AddEntry(hmeasjq[1], "SiR");
  TLegend* leg_jqmwl = new TLegend(0.7, 0.3, 0.9, 0.5);
  leg_jqmwl->AddEntry(heffmw[0], "MW");
  leg_jqmwl->AddEntry(heffjq[0], "JQ");
  TLegend* leg_jqmwr = new TLegend(0.7, 0.3, 0.9, 0.5);
  leg_jqmwr->AddEntry(heffmw[1], "MW");
  leg_jqmwr->AddEntry(heffjq[1], "JQ");

  TCanvas* ctm = new TCanvas("ctm", "ctm", 1400, 1000);
  ctm->Divide(2, 2);
  ctm->cd(1); hrespmw[0]->Draw("COL");
  ctm->cd(2); hrespmw[1]->Draw("COL");
  ctm->cd(3); hrespjq[0]->Draw("COL");
  ctm->cd(4); hrespjq[1]->Draw("COL");
  ctm->SaveAs("img/comp_tms.png");

  hmeasmw[0]->SetStats(0); hmeasmw[1]->SetStats(0);
  hmeasjq[0]->SetStats(0); hmeasjq[1]->SetStats(0);

  TCanvas* cproj = new TCanvas("cproj", "cproj", 1400, 1000);
  cproj->Divide(2, 2);
  cproj->cd(1); hgenmw [0]->Draw(); hgenmw [1]->Draw("SAME");
  cproj->cd(2); hmeasmw[1]->Draw(); hmeasmw[0]->Draw("SAME"); leg_lrmw->Draw();
  cproj->cd(3); hgenjq [0]->Draw(); hgenjq [1]->Draw("SAME");
  cproj->cd(4); hmeasjq[1]->Draw(); hmeasjq[0]->Draw("SAME"); leg_lrjq->Draw();
  cproj->SaveAs("img/comp_proj.png");

  heffmw[0]->SetStats(0);
  heffmw[1]->SetStats(0);
  heffjq[0]->SetStats(0);
  heffjq[1]->SetStats(0);

  TCanvas* ceff = new TCanvas("ceff", "ceff", 1400, 500);
  ceff->Divide(2);
  ceff->cd(1); heffmw[0]->Draw(); heffjq[0]->Draw("SAME"); leg_jqmwl->Draw();
  ceff->cd(2); heffmw[1]->Draw(); heffjq[1]->Draw("SAME"); leg_jqmwr->Draw();
  ceff->SaveAs("img/comp_eff.png");
 }
