{
  TFile* fold = new TFile("~/data/R15b/mudist/mudist_al50_precolfix.root");
  TFile* fnew = new TFile("~/data/R15b/mudist/mudist_al50.root");

  TH3* h3old = (TH3*)fold->Get("mustopdist");
  TH3* h3new = (TH3*)fnew->Get("mustopdist");

  TH1* hxold = (TH1*)h3old->Project3D("x") ->Clone("hxold");
  TH1* hyold = (TH1*)h3old->Project3D("y") ->Clone("hyold");
  TH1* hzold = (TH1*)h3old->Project3D("z") ->Clone("hzold");
  TH1* hxnew = (TH1*)h3new->Project3D("x") ->Clone("hxnew");
  TH1* hynew = (TH1*)h3new->Project3D("y") ->Clone("hynew");
  TH1* hznew = (TH1*)h3new->Project3D("z") ->Clone("hznew");
  TH2* h2old = (TH2*)h3old->Project3D("yx")->Clone("h2old");
  TH2* h2new = (TH2*)h3new->Project3D("yx")->Clone("h2new");

  h2old->SetLineColor(kRed); h2new->SetLineColor(kBlack);
  h2old->SetFillStyle(kRed); h2new->SetFillStyle(kBlack);
  hxold->SetLineColor(kRed); hxnew->SetLineColor(kBlack);
  hyold->SetLineColor(kRed); hynew->SetLineColor(kBlack);
  hzold->SetLineColor(kRed); hznew->SetLineColor(kBlack);

  h2old->Rebin2D(5, 5); h2new->Rebin2D(5, 5);

  h2new->SetTitle("MC #mu-stop Target XY Distribution;X [mm];Y [mm]");
  hxnew->SetTitle("MC #mu-stop Target X Distribution;X [mm];Arb.");
  hynew->SetTitle("MC #mu-stop Target Y Distribution;Y [mm];Arb.");
  hznew->SetTitle("MC #mu-stop Target Z Distribution;Z [mm];Arb.");
  hznew->GetXaxis()->SetRangeUser(-0.025, 0.025);

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hxold, "Pre-Fix",  "l");
  l->AddEntry(hxnew, "Post-Fix", "l");

  gStyle->SetOptStat(0x0);
  TCanvas* c = new TCanvas("c", "c", 1400, 1000);
  c->Divide(2, 2);
  c->cd(1); h2new->DrawNormalized("BOX SAME"); h2old->DrawNormalized("BOX SAME"); l->Draw();
  c->cd(2); hynew->DrawNormalized("SAME");     hyold->DrawNormalized("SAME");     l->Draw();
  c->cd(3); hxnew->DrawNormalized("SAME");     hxold->DrawNormalized("SAME");     l->Draw();
  c->cd(4); hznew->DrawNormalized("SAME");     hzold->DrawNormalized("SAME");     l->Draw();
  c->SaveAs("img/compare_precolfix_sim_xyzdist.png");
}
