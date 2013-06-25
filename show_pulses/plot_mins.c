{
  char title[80];
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TCanvas *c1 = new TCanvas();
  c1->Divide(4,2);

  for(int i = 1; i <= 8; i++) {
    c1->cd(i);
    TF1 *gaus = (TF1*)gROOT->GetFunction("gaus");
    gaus->SetLineColor(kRed);
    gaus->SetLineWidth(1);
    sprintf(title, "py%d", i);

    TH1 *py = minSample->ProjectionY(title,i,i);
    sprintf(title, "Channel %d", i-1);
    py->SetTitle(title);
    //    py->GetXaxis()->SetRange(regions[i-1] - 2000, regions[i-1] + 2000);
    //    py->Draw("E1");
    // py->Fit("gaus","LR","SAME",regions[i-1] - 2000, regions[i-1] + 2000);
    Int_t x_min = 1000 ;
    Int_t x_max = 2900 ;   
    py->Fit("gaus","","",x_min, x_max);
    py->Draw("E1");
  }
  
}
