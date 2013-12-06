void muSC()
{
  /*****************************************************************/
  // Prepare the canvas

  //gROOT->LoadMacro("palette.cxx");
  //SetRainbowPalette();
  // *** VT 10-Nov-2005
  //gStyle->SetOptStat("ner");
  gStyle->SetOptStat(1111);
  TCanvas *MuSunCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  MuSunCanvas->Clear();
  MuSunCanvas->Divide(2,2);  
 
  MuSunCanvas->cd(1);

  TH1 *hmusc = (TH1*) getObject("muSC_count_raw");
  hmusc->Draw("");
 
}

