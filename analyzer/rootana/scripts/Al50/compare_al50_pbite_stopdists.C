{
  vector<TFile*> fs = {
    new TFile("out/mudists/mudist_al50_pbite0.50.root"),
    new TFile("out/mudists/mudist_al50_pbite0.60.root"),
    new TFile("out/mudists/mudist_al50_pbite0.70.root"),
    new TFile("out/mudists/mudist_al50_pbite0.80.root"),
    new TFile("out/mudists/mudist_al50_pbite0.90.root"),
    new TFile("out/mudists/mudist_al50_pbite1.00.root"),
    new TFile("out/mudists/mudist_al50_pbite1.10.root"),
    new TFile("out/mudists/mudist_al50_pbite1.20.root"),
    new TFile("out/mudists/mudist_al50_pbite1.30.root"),
    new TFile("out/mudists/mudist_al50_pbite1.40.root"),
    new TFile("out/mudists/mudist_al50_pbite1.50.root"),
    new TFile("out/mudists/mudist_al50_pbite1.60.root"),
    new TFile("out/mudists/mudist_al50_pbite1.70.root"),
    new TFile("out/mudists/mudist_al50_pbite1.80.root"),
    new TFile("out/mudists/mudist_al50_pbite1.90.root"),
    new TFile("out/mudists/mudist_al50_pbite2.00.root") };
  const int N = fs.size();

  vector<TH3*> h3s;
  vector<TH2*> hyxs;
  vector<TH1*> hxs, hys, hzs;
  for (auto f : fs) {
    TH3* h3 = (TH3*)f->Get("mustopdist");
    h3->SetTitle("#mu Stop Distribution;Local x [mm];Local y [mm];Local z [mm]");
    h3s .push_back(h3);
    // char hyxname[16], hxname[16], hyname[16], hzname[16];
    hyxs.push_back((TH2*)h3->Project3D("yx")->Clone());
    hxs .push_back((TH1*)h3->Project3D("x") ->Clone());
    hys .push_back((TH1*)h3->Project3D("y") ->Clone());
    hzs .push_back((TH1*)h3->Project3D("z") ->Clone());
    hzs.back()->GetXaxis()->SetRangeUser(-0.025, 0.025);
  }

  TLegend* leg = new TLegend(0.2, 0.1, 0.8, 0.9);
  leg->SetHeader("Stoppinng Rates");
  for (int i = 0; i < N; ++i) {
    char legentry[64];
    sprintf(legentry, "%d%% stopped @ dp=%d keV",
            int(100.*hxs[i]->GetEntries()/12e6),
            int(100.*(0.5+0.1*i)));
    leg->AddEntry(hxs[i], legentry, "l");
  }

  hxs.front()->SetMaximum(hxs.back()->GetMaximum());
  hys.front()->SetMaximum(hys.back()->GetMaximum());
  hzs.front()->SetMaximum(hzs.back()->GetMaximum());

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 1000);
  c->Divide(2, 2);
  c->cd(1); for (auto h : hxs) h->Draw("SAME PLC");
  c->cd(2); for (auto h : hys) h->Draw("SAME PLC");
  c->cd(3); for (auto h : hzs) h->Draw("SAME PLC");
  c->cd(4); leg->Draw();
}
