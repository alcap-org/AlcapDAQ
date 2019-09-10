#include "scripts/XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_XRaySpectra_TimeCut() {

  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");
  
  const int n_spectra = 2;
  std::string settings[n_spectra] = {"5000nsTimeCut", "200nsTimeCut"};
  std::string leglabels[n_spectra] = {"|t| < 5000 ns", "|t| < 200 ns"};
  Int_t colours[n_spectra] = {kBlue, kRed};

  
  TLegend* leg = new TLegend(0.40,0.25,0.80,0.7);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  for (int i_spectra = 0; i_spectra < n_spectra; ++i_spectra) {
    std::string i_setting = settings[i_spectra];
    std::string ge_channel = "GeLoGain";
    std::string full_spectrum_name = ge_channel + "/hEnergyTime";
    std::string norm_file_name = "/home/edmonds/data/results/SiL3/normalisation_geq0TgtPulse_newPP.root";
    std::string norm_ws_name = "ws";

    TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
    std::string dirname = "XRaySpectrum_GeLoGain_2p1s_" + i_setting;
    std::string treename = dirname + "/counttree";
    TTree* counttree = (TTree*) norm_file->Get(treename.c_str());

    std::string histname = dirname + "/hGe_Spectrum";
    TH1F* hXRaySpectrum = (TH1F*) norm_file->Get(histname.c_str());
    //    RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());

    hXRaySpectrum->Rebin(4);
    //  hXRaySpectrum->SetLineColor(kBlue);
    std::string histtitle = "SiL3 Dataset, X-Ray Spectrum (" + ge_channel + ")";
    hXRaySpectrum->SetTitle(histtitle.c_str());
    hXRaySpectrum->SetStats(false);
    hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);
    hXRaySpectrum->GetXaxis()->SetRangeUser(250, 1200);
    hXRaySpectrum->SetMaximum(3e4);
    hXRaySpectrum->SetLineColor(colours[i_spectra]);

    std::stringstream axislabel;
    axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
    hXRaySpectrum->SetYTitle(axislabel.str().c_str());
    hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);

    double n_stopped_muons = 0;
    double n_stopped_muons_error = 0;
    counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
    counttree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
    counttree->GetEntry(0);
    
    
    std::stringstream leglabel;
    leglabel.str("");
    //    leglabel << "#splitline{" << leglabels[i_spectra] << "}{N_{stop-#mu} = (" << std::setprecision(4) << n_stopped_muons/1e6 << " #pm " << std::setprecision(2) << n_stopped_muons_error/1e6 << ") #times 10^{6}}";
    leglabel << leglabels[i_spectra];
    leg->AddEntry(hXRaySpectrum, leglabel.str().c_str(), "l");

    if (i_spectra == 0) {
      hXRaySpectrum->Draw("HIST E");
    }
    else {
      hXRaySpectrum->Draw("HIST E SAME");
    }
  }
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.50, 0.75, "AlCap Preliminary");

  leg->Draw();
  /*
  XRay xray;
  xray.material = "Si";
  int min_transition = 2;
  int max_transition = 3;
  std::stringstream transition;
  for (int i_transition = min_transition; i_transition <= max_transition; ++i_transition) {
    transition.str("");
    transition << i_transition << "p-1s";
    xray.transition = transition.str();
    if (FillXRayInfo(xray, ge_channel) == 1) { // if we return an error code
      std::cout << "Error: Problem getting X-ray information" << std::endl;
      return;
    }

    double arrow_length = 250;
    int i_transition_bin = hXRaySpectrum->FindBin(xray.energy);
    double arrow_y_end = hXRaySpectrum->GetBinContent(i_transition_bin)+10;
    double arrow_y_start = arrow_y_end+arrow_length;
    TArrow* arrow = new TArrow(xray.energy, arrow_y_start, xray.energy, arrow_y_end, 0.01);
    arrow->SetLineColor(kRed);
    arrow->SetLineWidth(2);
    arrow->Draw();

    std::stringstream text;
    text.str("");
    text << "#splitline{     " << transition.str() << "}{(" << xray.energy << " keV)}";
    latex->SetTextAlign(21);
    latex->SetTextSize(0.03);
    latex->DrawLatex(xray.energy, arrow_y_start+50, text.str().c_str());
  }
  */

  /*  TPad* inset_pad = new TPad("inset", "inset", 0.47, 0.15, 0.92, 0.59);
  inset_pad->SetFillStyle(0);
  inset_pad->Draw();
  inset_pad->cd();

  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle("Si 2p-1s X-Ray");
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  //  Eframe->SetMinimum(3000);
  (ws->data("data"))->plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);

  Eframe->Draw();
  */
}
