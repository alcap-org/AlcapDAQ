{
  TFile* f = new TFile("data/Al50/tm.2layer.root");
  TH2* tml = SiL_TM->Hresponse();
  TH2* tmr = SiR_TM->Hresponse();
  tml->SetTitle("SiL Transfer Matrix;E_{meas} [keV];E_{true} [keV]");
  tmr->SetTitle("SiR Transfer Matrix;E_{meas} [keV];E_{true} [keV]");

  tml->GetYaxis()->SetTitleOffset(1.4);
  tmr->GetYaxis()->SetTitleOffset(1.4);

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  tml->Draw("COL");
  c->cd(2);
  tmr->Draw("COL");
  c->SaveAs("img/tms.png");

}
