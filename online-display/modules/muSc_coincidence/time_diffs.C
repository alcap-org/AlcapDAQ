void time_diffs(std::string channel,std::string suffix)
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,1);
  TPad *AmpVT_pad = (TPad*) AlCapCanvas->GetListOfPrimitives()->FindObject("Canvas_2");
  TPad *left_pad  = (TPad*) AlCapCanvas->GetListOfPrimitives()->FindObject("Canvas_1");
  left_pad->Divide(1,2);
  TPad *TDiff_pad  = (TPad*) left_pad->GetListOfPrimitives()->FindObject("Canvas_1_1");
  TPad *Project_pad  = (TPad*) left_pad->GetListOfPrimitives()->FindObject("Canvas_1_2");

  //  gROOT->ProcessLine(".L common/get_histogram.C"); // get_histogram() is called here
  /*****************************************************************/
  std::string name ="MuSC_"+channel;
  std::string Timediff="Timediff"+suffix;
  std::string AmplitudeVsTdiff="AmplitudeVsTdiff"+suffix;
  std::string folder="MuSC_TimingCorrelations";

  double cut_min=-100;
  double cut_max=1000;

  // Draw the timing plot
  TDiff_pad->cd();
  TH1* tdiff = get_histogram(name,Timediff ,folder);
  tdiff->Draw();

  // Draw the amplitude vs timing diff
  AmpVT_pad->cd();
  TH2* AvsTdiff = get_histogram_2d(name, AmplitudeVsTdiff,folder);
  AvsTdiff->Draw("scat");

  // Draw the cut diagram from the projection of AmplitudeVsTdiff
  Project_pad->cd();
  TH1D* full_projection = AvsTdiff->ProjectionY("_py_full");
  TH1D* coincidence_projection = AvsTdiff->ProjectionY("_py_coinc", AvsTdiff->GetXaxis()->FindBin(-1500), AvsTdiff->GetXaxis()->FindBin(-500));
  coincidence_projection->SetLineColor(kRed);
  
  coincidence_projection->Rebin(5);
  full_projection->Rebin(5);
  
  full_projection->Draw();
  coincidence_projection->Draw("SAME");
}
