{
  TFile* ftm = new TFile("data/tmal50.root");

  SiL_TransTree->Draw("detseg>>hdetl(16,0.5,16.5)");
  SiR_TransTree->Draw("detseg>>hdetlinr(4,0.5,4.5)");
  SiL_TransTree->Draw("y0:x0>>htargl");
  SiR_TransTree->Draw("y0:x0>>htargr");
  TH2* hdetr = new TH2I("hdetr", "", 2, 0.5, 2.5, 2, 0.5, 2.5);
  hdetr->SetBinContent(1, 2, hdetlinr->GetBinContent(1));
  hdetr->SetBinContent(2, 2, hdetlinr->GetBinContent(2));
  hdetr->SetBinContent(2, 1, hdetlinr->GetBinContent(3));
  hdetr->SetBinContent(1, 1, hdetlinr->GetBinContent(4));
  hdetl->SetTitle("SiL MC Hit Distribution (Uniform proton pos&E Generation);Segment");
  hdetr->SetTitle("SiR MC Hit Distribution (Uniform proton pos&E Generation);Quadrant");

  TText* hdetr_t1 = new TText(1, 2, hdetr->GetBinContent(1, 2));
  TText* hdetr_t2 = new TText(2, 2, hdetr->GetBinContent(2, 2));
  TText* hdetr_t3 = new TText(2, 1, hdetr->GetBinContent(2, 1));
  TText* hdetr_t4 = new TText(1, 1, hdetr->GetBinContent(1, 1));

  TCanvas* c = new TCanvas("c", "Detector Distribution", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hdetl->Draw();
  c->cd(2);
  hdetr->Draw("BOX");
  hdetr_t1->Draw(); hdetr_t2->Draw(); hdetr_t3->Draw(); hdetr_t4->Draw();
}
