void Al50_FinalPlot_RotatedAxes_fromMark() {
  
  std::string filename = "~m-wong/data/R15b/al50.root";
  std::string treename = "tree";

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get(treename.c_str());

  double min_rotated = 0;
  double max_rotated = 1.4;
  double rotated_width = 0.015;
  int n_rotated_bins = (max_rotated - min_rotated) / rotated_width;
  TH2F* hist = new TH2F("hist", "", n_rotated_bins,min_rotated,max_rotated, n_rotated_bins,min_rotated,max_rotated);
  
  tree->Draw("0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) : 0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) )>>hist", "t2>400 && t2<10e3 && abs(t2-t1)<200 && timeToPrevTME>10e3 && timeToNextTME>10e3 && channel.Contains(\"SiL\")", "goff");
  hist->Draw("COLZ");

  double min_slice = 0.3;//0.5;
  double max_slice = 0.4;//0.6;
  int min_slice_bin = hist->GetXaxis()->FindBin(min_slice);
  int max_slice_bin = hist->GetXaxis()->FindBin(max_slice)-1;

  TCanvas* c = new TCanvas();
  TH1D* hProj  = (TH1D*) hist->ProjectionY("", min_slice_bin, max_slice_bin);
  //  hProj->Rebin(2);
  hProj->Draw("HIST E");

  TF1* fQuadGaus = new TF1("fQuadGaus", "[0]*TMath::Gaus(x, [1], [2]) + [3]*TMath::Gaus(x, [4], [5]) + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 0.3, 1.4);
  fQuadGaus->SetParameter(0, 250);
  fQuadGaus->SetParameter(1,0.4);
  //  fQuadGaus->SetParLimits(1, 0.35, 0.45);
  fQuadGaus->SetParameter(2,0.1);
  fQuadGaus->SetParameter(3, 100);
  fQuadGaus->SetParameter(4,0.6);
  //  fQuadGaus->SetParLimits(4, 0.55, 0.65);
  fQuadGaus->SetParameter(5,0.1);
  fQuadGaus->SetParameter(6, 50);
  fQuadGaus->SetParameter(7,0.7);
  fQuadGaus->SetParLimits(7, 0.62, 0.75);
  fQuadGaus->SetParameter(8,0.1);
  fQuadGaus->SetParameter(9, 25);
  //  fQuadGaus->SetParLimits(9, 10, 100);
  //  fQuadGaus->SetParameter(10,1.3);
  fQuadGaus->SetParameter(10,1.25);
  //  fQuadGaus->SetParLimits(10, 1.15, 1.35);
  fQuadGaus->SetParameter(11,0.1);
  fQuadGaus->SetNpx(100000);
  hProj->Fit(fQuadGaus, "R");

  // fQuadGaus->FixParameter(0, fQuadGaus->GetParameter(0));
  // fQuadGaus->FixParameter(1, fQuadGaus->GetParameter(1));
  // fQuadGaus->FixParameter(2, fQuadGaus->GetParameter(2));
  // fQuadGaus->FixParameter(3, fQuadGaus->GetParameter(3));
  // fQuadGaus->FixParameter(4, fQuadGaus->GetParameter(4));
  // fQuadGaus->FixParameter(5, fQuadGaus->GetParameter(5));
  // fQuadGaus->FixParameter(6, fQuadGaus->GetParameter(6));
  // fQuadGaus->FixParameter(7, fQuadGaus->GetParameter(7));
  // fQuadGaus->FixParameter(8, fQuadGaus->GetParameter(8));
  // hProj->Fit(fQuadGaus, "R");
	      
  
  hProj->GetFunction("fQuadGaus")->Draw("LSAME");

  TLatex* latex = new TLatex();
  std::stringstream text;
  text.str("");
  text << std::fixed << std::setprecision(4) << "#splitline{" << fQuadGaus->GetParameter(1) << "}{ #pm " << fQuadGaus->GetParError(1) << "}";
  latex->SetTextSize(0.04);
  latex->SetTextAlign(22);
  latex->DrawLatex(fQuadGaus->GetParameter(1)+0.05, fQuadGaus->GetParameter(0)*0.8, text.str().c_str());

  text.str("");
  text << std::fixed << std::setprecision(4) << "#splitline{" << fQuadGaus->GetParameter(4) << "}{ #pm " << fQuadGaus->GetParError(4) << "}";
  latex->SetTextSize(0.04);
  latex->SetTextAlign(22);
  latex->DrawLatex(fQuadGaus->GetParameter(4), fQuadGaus->GetParameter(3)*1.4, text.str().c_str());

  text.str("");
  text << std::fixed << std::setprecision(4) << "#splitline{" << fQuadGaus->GetParameter(7) << "}{ #pm " << fQuadGaus->GetParError(7) << "}";
  latex->SetTextSize(0.04);
  latex->SetTextAlign(22);
  latex->DrawLatex(fQuadGaus->GetParameter(7), fQuadGaus->GetParameter(6)*1.4, text.str().c_str());

  text.str("");
  text << std::fixed << std::setprecision(5) << "#splitline{" << fQuadGaus->GetParameter(10) << "}{ #pm " << fQuadGaus->GetParError(10) << "}";
  latex->SetTextSize(0.04);
  latex->SetTextAlign(22);
  latex->DrawLatex(fQuadGaus->GetParameter(10), fQuadGaus->GetParameter(9)*1.4, text.str().c_str());
}
