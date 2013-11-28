void IslandLoss()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne"); 
  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  canvas->Clear();

  /*****************************************************************/
  TH2 *hExpected = (TH2 *)gDirectory->Get("hExpectedNumberOfIslands");
  TH2 *hActual = (TH2 *)gDirectory->Get("hNOctalFADCIslandsReadPerBlock");
  if (hExpected && hActual)
    {
      TVirtualPad *pad = canvas->cd(1);
      hExpected->Add(hActual, -1);
      hExpected->Draw("COLZ");
    }
}
