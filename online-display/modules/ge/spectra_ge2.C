void spectra_ge2()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();

  TH2* h = (TH2*)getObject("h2Ge2SpectraMuCentered");
  h->Draw("COLZ");
  AlCapCanvas->SetLogz();
}
