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

  double cut_min=-1000;
  double cut_max=1000;

  // Draw the timing plot
  TDiff_pad->cd();
  TH1* tdiff = get_histogram(name,Timediff ,folder);
  tdiff->Draw();
  TDiff_pad->SetLogy();

  // Draw the amplitude vs timing diff
  AmpVT_pad->cd();
  AmpVT_pad->SetLogz();
  TH2* AvsTdiff = get_histogram_2d(name, AmplitudeVsTdiff,folder);
  AvsTdiff->Draw("COLZ");
  double y_min=AvsTdiff->GetYaxis()->GetBinLowEdge(AvsTdiff->GetYaxis()->GetFirst());
  double y_max=AvsTdiff->GetYaxis()->GetBinUpEdge(AvsTdiff->GetYaxis()->GetLast());
  TLine* line=new TLine();
  line->SetLineColor(kRed);
  line->DrawLine(cut_min,y_min,cut_min,y_max);
  line->DrawLine(cut_max,y_min,cut_max,y_max);

  // Draw the cut diagram from the projection of AmplitudeVsTdiff
  Project_pad->cd();
  Project_pad->SetLogy();
  TH1D* full_projection = AvsTdiff->ProjectionY("_py_full");
  full_projection->SetTitle("Projection");
  TH1D* coincidence_projection = AvsTdiff->ProjectionY("_py_coinc", AvsTdiff->GetXaxis()->FindBin(cut_min), AvsTdiff->GetXaxis()->FindBin(cut_max));
  coincidence_projection->SetLineColor(kRed);
  
  coincidence_projection->Rebin(5);
  full_projection->Rebin(5);
  
  full_projection->Draw();
  coincidence_projection->Draw("SAME");
}
