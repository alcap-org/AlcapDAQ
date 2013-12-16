char*  MakeName(std::string name, std::string hist_type){
	std::cout<< ("h"+name+"_"+hist_type).c_str()<<std::endl;
        return ("h"+name+"_"+hist_type).c_str();
}

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
  std::string AmplitudeVsTdiff="AmplitudeVsTdiff";
  std::string FullProj_name=AmplitudeVsTdiff+ "_py_full"+suffix;
  std::string CutProj_name=AmplitudeVsTdiff+ "_py_cut"+suffix;
  AmplitudeVsTdiff+=suffix;
  //std::string folder="MuSC_TimingCorrelations";

  double cut_min=-1000;
  double cut_max=1000;

  // Create all histograms to avoid memory leaks and redefinition issues (thanks root...)
  TH1* tdiff=NULL;
  //gROOT->GetObject(MakeName(name,Timediff),tdiff);
  TH2* AvsTdiff=NULL;
  //gROOT->GetObject(MakeName(name,AmplitudeVsTdiff),AvsTdiff);
  TH1D* full_projection=NULL;
  gROOT->Delete(FullProj_name.c_str());//,full_projection);
  if(full_projection) delete full_projection;
  TH1D* coincidence_projection=NULL;
  gROOT->Delete(CutProj_name.c_str());//,coincidence_projection);
  if( coincidence_projection) delete coincidence_projection;

  // Draw the timing plot
  TDiff_pad->cd();
  tdiff = get_histogram(name,Timediff );//,folder);
  tdiff->Draw();
  TDiff_pad->SetLogy();

  // Draw the amplitude vs timing diff
  AmpVT_pad->cd();
  AmpVT_pad->SetLogz();
  AvsTdiff = get_histogram_2d(name, AmplitudeVsTdiff);//,folder);
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
  full_projection = AvsTdiff->ProjectionY(FullProj_name.c_str());
  full_projection->SetTitle("Projection (black: no cut. red: t_diff small)");
  TH1D* coincidence_projection = AvsTdiff->ProjectionY(CutProj_name.c_str(), AvsTdiff->GetXaxis()->FindBin(cut_min), AvsTdiff->GetXaxis()->FindBin(cut_max));
  coincidence_projection->SetLineColor(kRed);
  
  coincidence_projection->Rebin(5);
  full_projection->Rebin(5);
  
  full_projection->Draw();
  coincidence_projection->Draw("SAME");
}
