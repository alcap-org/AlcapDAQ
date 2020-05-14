{
  TFile* f[11] = {
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.6MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.7MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.8MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom25.9MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.0MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.1MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.2MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.3MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.4MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.5MeV.1.root"),
    new TFile("data/mc/momscan/R15bAl50MuPreSiT.mom26.6MeV.1.root"),
  };
  TTree* tr[11] = {
    (TTree*)f[0]->Get("tree"), (TTree*)f[1]->Get("tree;83"),
    (TTree*)f[2]->Get("tree"), (TTree*)f[3]->Get("tree"),
    (TTree*)f[4]->Get("tree"), (TTree*)f[5]->Get("tree"),
    (TTree*)f[6]->Get("tree"), (TTree*)f[7]->Get("tree"),
    (TTree*)f[8]->Get("tree"), (TTree*)f[9]->Get("tree"),
    (TTree*)f[10]->Get("tree")
  };
  double n[11];
  TH1* hstopdep[11];
  for (int i = 0; i < 11; ++i) {
    n [i] = tr[i]->GetEntries("200<1e6*M_edep && M_particleName==\"mu-\" && M_volName==\"SiT\"");
    char hname[32], cmd[64];
    sprintf(hname, "hstopdepth%d", i);
    sprintf(cmd, "1e4*M_local_Oz>>%s", hname);
    hstopdep[i] = new TH1D(hname, "MC Stopping Depth in Al50 Target;Target local z position [#mum];Fraction #mu stopping in target", 60, -30, 30);
    tr[i]->Draw(cmd, "M_stopped==1 && M_particleName==\"mu-\" && M_volName==\"Target\"", "goff");
    hstopdep[i]->Scale(1./n[i]);
  }

  TLegend* leg = new TLegend(0.45, 0.1, 0.9, 0.5);
  leg->SetNColumns(2);
  char str[32];
  sprintf(str, "25.6 MeV/c, %d%% stop", int(100.*hstopdep[0]->GetEntries()/n[0]));
  leg->AddEntry(hstopdep[0], str);
  sprintf(str, "25.7 MeV/c, %d%% stop", int(100.*hstopdep[1]->GetEntries()/n[1]));
  leg->AddEntry(hstopdep[1], str);
  sprintf(str, "25.8 MeV/c, %d%% stop", int(100.*hstopdep[2]->GetEntries()/n[2]));
  leg->AddEntry(hstopdep[2], str);
  sprintf(str, "25.9 MeV/c, %d%% stop", int(100.*hstopdep[3]->GetEntries()/n[3]));
  leg->AddEntry(hstopdep[3], str);
  sprintf(str, "26.0 MeV/c, %d%% stop", int(100.*hstopdep[4]->GetEntries()/n[4]));
  leg->AddEntry(hstopdep[4], str);
  sprintf(str, "26.1 MeV/c, %d%% stop", int(100.*hstopdep[5]->GetEntries()/n[5]));
  leg->AddEntry(hstopdep[5], str);
  sprintf(str, "26.2 MeV/c, %d%% stop", int(100.*hstopdep[6]->GetEntries()/n[6]));
  leg->AddEntry(hstopdep[6], str);
  sprintf(str, "26.3 MeV/c, %d%% stop", int(100.*hstopdep[7]->GetEntries()/n[7]));
  leg->AddEntry(hstopdep[7], str);
  sprintf(str, "26.4 MeV/c, %d%% stop", int(100.*hstopdep[8]->GetEntries()/n[8]));
  leg->AddEntry(hstopdep[8], str);
  sprintf(str, "26.5 MeV/c, %d%% stop", int(100.*hstopdep[9]->GetEntries()/n[9]));
  leg->AddEntry(hstopdep[9], str);
  sprintf(str, "26.6 MeV/c, %d%% stop", int(100.*hstopdep[10]->GetEntries()/n[10]));
  leg->AddEntry(hstopdep[10], str);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  hstopdep[3]->SetLineColor(kBlack);
  hstopdep[3]->Draw();
  for (int i = 0; i < 11; ++i)
    if (i != 3)
      hstopdep[i]->Draw("SAME PLC");
  leg->Draw();
  c->SaveAs("img/thesis/5/sistopdep.png");
}
