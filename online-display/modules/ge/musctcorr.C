void musctcorr()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");

  TH1* hists[9];
  string hist_names[9] = { "hTCorrMuScGe1CF",  "hTCorrMuScGe2CF",
			   "hTCorrMuScGe1LE",  "hTCorrMuScGe2LE",
			   "hTCorrMuScESc1Lo", "hTCorrMuScESc1Hi",
			   "hTCorrMuScESc2Lo", "hTCorrMuScESc2Hi",
			   "hTCorrMuScMuScA" };
  for (int i = 0; i < 9; ++i) {
    hists[i] = (TH1*)getObject(hist_names[i].c_str());
    hists[i]->SetLineColor(kBlack);
    hists[i]->SetStats(0);
  }

  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(3,3);
  for (int i = 0; i < 9; ++i) {
    AlCapCanvas->cd(i+1);
    hists[i]->Draw();
  }
}
