#include "scripts/XRayAnalysis/XRayUtils.h"

void MT_FinalPlot_XRayPeakFit() {

  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};
  Int_t colours[n_settings] = {kBlue, kRed};
  double text_yNDC[n_settings] = {0.7, 0.35};
  
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);

  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];

    std::string norm_file_name = "/home/edmonds/data/results/MT/normalisation_" + setting + ".root";
    std::string ge_channel = "GeLoGain";
    std::string transition = "2p-1s";
    std::string material = "Si";
    std::string norm_dir_name = "XRaySpectrum_" + ge_channel + "_" + material + "_noTimeCut";

    std::string norm_ws_name = norm_dir_name + "/ws";
    std::string norm_tree_name = norm_dir_name + "/counttree";

    TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
    RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());

    TTree* counttree = (TTree*) norm_file->Get(norm_tree_name.c_str());
    double xray_count = 0;
    double xray_count_error = 0;
    std::string* xray_material;
    std::string* xray_transition;
    double xray_energy = 0;
    double xray_intensity = 0;
    double xray_intensity_error = 0;
    counttree->SetBranchAddress("xray_count", &xray_count);
    counttree->SetBranchAddress("xray_count_error", &xray_count_error);
    counttree->SetBranchAddress("xray_material", &xray_material);
    counttree->SetBranchAddress("xray_transition", &xray_transition);
    counttree->SetBranchAddress("xray_energy", &xray_energy);
    counttree->SetBranchAddress("xray_intensity", &xray_intensity);
    counttree->SetBranchAddress("xray_intensity_error", &xray_intensity_error);
    counttree->GetEntry(0);
    
    //  hXRaySpectrum->SetLineColor(kBlue);
    //  std::string histtitle = "MT Dataset, Full X-Ray Spectrum (" + ge_channel + ")";

  
    RooPlot* Eframe = (ws->var("edep"))->frame();
    std::string histtitle = "MT Dataset, X-Ray Spectrum, " + ge_channel + " (with #pm 200 ns time cut)";
    Eframe->SetTitle(histtitle.c_str());
    Eframe->GetXaxis()->SetTitle("Energy [keV]");
    //  Eframe->SetMinimum(3000);
    (ws->data("data"))->plotOn(Eframe, RooFit::LineColor(colours[i_setting]), RooFit::MarkerColor(colours[i_setting]));
    (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineColor(colours[i_setting]));
    
    std::stringstream axislabel;
    axislabel << "Counts / " << Eframe->getHist("h_data")->getNominalBinWidth() << " keV";
    Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());
    
    Eframe->Draw("SAME");

    std::stringstream text;
    text.str("");
    text << "N_{X-ray} = " << std::fixed << std::setprecision(0) << xray_count << " #pm " << xray_count_error;
    latex->SetTextColor(colours[i_setting]);
    if (ge_channel == "GeLoGain") {
      latex->DrawLatexNDC(0.72, text_yNDC[i_setting], text.str().c_str());
    }
    else if (ge_channel == "GeHiGain") {
      latex->DrawLatexNDC(0.72, text_yNDC[i_setting], text.str().c_str());
    }
  }

  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.30, 0.80, "AlCap Preliminary");


  text.str("");
  text << "I_{" << *xray_material << ", " << *xray_transition << "} = " << std::fixed << std::setprecision(3) << xray_intensity << " #pm " << xray_intensity_error;
  if (ge_channel == "GeLoGain") {
    latex->DrawLatexNDC(0.65, 0.85, text.str().c_str());
  }
  else if (ge_channel == "GeHiGain") {
    latex->DrawLatexNDC(0.65, 0.85, text.str().c_str());
  }


  text.str("");
  text << "E_{" << *xray_material << ", " << *xray_transition << "} = " << std::fixed;
  if (transition == "2p-1s") {
    text << std::setprecision(3);
  }
  else {
    text << std::setprecision(1);
  }
  text << xray_energy << " keV";
  if (ge_channel == "GeLoGain") {
    latex->DrawLatexNDC(0.65, 0.78, text.str().c_str());
  }
  else if (ge_channel == "GeHiGain") {
    latex->DrawLatexNDC(0.65, 0.78, text.str().c_str());
  }
}
