#include "../../../XRayAnalysis/XRayUtils.h"

void SiL3_FinalPlot_XRaySpectra_TimeCut(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  
  const int n_settings = 2;
  int time_cuts[n_settings] = {5000, 200};
  //  std::string settings[n_settings] = {"5000nsTimeCut", "200nsTimeCut"};
  //  std::string leglabels[n_settings] = {"|t| < 5000 ns", "|t| < 200 ns"};
  Int_t colours[n_settings] = {kBlue, kRed};

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};
  
  numbers_file << "% From Si16b_FinalPlot_XRaySpectrum_TimeCuts.C" << std::endl;
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
    std::string ge_channel = ge_channels[i_ge_channel];
    std::string canvasname = "c_XRaySpectrum_" + ge_channel;

    TLegend* leg = new TLegend(0.40,0.25,0.80,0.7);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);
    leg->SetFillColor(kWhite);


    TCanvas* c_XRaySpectrum = new TCanvas(canvasname.c_str(), canvasname.c_str());
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string time_cut = std::to_string(time_cuts[i_setting]) + "ns";
      //      std::string i_setting = settings[i_settings];
      std::string full_spectrum_name = ge_channel + "/hEnergyTime";
      std::string norm_file_name = "/home/edmonds/data/results/SiL3/normalisation_geq0TgtPulse_newPP.root";
      std::string norm_ws_name = "ws";

      TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
      std::string dirname = "XRaySpectrum_" + ge_channel + "_2p1s_" + time_cut + "TimeCut";
      std::string treename = dirname + "/counttree";
      TTree* counttree = (TTree*) norm_file->Get(treename.c_str());

      std::string histname = dirname + "/hGe_Spectrum";
      TH1F* hXRaySpectrum = (TH1F*) norm_file->Get(histname.c_str());
      if (!hXRaySpectrum) {
	std::cout << "Could not get histogram " << histname << std::endl;
      }
	
      //    RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());

      hXRaySpectrum->Rebin(4);
      //  hXRaySpectrum->SetLineColor(kBlue);
      std::string histtitle = "SiL3 Dataset, X-Ray Spectrum (" + ge_channel + ")";
      hXRaySpectrum->SetTitle(histtitle.c_str());
      hXRaySpectrum->SetStats(false);
      //    hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);
      //    hXRaySpectrum->GetXaxis()->SetRangeUser(250, 1200);
      //    hXRaySpectrum->SetMaximum(3e4);
      hXRaySpectrum->SetLineColor(colours[i_setting]);

      std::stringstream axislabel;
      axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
      hXRaySpectrum->SetYTitle(axislabel.str().c_str());
      hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);

      double n_stopped_muons = 0;
      double n_stopped_muons_error = 0;
      counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
      counttree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
      counttree->GetEntry(0);
    

      std::string leglabel = "|t| < " + std::to_string(time_cuts[i_setting]) + " ns";
      leg->AddEntry(hXRaySpectrum, leglabel.c_str(), "l");
      
      //      std::stringstream leglabel;
      //      leglabel.str("");
      //    leglabel << "#splitline{" << leglabels[i_setting] << "}{N_{stop-#mu} = (" << std::setprecision(4) << n_stopped_muons/1e6 << " #pm " << std::setprecision(2) << n_stopped_muons_error/1e6 << ") #times 10^{6}}";
      //      leglabel << leglabels[i_setting];
      //      leg->AddEntry(hXRaySpectrum, leglabel.str().c_str(), "l");

      alcaphistogram(hXRaySpectrum);
      if (i_setting == 0) {
	hXRaySpectrum->Draw();
	hXRaySpectrum->SetDrawOption("HIST E1");
	alcapPreliminary(hXRaySpectrum);
      }
      else {
	hXRaySpectrum->Draw("SAME");
	hXRaySpectrum->SetDrawOption("HIST E1 SAME");
      }

      if (i_setting == 1) {
	if (ge_channel == "GeHiGain") { // only want to write this out once
	  numbers_file << "\\newcommand\\SiLNXRaysTimeCut{$|t|~\\SI{<" << time_cuts[i_setting] << "}{\\nano\\second}$}" << std::endl;
	}
      
	std::string treename = dirname + "/counttree";
	TTree* counttree = (TTree*) norm_file->Get(treename.c_str());
	double n_xrays = 0;
	double n_xrays_error = 0;
	counttree->SetBranchAddress("xray_count", &n_xrays);
	counttree->SetBranchAddress("xray_count_error", &n_xrays_error);
	counttree->GetEntry(0);
      
	numbers_file << "\\newcommand\\SiL" << ge_channel << "NXRaysTimeCut";
	numbers_file << std::fixed << std::setprecision(0) << "{$" << n_xrays << " \\pm " << n_xrays_error << "$}" << std::endl;
      }
    }

    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.17, 0.72, ge_channel.c_str());
  
    //  TLatex* latex = new TLatex();
    //  latex->DrawLatexNDC(0.50, 0.75, "AlCap Preliminary");
    leg->Draw();
    
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_SiL3Dataset_XRaySpectrum_" + ge_channel + "_TimeCuts";
    
      std::string pdfname = savename + ".pdf";
      c_XRaySpectrum->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c_XRaySpectrum->SaveAs(pngname.c_str());
    }
  }
  numbers_file << std::endl;
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
