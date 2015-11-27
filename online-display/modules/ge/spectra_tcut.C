void spectra_tcut()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(1,2);

  TH2* hge1_spectrum = (TH2*)getObject("h2Ge1SpectraMuCentered");
  TH2* hge2_spectrum = (TH2*)getObject("h2Ge2SpectraMuCentered");

  TH1* hge1_all = hge1_spectrum->ProjectionY("hge1_all");
  TH1* hge1_pws = hge1_spectrum->ProjectionY("hge1_promptwithstop",
					     hge1_spectrum->GetXaxis()->FindBin(50.),
					     hge1_spectrum->GetXaxis()->FindBin(250.));
  TH1* hge1_pwc = hge1_spectrum->ProjectionY("hge1_promptwithcapture",
					     hge1_spectrum->GetXaxis()->FindBin(250.),
					     hge1_spectrum->GetXaxis()->FindBin(2250.));
  TH1* hge1_del = hge1_spectrum->ProjectionY("hge1_delayed",
					     hge1_spectrum->GetXaxis()->FindBin(2250.), -1);
  hge1_del->Add(hge1_spectrum->ProjectionY("tmp1", 0, hge1_spectrum->GetXaxis()->FindBin(50.)));

  TH1* hge2_all = hge2_spectrum->ProjectionY("hge2_all");
  TH1* hge2_pws = hge2_spectrum->ProjectionY("hge2_promptwithstop", hge2_spectrum->GetXaxis()->FindBin(140.), hge2_spectrum->GetXaxis()->FindBin(300.));
  TH1* hge2_pwc = hge2_spectrum->ProjectionY("hge2_promptwithcapture",
					     hge2_spectrum->GetXaxis()->FindBin(350.),
					     hge2_spectrum->GetXaxis()->FindBin(2350.));
  TH1* hge2_del = hge2_spectrum->ProjectionY("hge2_delayed",
					     hge2_spectrum->GetXaxis()->FindBin(2350.), -1);
  hge2_del->Add(hge2_spectrum->ProjectionY("tmp2", 0, hge2_spectrum->GetXaxis()->FindBin(140.)));
  
  hge1_all->SetLineColor(kBlack); hge2_all->SetLineColor(kBlack);
  hge1_pws->SetLineColor(kRed);   hge2_pws->SetLineColor(kRed);
  hge1_pwc->SetLineColor(kGreen); hge2_pwc->SetLineColor(kGreen);
  hge1_del->SetLineColor(kBlue);  hge2_del->SetLineColor(kBlue);

  hge1_all->SetStats(0); hge2_all->SetStats(0);
  hge1_pws->SetStats(0); hge2_pws->SetStats(0);
  hge1_pwc->SetStats(0); hge2_pwc->SetStats(0);
  hge1_del->SetStats(0); hge2_del->SetStats(0);
  
  TLegend* leg = new TLegend(0.7, 0.5, 0.9, 0.9);
  leg->SetHeader("Time from muon");
  leg->AddEntry(hge1_all, "No Time Cuts");
  leg->AddEntry(hge1_pws, "-100-100 ns");
  leg->AddEntry(hge1_pwc, "100-2000 ns");
  leg->AddEntry(hge1_del, ">2#mus");

  AlCapCanvas->cd(1)->SetLogy();
  hge1_all->Draw();       hge1_pws->Draw("SAME");
  hge1_pwc->Draw("SAME"); hge1_del->Draw("SAME");
  leg->Draw("SAME");
  AlCapCanvas->cd(2)->SetLogy();
  hge2_all->Draw();       hge2_pws->Draw("SAME");
  hge2_pwc->Draw("SAME"); hge2_del->Draw("SAME");
  leg->Draw("SAME");
  
}
