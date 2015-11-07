void v1290_rawtimes_0_8()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(3,3);

  const int nplots = 9;
  const std::string bank_names[nplots] = {"T400", "T401", "T402", "T403", "T404", "T405", "T406", "T407", "T408"};

  for (int iplot = 0; iplot < nplots; ++iplot) {
    char histname[64]; sprintf(histname, "h%s_RawTime", bank_names[iplot].c_str());
    TH1* hist = getObject(histname);    
    if (hist) {
      TVirtualPad *pad = AlCapCanvas->cd(iplot+1);
      hist->Draw();
      pad->SetLogy(1);
      pad->SetLogx(0);
    }
  }
}
