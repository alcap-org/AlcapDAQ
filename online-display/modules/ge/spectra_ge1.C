void spectra_ge1()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();

  TH2* h = (TH2*)getObject("h2Ge1SpectraMuCentered");
  h->Draw("COLZ");
  AlCapCanvas->SetLogz();
}
