{
  Int_t kACBlue = TColor::GetColor("#67a9cf");
  Int_t kACRed  = TColor::GetColor("#ef8a62");

  TFile* fmw = new TFile("~m-wong/data/R15b/al50.root");
  TFile* fjq = new TFile("~jrquirk/data/R15b/Al50/psel.3layer.pidpow2.root");

  TTree* trmw = (TTree*)fmw->Get("tree");
  TTree* trjq[2] = {
    (TTree*)fjq->Get("PID_LP"),
    (TTree*)fjq->Get("PID_RP")
  };

  trmw   ->Draw("e1+e2>>hemwl", "channel.Contains(\"SiL\") && sig1.Contains(\"proton\") && t1>400 && abs(t2-t1)<200 && e3<=0", "goff");
  trmw   ->Draw("e1+e2>>hemwr", "channel.Contains(\"SiR\") && sig1.Contains(\"proton\") && t1>400 && abs(t2-t1)<200 && e3<=0", "gooff");
  trjq[0]->Draw("e1+e2>>hejql", "abs(t2-t1)<200 && t1>400 && e3==0", "goff");
  trjq[1]->Draw("e1+e2>>hejqr", "abs(t2-t1)<200 && t1>400 && e3==0", "goff");

  hemwl->SetLineColor(kACBlue);
  hemwr->SetLineColor(kACRed);
  hejql->SetLineColor(kACBlue);
  hejqr->SetLineColor(kACRed);
  hejql->SetLineStyle(2);
  hejqr->SetLineStyle(2);
  hemwl->Draw();
  hejql->Draw("SAME");
}
