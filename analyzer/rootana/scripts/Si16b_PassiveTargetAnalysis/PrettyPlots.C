// Make all the pretty plots for this analysis
void PrettyPlots() {

  //////////////////
  // Open all files
  std::string dataset = "Si16b_passive";
  std::string mustops_gelo_filename = "/home/edmonds/data/results/" + dataset + "/CountStoppedMuons_GeLoGain_fromTMEs_NewCuts.root";
  std::string outfilename = "/home/edmonds/data/results/" + dataset + "/pretty-plots.root";
  
  TFile* mustops_gelo_file = new TFile(mustops_gelo_filename.c_str(), "READ");
  if (mustops_gelo_file->IsZombie()) {
    std::cout << "Trouble opening mustops_gelo " << mustops_gelo_filename << std::endl;
    return 1;
  }


  /////////////////////////
  // Create mustops canvas
  TCanvas* c_mustops = new TCanvas("c_mustops", "c_mustops");

  TTree* mustops_tree = (TTree*) mustops_gelo_file->Get("mustops");
  double xray_energy = 0;
  std::string* xray_transition = 0;
  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;

  mustops_tree->SetBranchAddress("xray_energy", &xray_energy);
  mustops_tree->SetBranchAddress("xray_transition", &xray_transition);
  mustops_tree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  mustops_tree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
  
  mustops_tree->GetEntry(0);
  
  TH1F* hGe_Spectrum = (TH1F*) mustops_gelo_file->Get("hGe_Spectrum");
  RooWorkspace* ws = (RooWorkspace*) mustops_gelo_file->Get("ws");
  ws->Print();

  c_mustops->cd();
  hGe_Spectrum->Draw("HIST E");
  
  int bin_xray = hGe_Spectrum->FindBin(xray_energy);
  double arrow_y_end = hGe_Spectrum->GetBinContent(bin_xray)*1.1;
  double arrow_y_start = arrow_y_end*1.5;
  TArrow* arrow = new TArrow(xray_energy, arrow_y_start, xray_energy, arrow_y_end, 0.01);
  arrow->SetLineColor(kRed);
  arrow->SetLineWidth(2);
  arrow->Draw("");

  std::stringstream text;
  text.str("");
  text << "(" << xray_energy << " keV)";
  TLatex* energy_label = new TLatex(xray_energy, arrow_y_start*1.1, text.str().c_str());
  energy_label->SetTextAlign(21);
  energy_label->SetTextSize(0.034);
  energy_label->Draw("");

  
  TLatex* label = new TLatex(xray_energy, arrow_y_start*1.35, (*xray_transition).c_str());
  label->SetTextAlign(21);
  label->SetTextSize(0.04);
  label->Draw("");

  TLatex* preliminary_text = new TLatex(300, 25000, "AlCap Preliminary");
  preliminary_text->SetTextAlign(22);
  preliminary_text->SetTextSize(0.05);
  preliminary_text->Draw("");
  
  // Draw the fit
  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle("Si 2p-1s X-Ray");
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe, RooFit::Components(ws->argSet("xraypeak_pdf")), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  (ws->pdf("sum"))->plotOn(Eframe, RooFit::Components(ws->argSet("pol1_bkg")), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

  
  TPad* inset_pad = new TPad("inset", "inset", 0.45, 0.40, 0.89, 0.84);
  inset_pad->Draw();
  inset_pad->cd();
  Eframe->Draw();

  text.str("");
  text << "N_{stop #mu} = (" << std::fixed << std::setprecision(1) << n_stopped_muons/1e6 << " #pm " << std::setprecision(1) << n_stopped_muons_error/1e6 << ") #times 10^{6}";
  TLatex* mustop_text = new TLatex(400, 1000, text.str().c_str());
  mustop_text->SetTextAlign(22);
  mustop_text->SetTextSize(0.1);
  mustop_text->Draw("");


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  c_mustops->Write();
  outfile->Write();
  outfile->Close();
}
