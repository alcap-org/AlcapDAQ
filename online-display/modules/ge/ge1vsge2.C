void ge1vsge2()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();

  TH2* h = (TH2*)getObject("hGeEVsE");
  h->Draw("COLZ");
  AlCapCanvas->SetLogz();
}
