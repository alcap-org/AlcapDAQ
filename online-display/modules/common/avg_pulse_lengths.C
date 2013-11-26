void avg_pulse_lengths()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne"); 
  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  canvas->Clear();
  /*****************************************************************/
  TH2 *h1 = (TH2 *)gDirectory->Get("hAvgPulseLengthsPerChannel");
  if (h1)
    {
      TVirtualPad *pad = canvas->cd(1);
      h1->Draw("COLZ");
    }
}
