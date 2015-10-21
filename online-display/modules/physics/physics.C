void physics()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  gStyle->SetPalette(1);
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
  
  // Do the dEdx plots first
  AlCapCanvas->cd(1);
  TH2* left_hist = (TH2 *)getObject("hdEdx_SiL");
  left_hist->Draw("COLZ");

  AlCapCanvas->cd(2);
  TH2* right_hist = (TH2 *)getObject("hdEdx_SiR");
  right_hist->Draw("COLZ");
  

  // Possible x-rays we might be interested in
  double al_xray = 346.8;
  double si_xray = 400.2;
  double xray = al_xray; // the one we actually want

  TLine* xray_line = new TLine(xray, 0, xray, 1);
  xray_line->SetLineColor(kRed);
  xray_line->SetLineWidth(2);
  xray_line->SetLineStyle(2);

  // Now do the full germanium spectrum
  AlCapCanvas->cd(3);
  gPad->SetLogy();
  TH1* ge_full = (TH1 *)getObject("hGeSpectrum");
  TH1* ge_zoom = (TH1 *)ge_full->Clone("hGeSpectrum_Zoom");

  ge_full->Rebin(10);
  ge_full->SetXTitle("Energy [keV]");
  ge_full->SetYTitle("Counts per 10 keV");
  ge_full->Draw();

  xray_line->SetY2(ge_full->GetMaximum());
  xray_line->Draw("LSAME");

  // Now zoom in around the xray we want
  AlCapCanvas->cd(4);
  ge_zoom->GetXaxis()->SetRangeUser(xray-10, xray+10);
  ge_zoom->Draw();

  TLine* xray_line_zoom = (TLine*) xray_line->Clone();
  xray_line_zoom->SetY2(ge_zoom->GetMaximum());
  xray_line_zoom->Draw("LSAME");
}
