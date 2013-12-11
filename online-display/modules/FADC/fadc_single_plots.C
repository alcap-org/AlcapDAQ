void fadc_single_plots(std::string fadc_id,std::string hist_type)
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

  for (int iChn = 0; iChn < n_channels; iChn++) {
    name=bank_names[iChn]+fadc_id;
    TH1* hist = get_histogram(name, hist_type);
    if (hist) {
      AlCapCanvas->cd(iChn+1);

      if (hist_type == "Shapes"){
	hist->Draw("COLZ");
	gPad->SetLogz(1);
      }
      else if(hist_type == "LatestPulse") {
	gPad->SetLogy(0);
	hist->GetXaxis()->SetRange(0,hist->FindLastBinAbove(1.,2));
      }
      else if(hist_type == "Times"){
	TLine *l = new TLine(1.12E8, 0, 1.12E8, hist->GetMaximum());
	l->SetLineColor(kRed);
	hist->GetXaxis()->SetRangeUser(0., 1.4E8);
	hist->Draw();
	l->Draw("SAME");
      }
      else{
	hist->GetYaxis()->SetRangeUser(0.,4095.);
	gPad->SetLogy(1);
	hist->Draw();
      }
    }
  }
}
