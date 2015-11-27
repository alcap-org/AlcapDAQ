void vetotcorr()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");

  TH1* hists[2];
  string hist_names[2] = { "hTCorrGe1ESc1", "hTCorrGe2ESc2" };
  for (int i = 0; i < 2; ++i) {
    hists[i] = (TH1*)getObject(hist_names[i].c_str());
    hists[i]->SetLineColor(kBlack);
    hists[i]->SetStats(0);
  }

  TPaveText* txt[2];
  for (int i = 0; i < 2; ++i) {
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
  AlCapCanvas->Divide(2);
  for (int i = 0; i < 2; ++i) {
    AlCapCanvas->cd(i+1);
    hists[i]->Draw();
    txt[i]->Draw();
  }
}
