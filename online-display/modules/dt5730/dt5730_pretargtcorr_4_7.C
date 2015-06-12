void dt5730_pretargtcorr_4_7()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);

  const int nplots = 4;
  const std::string bank_names[nplots] = {"D704", "D705", "D706", "D707"};

  for (int iplot = 0; iplot < nplots; ++iplot) {
    char histname[64]; sprintf(histname, "hPreTargTCorrWFD_%s", bank_names[iplot]);
    TH1* hist = getObject(histname);
    if (hist) {
  	  TVirtualPad *pad = AlCapCanvas->cd(iplot+1);
  	  hist->Draw();
  	  pad->SetLogy(1);
  	  pad->SetLogx(0);
  	}
  }
}
