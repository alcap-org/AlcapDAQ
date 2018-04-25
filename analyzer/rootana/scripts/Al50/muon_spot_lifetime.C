{
  TCanvas* c = new TCanvas();
  TChain* ch = new TChain();
  ch->Add("data/pselal50.root/PID_R1P");
  ch->Add("data/pselal50.root/PID_R1D");
  ch->Add("data/pselal50.root/PID_R1T");
  ch->Add("data/pselal50.root/PID_R1A");
  ch->Add("data/pselal50.root/PID_R1U");
  ch->Add("data/pselal50.root/PID_R2P");
  ch->Add("data/pselal50.root/PID_R2D");
  ch->Add("data/pselal50.root/PID_R2T");
  ch->Add("data/pselal50.root/PID_R2A");
  ch->Add("data/pselal50.root/PID_R2U");
  ch->Add("data/pselal50.root/PID_R3P");
  ch->Add("data/pselal50.root/PID_R3D");
  ch->Add("data/pselal50.root/PID_R3T");
  ch->Add("data/pselal50.root/PID_R3A");
  ch->Add("data/pselal50.root/PID_R3U");
  ch->Add("data/pselal50.root/PID_R4P");
  ch->Add("data/pselal50.root/PID_R4D");
  ch->Add("data/pselal50.root/PID_R4T");
  ch->Add("data/pselal50.root/PID_R4A");
  ch->Add("data/pselal50.root/PID_R4U");

  TBox* muspot1 = new TBox(1200, 600, 2000, 1200);
  TBox* muspot2 = new TBox(900, 300,  1500,  800);
  muspot1->SetLineWidth(3);
  muspot2->SetLineWidth(3);
  ch->Draw("de:e>>hall(200, 0, 5e3, 108, 300, 3e3)");
  ch->Draw("de:e>>hearly(200, 0, 5e3, 108, 300, 3e3)",  "t<400");
  ch->Draw("de:e>>hlate(200, 0, 5e3, 108, 300, 3e3)",   "400<t");
  ch->Draw("de:e>>hprompt(200, 0, 5e3, 108, 300, 3e3)", "-200<t && t<200");
  ch->Draw("t>>htimemu1(150,-500,1000)",  "600<de && de<1200 && 1200<e && e<2000");
  ch->Draw("t>>htimemu2(150,-500,1000)", "300<de && de<800  && 900<e  && e<1500");
  hall    ->SetTitle("SiR dE vs E (No time cut);E [keV];dE [keV]");
  hprompt ->SetTitle("SiR dE vs E (-200ns<t && t<200ns);E [keV];dE [keV]");
  hearly  ->SetTitle("SiR dE vs E (t<400ns);E [keV];dE [keV]");
  hlate   ->SetTitle("SiR dE vs E (400ns<t);E [keV];dE [keV]");
  htimemu1->SetTitle("SiR Muon Spot Time;t [ns];Height Normalized");
  htimemu2->SetTitle("SiR Muon Spot Time;t [ns];Height Normalized");
  htimemu1->SetStats(false);
  htimemu2->SetStats(false);
  htimemu1->SetLineColor(1);
  htimemu2->SetLineColor(2);
  muspot1->SetFillStyle(0);
  muspot2->SetFillStyle(0);
  muspot1->SetLineColor(1);
  muspot2->SetLineColor(2);

  TCanvas* cevde = new TCanvas("cevde", "EvdE Plots", 1400, 1000);
  cevde  ->Divide(2,2);
  cevde  ->cd(1);
  hall   ->Draw("COL");
  cevde  ->cd(2);
  hprompt->Draw("COL");
  cevde  ->cd(3);
  hearly ->Draw("COL");
  cevde  ->cd(4);
  hlate  ->Draw("COL");
  cevde  ->SaveAs("img/evde_r_tcuts.png");

  TCanvas* ct = new TCanvas("ct", "Muon Spots", 1400, 500);
  ct->Divide(2);
  ct->cd(1);
  hall->Draw("COL");
  muspot1->Draw();
  muspot2->Draw();
  int nmu1_all  = htimemu1->GetEntries();
  int nmu2_all  = htimemu2->GetEntries();
  int nmu1_tcut = htimemu1->Integral(htimemu1->FindFixBin(-200),
                                     htimemu1->FindFixBin(200));
  int nmu2_tcut = htimemu2->Integral(htimemu2->FindFixBin(-200),
                                     htimemu2->FindFixBin(200));
  TText* nmutitle = new TText(200, 0.7, "Total (Prompt)");
  TText* nmu1lab  = new TText(200, 0.5,
                              TString::Format("%d (%d)", nmu1_all, nmu1_tcut));
  TText* nmu2lab  = new TText(200, 0.3,
                              TString::Format("%d (%d)", nmu2_all, nmu2_tcut));
  nmu1lab ->SetTextColor(1);
  nmu2lab ->SetTextColor(2);
  htimemu1->Scale(1./htimemu1->GetMaximum());
  htimemu2->Scale(1./htimemu2->GetMaximum());
  ct      ->cd(2);
  htimemu1->Draw();
  htimemu2->Draw("SAME");
  nmutitle->Draw();
  nmu1lab ->Draw();
  nmu2lab ->Draw();
  ct      ->SaveAs("img/t_muspots.png");
}
