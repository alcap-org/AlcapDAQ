{
  TF1* f = new TF1("f", "(x<=105.658/2)*(3*(x/(105.658/2))^2-2*(x/(105.658/2))^3)", 0, 106);
  f->SetTitle("Free Muon Decay;Energy [MeV];#frac{d#Gamma}{dx}");
  f->SetLineColor(kBlack);
  f->SetNpx(10000);

  TLatex* lt = new TLatex(55, 0.2, "#frac{m_{#mu}}{2}");

  TArrow* arr = new TArrow(57., 0.15, 105.658/2., 0.01, 0.03);
  arr->SetLineWidth(3);
  arr->SetLineColor(kRed);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  f  ->Draw();
  lt ->Draw();
  arr->Draw();
  c->SaveAs("img/mudecayfree.png");
}
