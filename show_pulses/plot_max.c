{
char title[80];
gROOT->SetStyle("Plain");
gStyle->SetOptStat(0);
gStyle->SetOptFit(1111);
TCanvas *c1 = new TCanvas();

// double regions[] = {    };
int channels[] = { 0, 1, 2, 3, 4, 5, 6, 7};

c1->Divide(3,2);
for(int j = 0; j < 6; j++) {
  int i = channels[j];
  c1->cd(j+1);
  TF1 *gaus = (TF1*)gROOT->GetFunction("gaus");
  gaus->SetLineColor(kRed);
  gaus->SetLineWidth(1);
  sprintf(title, "py%d", i);
  TH1 *py = maxSample->ProjectionY(title,i+1,i+1);
  sprintf(title, "Channel %d", i);
  py->SetTitle(title);
//  py->GetXaxis()->SetRange(regions[i-1] - 2000, regions[i-1] + 2000);
  py->Draw("E1");
//  py->Fit("gaus","LR","SAME",regions[i-1] - 2000, regions[i-1] + 2000);
}

}
