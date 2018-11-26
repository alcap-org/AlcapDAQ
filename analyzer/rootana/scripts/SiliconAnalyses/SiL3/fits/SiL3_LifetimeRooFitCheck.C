
void SiL3_LifetimeRooFitCheck() {

  std::string infilename = "~/data/results/SiL3/fits.root";
  std::string wsname = "LifetimeRooFit/ws";

  TFile* infile = new TFile(infilename.c_str(), "READ");
  RooWorkspace* ws = (RooWorkspace*) infile->Get(wsname.c_str());

  RooPlot* Eframe = (ws->var("time"))->frame();
  std::string histtitle = "SiL3 Dataset";
  Eframe->SetTitle(histtitle.c_str());
  Eframe->GetXaxis()->SetTitle("Time [ns]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  //  (ws->pdf("full_pdf"))->plotOn(Eframe);
  //  (ws->pdf("full_pdf"))->plotOn(Eframe, RooFit::Components("beam_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));
  //  (ws->pdf("full_pdf"))->plotOn(Eframe, RooFit::Components("lifetime_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  //  (ws->pdf("full_pdf"))->plotOn(Eframe, RooFit::Components("flat_bkg_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kBlack));
  //  (ws->pdf("beam_pdf"))->plotOn(Eframe);
  //  (ws->pdf("lifetime_pdf"))->plotOn(Eframe);
  //  (ws->pdf("flat_bkg_pdf"))->plotOn(Eframe);
  (ws->pdf("full_bkg_pdf"))->plotOn(Eframe);

  std::stringstream axislabel;
  axislabel << "Counts / " << Eframe->getHist("h_data")->getNominalBinWidth() << " ns";
  Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());

  Eframe->Draw();
  ws->Print();

  /*
  double landau_mean = ws->var("landau_mean")->getValV();
  double landau_sigma = ws->var("landau_sigma")->getValV();
  double resolution_mean = ws->var("resolution_mean")->getValV();
  double resolution_sigma = ws->var("resolution_sigma")->getValV();
  std::cout << landau_mean << "\t" << landau_sigma << std::endl;
  */
  //  double lifetime = ws->var("lifetime")->getValV();
  //  std::cout << -1.0/lifetime << std::endl;
  //  std::cout << lifetime << std::endl;
}
