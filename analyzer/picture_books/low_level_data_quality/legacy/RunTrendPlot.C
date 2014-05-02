void RunTrendPlot() {

  gROOT->ProcessLine(".L TrendPlot.C+");
  TrendPlot("../../../../data", 2600, 3);
}
