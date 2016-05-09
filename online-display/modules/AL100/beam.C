void beam()
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
  hist->Draw();
  hist->SetAxisRange(0,20000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);

  AlCapCanvas->cd(2);
  TH1 * hist = get_histogram("D701","Times");
  hist->DrawClone("");
  hist->Draw();
  hist->SetAxisRange(0,3000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);

  AlCapCanvas->cd(3);
  TH1 * hist = get_histogram("D700","Times");
  hist->DrawClone("");
  hist->Draw();
  hist->SetAxisRange(0,20000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);

  AlCapCanvas->cd(4);
  TH1 * hist = get_histogram("D701","Heights");
  hist->DrawClone("");
  hist->Draw();
  hist->SetAxisRange(0,3000,"X");
  gPad->SetLogy(1);
  gPad->SetLogx(0);

}
