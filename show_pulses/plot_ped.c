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
  TH1 *py = samples->ProjectionY(title,i,i);
  sprintf(title, "Channel %d", i-1);
  py->SetTitle(title);
  py->GetXaxis()->SetRange(py->GetMean()-9*py->GetRMS(), py->GetMean()+9*py->GetRMS());
  py->Draw("E1");
  py->Fit("gaus","LR","SAME",py->GetMean()-9*py->GetRMS(), py->GetMean()+9*py->GetRMS());
}

}
