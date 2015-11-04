void sis330x_board_errors()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(3,2);

  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/
  const int n_boards = 6;
  std::string bank_names[n_boards] = {
    "SIS3300_B1", "SIS3300_B2","SIS3300_B3","SIS3300_B4", "SIS3300_B5","SIS3300_B6"};

  for (int iBrd = 0; iBrd < n_boards; iBrd++) {
    std::string histname = "h1_Errors_" + bank_names[iBrd];
    TH1* hist =   (TH1 *)getObject(histname.c_str());
    if (hist) {
      AlCapCanvas->cd(iBrd+1);
      hist->Draw("");
    }
  }
}
