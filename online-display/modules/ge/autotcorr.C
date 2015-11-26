void autotcorr()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");

  TH1* hists[4];
  string hist_names[4] = { "hTCorrGe1TCFVsE", "hTCorrGe2TCFVsE",
			   "hTCorrGe1TLEVsE", "hTCorrGe2TLEVsE" };
  for (int i = 0; i < 4; ++i) {
    hists[i] = ((TH2*)getObject(hist_names[i].c_str()))->ProjectionX();
    hists[i]->SetLineColor(kBlack);
    hists[i]->SetStats(0);
  }
  hists[0]->SetTitle("Ge1 WFD-TDC(CF) Timing;T_{WFD}-T_{TDC} (ns)");
  hists[1]->SetTitle("Ge2 WFD-TDC(CF) Timing;T_{WFD}-T_{TDC} (ns)");
  hists[2]->SetTitle("Ge1 WFD-TDC(LE) Timing;T_{WFD}-T_{TDC} (ns)");
  hists[3]->SetTitle("Ge2 WFD-TDC(LE) Timing;T_{WFD}-T_{TDC} (ns)");

  TPaveText* txt[4];
  for (int i = 0; i < 4; ++i) {
    txt[i] = new TPaveText(0.1, 0.2, 0.4, 0.9, "NDC BR");
    TFitResultPtr res = hists[i]->Fit("gaus", "S");
    char str[256];
    int j = 0;
    sprintf(str, "Offset: %g ns", res->Parameter(1));
    txt[i]->AddText(str);
    sprintf(str, "FWHM: %.2f ns", 2.35482*res->Parameter(2));
    txt[i]->AddText(str);
    sprintf(str, "#Chi^{2}/NDF: %.2f/%d", res->Chi2(), res->Ndf());
    txt[i]->AddText(str);
  }

  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  for (int i = 0; i < 4; ++i) {
  AlCapCanvas->cd(i+1);
  hists[i]->Draw();
  txt[i]->Draw();
  }
}
