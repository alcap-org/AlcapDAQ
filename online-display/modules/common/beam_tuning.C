void beam_tuning()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  //  gROOT->ProcessLine(".L common/get_histogram.C"); // get_histogram() is called here
  /*****************************************************************/
  const int n_channels = 8;
  std::string bank_names[n_channels] = {"Na", "Nb", "Nc", "Nd", "Ne", "Nf", "Ng", "Nh"};
  std::string name;

  TH1* hist = (TH2*)getObject("hSiL1PulseShapes");
  AlCapCanvas->cd(1);
  if (hist)
    hist->Draw("COLZ");

  hist = (TH2*)getObject("hNa80_Shapes");
  AlCapCanvas->cd(2);
  if (hist)
    hist->Draw("COLZ");

  hist = (TH2*)getObject("hCbBU_Shapes"); // want muSC
  AlCapCanvas->cd(3);
  if (hist)
    hist->Draw("COLZ");

  hist = (TH2*)getObject("hCcBU_Shapes"); // want muSCA
  AlCapCanvas->cd(4);
  if (hist)
    hist->Draw("COLZ");

  hist = (TH1*)getObject("hSiL1PulseHeights");
  AlCapCanvas->cd(5);
  if (hist)
    hist->Draw();

  hist = (TH1*)getObject("hNa80_Heights");
  AlCapCanvas->cd(6);
  if (hist)
    hist->Draw();

  hist = (TH1*)getObject("hCbBU_Heights");
  AlCapCanvas->cd(7);
  if (hist)
    hist->Draw();

  hist = (TH1*)getObject("hCcBU_Heights");
  AlCapCanvas->cd(8);
  if (hist)
    hist->Draw();
}
