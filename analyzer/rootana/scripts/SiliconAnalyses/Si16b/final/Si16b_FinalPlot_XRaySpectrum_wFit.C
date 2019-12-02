#include "../../../XRayAnalysis/XRayUtils.h"

void Si16b_FinalPlot_XRaySpectrum_wFit() {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  XRay xray;
  std::stringstream text;
  int transition_start = 2;
  double fit_bin_width = 0.5;

  std::stringstream start_str;
  start_str.str("");
  start_str << transition_start << "p";
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
    std::string norm_file_name = "/home/edmonds/data/results/Si16b/normalisation_newPP_geq1TgtPulse.root";
    std::string ge_channel = ge_channels[i_ge_channel];
    std::string dirname = "XRaySpectrum_" + ge_channel + "_" + start_str.str() + "1s_5000nsTimeCut";
    std::string norm_ws_name = dirname + "/ws";
    std::string full_spectrum_name = dirname + "/hGe_Spectrum";
    
    TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
    RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
    if (!ws) {
      std::cout << "Error: Cannot find RooWorkspace " << norm_ws_name << std::endl;
      return;
    }
    TH1D* hXRaySpectrum = (TH1D*) norm_file->Get(full_spectrum_name.c_str());
    if (!hXRaySpectrum) {
      std::cout << "Error: Cannot find XRaySpectrum " << full_spectrum_name << std::endl;
      return;
    }

    std::string treename = dirname + "/counttree";
    TTree* counttree = (TTree*) norm_file->Get(treename.c_str());
    if (!counttree) {
      std::cout << "Error: Cannot find TTree " << treename << std::endl;
      return;
    }

    std::string canvasname = "c_XRaySpectrum_" + ge_channel;
    TCanvas* c_XRaySpectrum = new TCanvas(canvasname.c_str(), canvasname.c_str());
    hXRaySpectrum->Rebin(4);
    std::string histtitle = "Si16b Dataset, Full X-Ray Spectrum (" + ge_channel + ")";
    hXRaySpectrum->SetTitle(histtitle.c_str());
    hXRaySpectrum->SetStats(false);
    hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);
  
    std::stringstream axislabel;
    axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
    hXRaySpectrum->SetYTitle(axislabel.str().c_str());
    hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
    hXRaySpectrum->Draw("HIST E");

    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.55, 0.30, "AlCap Preliminary");

    xray.material = "Si";
    int min_transition = transition_start;
    int max_transition = transition_start;
    std::stringstream transition_str;
    for (int i_transition = min_transition; i_transition <= max_transition; ++i_transition) {
      transition_str.str("");
      transition_str << i_transition << "p-1s";
      xray.transition = transition_str.str();
      if (FillXRayInfo(xray, ge_channel) == 1) { // if we return an error code
	std::cout << "Error: Problem getting X-ray information" << std::endl;
	return;
      }

      double arrow_length = 200;
      int i_transition_bin = hXRaySpectrum->FindBin(xray.energy);
      double arrow_y_end = hXRaySpectrum->GetBinContent(i_transition_bin)*1.1;
      double arrow_y_start = arrow_y_end+arrow_length;
      TArrow* arrow = new TArrow(xray.energy, arrow_y_start, xray.energy, arrow_y_end, 0.01);
      arrow->SetLineColor(kRed);
      arrow->SetLineWidth(2);
      arrow->Draw();

      text.str("");
      text << "#splitline{     " << transition_str.str() << "}{(" << xray.energy << " keV)}";
      latex->SetTextAlign(21);
      latex->SetTextSize(0.03);
      latex->DrawLatex(xray.energy, arrow_y_start+100, text.str().c_str());
    }

    TPad* inset_pad = new TPad("inset", "inset", 0.47, 0.45, 0.92, 0.90);
    inset_pad->SetFillStyle(0);
    inset_pad->Draw();
    inset_pad->cd();

    RooPlot* Eframe = (ws->var("edep"))->frame();
    std::string title = "Si " + start_str.str() + "-1s X-Ray";
    Eframe->SetTitle(title.c_str());
    axislabel.str("");
    axislabel << "Count / " << fit_bin_width << " keV";
    Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());
    Eframe->GetXaxis()->SetTitle("Energy [keV]");
    (ws->data("data"))->plotOn(Eframe);
    (ws->pdf("sum"))->plotOn(Eframe);
    Eframe->Draw();

    double n_stopped_muons = 0;
    double n_stopped_muons_error = 0;
    counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
    counttree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
    counttree->GetEntry(0);
    TLatex* count = new TLatex();
    //    count->SetTextAlign(22);
    text.str("");
    text << "#splitline{Number of #mu-stop}{ (" << std::fixed << std::setprecision(1) << n_stopped_muons/1e6 << " #pm " << std::setprecision(1) << n_stopped_muons_error/1e6 << ") #times 10^{6}}";
    std::cout << text.str() << std::endl;
    count->DrawLatexNDC(0.15, 0.7, text.str().c_str());

    text.str("");
    text << "#chi^2 / ndf = " << Eframe->chiSquare();
    std::cout << text.str() << std::endl;
    count->DrawLatexNDC(0.60, 0.7, text.str().c_str());

    TCanvas* c2 = new TCanvas();
    RooHist* pull = Eframe->pullHist();
    RooPlot* pull_frame = (ws->var("edep"))->frame();
    pull_frame->addPlotable(pull, "P");
    pull_frame->Draw(); 
    std::cout << Eframe->chiSquare() << std::endl;
  }
}
