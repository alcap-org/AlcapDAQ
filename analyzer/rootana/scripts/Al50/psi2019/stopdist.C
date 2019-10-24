{
  TChain* tr = new TChain("tree");
  tr->Add("data/mc/mubeam/R15bAl50MuPreSiT.1.root");
  tr->Add("data/mc/mubeam/R15bAl50MuPreSiT.2.root");
  tr->Add("data/mc/mubeam/R15bAl50MuPreSiT.3.root");
  TH1* h = new TH1D("h", "Muon Stopping Dsitribution (Simulation);Stopping Depth in Target [#mum];#mu-Stops/#mum", 61, -30.5, 30.5);
  tr->Draw("1e4*M_local_Oz>>h", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "goff");

  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetMaximum(11000);
  h->SetMinimum(0);
  TLine* l[2]   = { new TLine(-25, 0, -25, 11e3), new TLine(25, 0, 25, 11e3) };
  TText* txt[2] = { new TText(-24, 6e3, "Front"), new TText(18, 6e3, "Back") };

  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  h->Draw();
  l  [0]->Draw(); l  [1]->Draw();
  txt[0]->Draw(); txt[1]->Draw();
  c->SaveAs("img/psi2019/stopdist.png");
}
