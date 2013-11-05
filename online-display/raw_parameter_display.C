raw_parameter_display()
{
  /*****************************************************************/
  // Prepare the canvas

  gStyle->SetOptStat("ne");

  TCanvas *MuSunCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  MuSunCanvas->Divide(3,3);

  /*****************************************************************/

  MuSunCanvas->cd(1);
  TH1 *parameters_muSC = getHist("muSC_count_raw");
  parameters_muSC->SetFillColor(kBlack);
  int muSCtot=parameters_muSC->Integral();
  printf("muSC_count_raw: %d \n",muSCtot);
  parameters_muSC->Draw();

  TH1 *parameters_all = getHist("hParameters_All");
  MuSunCanvas->cd(2);
  gPad->SetLogy();
  TH1 *parameters_muPC1 = extractParameters(parameters_all, "muPC", 4001, 4074);
  parameters_muPC1->SetFillColor(kBlack);
  parameters_muPC1->Draw();
}

TH1 *extractParameters(TH1 *parametersAll, char *title, int first, int last)
{
  char name[80];
  sprintf(name, "hPar%d_%d", first, last);
  TH1D *h = new TH1D(name, title, last-first+1, first-0.5, last+0.5);

  for(int i = first; i <= last; i++) 
  {
    h->SetBinContent(i-first+1, parametersAll->GetBinContent(i));
  }

  return h;
}
