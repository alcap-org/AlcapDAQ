{
  TFile* f = new TFile("data/Al50/tm.p.2layer.mudist.root");
  RooUnfoldResponse* resp = (RooUnfoldResponse*)f->Get("SiR_TM");
  TH2* h = resp->Hresponse();
  TPaveText* txt = new TPaveText(10e3, 2e3, 14e3, 5e3);
  txt->AddText("Al 50#mum Target");
  h->SetTitle("Simulated SiR Proton Response;E_{meas} [keV];E_{true} [keV]");
  h->GetXaxis()->SetRangeUser(0, 15e3);
  h->GetYaxis()->SetRangeUser(0, 15e3);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetTitleOffset(0.90);
  h->GetYaxis()->SetTitleOffset(1.0);

  TLine* l = new TLine(  0, 6e3, 6e3, 6e3);
  TBox*  b = new TBox (4e3,   0, 6e3, 6e3);

  l->SetLineColor(kBlack);
  b->SetFillColor(kBlack);
  l->SetLineWidth(5);
  b->SetFillStyle(3352);

  TLegend* leg = new TLegend(0.62, 0.4, 0.9, 0.6);
  leg->SetHeader("Example: 6 MeV");
  leg->AddEntry(l, "True Energy",     "l");
  leg->AddEntry(b, "Measured Energy", "f");

  gStyle->SetOptStat("e");

  TCanvas* c = new TCanvas();
  h->Draw("COL");
  txt->Draw();
  l->Draw();
  b->Draw();
  leg->Draw();
  c->SaveAs("img/psi2019/tm.png");
}
