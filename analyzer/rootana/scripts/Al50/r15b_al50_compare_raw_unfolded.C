// Compare unfolded and raw energy spectra.
{
	TFile* fraw = new TFile("data/pselal50_102.root");
  TFile* frec = new TFile("data/enal50p_102.root");

  TTree* tr[2] = { (TTree*)fraw->Get("PID_LP"), (TTree*)fraw->Get("PID_RP") };
  tr[0]->Draw("e>>hlraw(75,0,15e3)", "t > 400 && abs(dt) < 200");
  tr[1]->Draw("e>>hrraw(75,0,15e3)", "t > 400 && abs(dt) < 200");
  TH1* hraw[2] = { (TH1*)gDirectory->Get("hlraw"),
                   (TH1*)gDirectory->Get("hrraw") };

  TH1* hrec[2] = { (TH1*)frec->Get("hl_e_reco"), (TH1*)frec->Get("hr_e_reco") };

  gStyle->SetOptStat(0);
  hrec[0]->SetTitle("Al50 Left Proton Spectrum;Energy [keV];Integral Normalized");
  hrec[1]->SetTitle("Al50 Right Proton Spectrum;Energy [keV];Integral Normalized");
  hrec[0]->GetYaxis()->SetTitleOffset(1.4);
  hrec[1]->GetYaxis()->SetTitleOffset(1.4);
  hraw[0]->SetLineColor(kBlack);
  hraw[1]->SetLineColor(kBlack);
  hrec[0]->SetLineColor(kBlack);
  hrec[1]->SetLineColor(kBlack);
  hrec[0]->SetLineStyle(2);
  hrec[1]->SetLineStyle(2);
  hraw[0]->Scale(1/hraw[0]->Integral());
  hraw[1]->Scale(1/hraw[1]->Integral());
  hrec[0]->Scale(1/hrec[0]->Integral());
  hrec[1]->Scale(1/hrec[1]->Integral());

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hraw[0], "Raw", "lpf");
  leg->AddEntry(hrec[0], "Unfolded", "l");

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hrec[0]->Draw();
  hraw[0]->Draw("SAME");
  leg->Draw();
  c->cd(2);
  hrec[1]->Draw();
  hraw[1]->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/raw_unfolded_compare.png");
}
