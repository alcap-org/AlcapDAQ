
void SiL3_FlatBkgFitsCheck() {

  std::string infilename = "~/data/results/SiL3/fits.root";
  std::string wsname = "FlatBkgFit/ws";

  TFile* infile = new TFile(infilename.c_str(), "READ");
  RooWorkspace* ws = (RooWorkspace*) infile->Get(wsname.c_str());

  RooPlot* Eframe = (ws->var("edep"))->frame();
  std::string histtitle = "SiL3 Dataset, Flat Bkg Fit";
  Eframe->SetTitle(histtitle.c_str());
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("full_pdf"))->plotOn(Eframe);

  std::stringstream axislabel;
  axislabel << "Counts / " << Eframe->getHist("h_data")->getNominalBinWidth() << " keV";
  Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());

  Eframe->Draw();
  ws->Print();

  double landau_mean = ws->var("landau_mean")->getValV();
  double landau_sigma = ws->var("landau_sigma")->getValV();
  double resolution_mean = ws->var("resolution_mean")->getValV();
  double resolution_sigma = ws->var("resolution_sigma")->getValV();


  std::cout << landau_mean << "\t" << landau_sigma << std::endl;
}
