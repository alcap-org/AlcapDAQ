{
  double Z[17] = {      1.,     2.,     3.,    4.,     5.,     6.,    7.,      8.,    9.,   13.,   14.,    20.,    40.,     82.,     83.,     90.,     92.};
  double Q[17] = {      1.,     1.,     1.,    1.,     1.,     1.,    1.,   0.998, 0.998, 0.993, 0.992,  0.985,  0.940,   0.844,   0.840,   0.824,   0.820};
  double G[17] = { 0.450e3, 2.15e3, 4.68e3, 6.1e3, 27.5e3, 37.9e3,  66e3, 102.5e3, 229e3, 705e3, 868e3, 2546e3, 8630e3, 12985e3, 13240e3, 12560e3, 12610e3};
  double L[17] = {  2194.9, 2186.7, 2175.3, 2168.,  2072.,  2028., 1919.,   1796., 1463.,  864.,  758.,   334.,  110.4,    74.8,    73.4,    77.3,    77.0};

  TGraph* gq = new TGraph(17, Z, Q);
  TGraph* gg = new TGraph(17, Z, G);
  TGraph* gl = new TGraph(17, Z, L);

  gq->SetMarkerStyle(8);
  gg->SetMarkerStyle(8);
  gl->SetMarkerStyle(8);
  gq->SetMarkerColor(kBlack);
  gg->SetMarkerColor(kBlack);
  gl->SetMarkerColor(kBlack);

  TMultiGraph* mg = new TMultiGraph("mg", "mg");

  TCanvas* cl = new TCanvas("cl", "cl");
  gl->SetTitle("Muonic Atom Lifetime;Atomic Number;Lifetime (ns)");
  gl->SetMinimum(0);
  gl->Draw("AP");
  cl->SaveAs("img/mu_lifetimes.png");
}
