#include "scripts/XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_2p1sFit_wTargetCoinc_SplitPeak() {

  std::string norm_file_name = "/home/edmonds/data/results/SiL3_active/normalisation.root";
  TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");

  std::string ge_channel = "GeLoGain";
  std::string transition = "3p-1s";
  const int n_slices = 2;
  std::string norm_dir_names[n_slices] = {"_wSiL3Coinc_MuonSlice3000_4000", "_wSiL3Coinc_MuonSlice4000_5000"};

  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");
  c_XRaySpectrum->Divide(2,1);

  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    c_XRaySpectrum->cd(i_slice+1);

    std::string norm_dir_name = ge_channel + norm_dir_names[i_slice] + "_" + transition;
    std::string norm_ws_name = norm_dir_name + "/ws";
    std::string norm_tree_name = norm_dir_name + "/counttree";

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

    RooPlot* Eframe = (ws->var("edep"))->frame();
    std::string histtitle = "SiL3 Dataset, X-Ray Spectrum, " + ge_channel + " (coincident with muon hit)";
    Eframe->SetTitle(histtitle.c_str());
    Eframe->GetXaxis()->SetTitle("Energy [keV]");
    //  Eframe->SetMinimum(3000);
    (ws->data("data"))->plotOn(Eframe);
    (ws->pdf("sum"))->plotOn(Eframe);
    
    std::stringstream axislabel;
    axislabel << "Counts / " << Eframe->getHist("h_data")->getNominalBinWidth() << " keV";
    Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());
    
    Eframe->Draw();

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.035);
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.30, 0.70, "AlCap Preliminary");
    
    std::stringstream text;
    text.str("");
    text << "N_{X-ray} = " << std::fixed << std::setprecision(0) << xray_count << " #pm " << xray_count_error;
    if (ge_channel == "GeLoGain") {
      latex->DrawLatexNDC(0.30, 0.55, text.str().c_str());
    }
    else if (ge_channel == "GeHiGain") {
      latex->DrawLatexNDC(0.72, 0.55, text.str().c_str());
    }
    
    text.str("");
    text << "I_{" << *xray_material << ", " << *xray_transition << "} = " << std::fixed << std::setprecision(3) << xray_intensity << " #pm " << xray_intensity_error;
    if (ge_channel == "GeLoGain") {
      latex->DrawLatexNDC(0.30, 0.49, text.str().c_str());
    }
    else if (ge_channel == "GeHiGain") {
      latex->DrawLatexNDC(0.72, 0.49, text.str().c_str());
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
      latex->DrawLatexNDC(0.30, 0.43, text.str().c_str());
    }
    else if (ge_channel == "GeHiGain") {
      latex->DrawLatexNDC(0.72, 0.43, text.str().c_str());
    }
  }
}
