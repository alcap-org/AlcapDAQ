void IslandLoss()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat(""); 
  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  canvas->Clear();

  /*****************************************************************/
  TH2 *hExpected = (TH2 *)gDirectory->Get("hExpectedNumberOfIslands");
  TH2 *hActual = (TH2 *)gDirectory->Get("hNumberOfIslands");
  if (hExpected && hActual)
    {
      TVirtualPad *pad = canvas->cd(1);

      hExpected->Add(hActual, -1);
      hExpected->SetTitle("Difference between Expected Number of TPulseIslands and Actual Number of TPulseIslands");
      hExpected->GetXaxis()->SetRange(9,9);
      hExpected->GetYaxis()->SetRangeUser(0, 1200);
      hExpected->Draw("COLZ");
    }
}
