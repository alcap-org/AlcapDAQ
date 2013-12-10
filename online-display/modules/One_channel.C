void One_channel(std::string ch_name)
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,1);
  AlCapCanvas->cd(1);

  //  gROOT->ProcessLine(".L common/get_histogram.C"); // get_histogram() is called here
  /*****************************************************************/
  const int n_channels = 2;
  //std::string bank_names[n_channels] = {"Na", "Nb", "Nc", "Nd", "Ne", "Nf", "Ng", "Nh"};
  std::string hist_type[]={"Shapes","Heights"};
  std::string name;

  for (int counter = 0; counter < n_channels; counter++) {
    TH1* hist = get_histogram(ch_name, hist_type[counter]);
    if (hist) {
     AlCapCanvas->cd(counter+1);
      if (hist_type[counter] == "Shapes"){
	hist->Draw("COLZ");
        gPad->SetLogz();
      }
      else{
	//hist->GetYaxis()->SetRangeUser(0.,4095.);
	hist->Draw();
        gPad->SetLogy();
      }
    }
  }
}
