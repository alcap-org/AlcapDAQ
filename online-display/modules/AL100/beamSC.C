void beamSC()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,3);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  AlCapCanvas->cd(1);
  TH1 * hist = get_histogram("D700","Heights");
  hist->DrawClone("");
  hist->SetAxisRange(0,20000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);

  AlCapCanvas->cd(2);
  TH1 * hist = get_histogram("D701","Heights");
  hist->SetAxisRange(0,3000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);
  hist->DrawClone("");

  AlCapCanvas->cd(3);
  TH1 * hist = get_histogram("D700","Times");
  //hist->SetAxisRange(0,20000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);
  hist->DrawClone("");

  AlCapCanvas->cd(4);
  TH1 * hist = get_histogram("D701","Times");
  hist->DrawClone("");
  //hist->SetAxisRange(0,3000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);

  AlCapCanvas->cd(5);
  TH2D * hist2 = get_histogram("D700","Shapes");
  hist2->GetXaxis()->SetRangeUser(0.,100.);
  gPad->SetLogz(1);
  hist2->DrawClone("col");

  AlCapCanvas->cd(6);
  TH2D * hist3 = get_histogram("D701","Shapes");
  hist3->GetXaxis()->SetRangeUser(0.,100.);
  hist3->GetYaxis()->SetRangeUser(14000.,18500.);
  gPad->SetLogz(1);
  hist3->DrawClone("col");
}
