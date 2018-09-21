// Make all the pretty plots for this analysis
TCanvas* FinalPlot_XRayFit() {

  //////////////////
  // Open all files
  //  std::string dataset = "SiL3_active";
  std::string dataset = "Si16a_useful";
  std::string filename = "/home/edmonds/data/results/" + dataset + "/normalisation.root";
  
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Trouble opening " << filename << std::endl;
    return 1;
  }


  /////////////////////////
  // Create mustops canvas
  TCanvas* c_mustops = new TCanvas("c_mustops", "c_mustops");

  RooWorkspace* ws = (RooWorkspace*) file->Get("ws");
  //  RooWorkspace* ws = (RooWorkspace*) file->Get("ws_cross_check");
  ws->Print();

 
  // Draw the fit
  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle("Si 2p-1s X-Ray");
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);

  Eframe->Draw();

  return c_mustops;
}
