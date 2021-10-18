#include "../../../XRayAnalysis/XRayUtils.h"

void Si16b_FinalPlot_XRaySpectrum_wFit(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  const int n_transitions = 2;
  int transition_starts[n_transitions] = {2, 3};
  double fit_bin_widths[n_transitions] = {0.5, 0.5};

  numbers_file << "% From Si16b_FinalPlot_XRaySpectrum_wFit.C" << std::endl;
  for (int i_transition = 0; i_transition < n_transitions; ++i_transition) {
    XRay xray;
    std::stringstream text;
    int transition_start = transition_starts[i_transition];
    double fit_bin_width = fit_bin_widths[i_transition];

    std::stringstream start_str;
    start_str.str("");
    start_str << transition_start << "p";
    for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
      std::string norm_file_name = "/home/edmonds/data/results/Si16b/normalisation_newPP_geq1TgtPulse_1.root";
      //      std::string norm_file_name = "/home/edmonds/data/results/Si16b/normalisation_newPP.root";//_geq0TgtPulse.root";
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
      TCanvas* c_XRaySpectrum = new TCanvas();//canvasname.c_str(), canvasname.c_str());
      hXRaySpectrum->Rebin(4);
      std::string histtitle = "Si16b Dataset, Full X-Ray Spectrum (" + ge_channel + ")";
      hXRaySpectrum->SetTitle(histtitle.c_str());
      hXRaySpectrum->SetStats(false);
      hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);
  
      std::stringstream axislabel;
      axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
      hXRaySpectrum->SetYTitle(axislabel.str().c_str());
      hXRaySpectrum->GetYaxis()->SetTitleOffset(0.9);
  
      hXRaySpectrum->Draw("HIST E");
      alcaphistogram(hXRaySpectrum);
      alcapPreliminary(hXRaySpectrum);
      hXRaySpectrum->SetLineWidth(1);
      hXRaySpectrum->SetDrawOption("HIST E");
      //      alcaplabel(ge_channel, hXRaySpectrum);
    
      TLatex* latex = new TLatex();
      latex->DrawLatexNDC(0.17, 0.79, ge_channel.c_str());

      xray.material = "Si";
      int min_transition = transition_start;
      int max_transition = transition_start;
      std::stringstream transition_str;
      for (int j_transition = min_transition; j_transition <= max_transition; ++j_transition) {
	transition_str.str("");
	transition_str << j_transition << "p-1s";
	xray.transition = transition_str.str();
	if (FillXRayInfo(xray, ge_channel) == 1) { // if we return an error code
	  std::cout << "Error: Problem getting X-ray information" << std::endl;
	  return;
	}

	double arrow_length = 200;
	int j_transition_bin = hXRaySpectrum->FindBin(xray.energy);
	double arrow_y_end = hXRaySpectrum->GetBinContent(j_transition_bin)*1.1;
	double arrow_y_start = arrow_y_end+arrow_length;
	TArrow* arrow = new TArrow(xray.energy, arrow_y_start, xray.energy, arrow_y_end, 0.01);
	arrow->SetLineColor(kRed);
	arrow->SetLineWidth(2);
	arrow->Draw();

	TLatex* latex = new TLatex();
	text.str("");
	text << "#splitline{     " << transition_str.str() << "}{(" << xray.energy << " keV)}";
	latex->SetTextAlign(21);
	latex->SetTextSize(0.03);
	latex->DrawLatex(xray.energy, arrow_y_start+100, text.str().c_str());
      }

      TPad* inset_pad = new TPad("inset", "inset", 0.40, 0.37, 0.87, 0.87);
      //    inset_pad->SetFillStyle(0);
      inset_pad->SetFillColor(kWhite);
      inset_pad->Draw();
      inset_pad->cd();

      RooPlot* Eframe = (ws->var("edep"))->frame();
      std::string title = "Si " + start_str.str() + "-1s X-Ray";
      Eframe->SetTitle(title.c_str());
      axislabel.str("");
      axislabel << "Count / " << fit_bin_width << " keV";
      Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());
      Eframe->GetXaxis()->SetTitle("Energy [keV]");
      Eframe->GetXaxis()->SetTitleOffset(0.9);
      Eframe->GetYaxis()->SetTitleOffset(0.9);
      (ws->data("data"))->plotOn(Eframe);
      (ws->pdf("sum"))->plotOn(Eframe);
      Eframe->Draw();

      double n_xrays = 0;
      double n_xrays_error = 0;
      double n_stopped_muons = 0;
      double n_stopped_muons_error = 0;
      counttree->SetBranchAddress("xray_count", &n_xrays);
      counttree->SetBranchAddress("xray_count_error", &n_xrays_error);
      counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
      counttree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
      counttree->GetEntry(0);

      double capture_fraction = 0.658;
      double n_captured_muons = n_stopped_muons * capture_fraction;
      double n_captured_muons_error = (n_stopped_muons_error / n_stopped_muons) * n_captured_muons;

      numbers_file << std::fixed;
      numbers_file << "\\newcommand\\Sib" << ge_channel << "NXRays";
      if (transition_start > 2) {
	numbers_file << "Alternate";
      }
      numbers_file << "{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_xrays << "\\pm" << n_xrays_error << "}}" << std::endl;
      
      numbers_file << "\\newcommand\\Sib" << ge_channel << "NStoppedMuons";
      if (transition_start > 2) {
	numbers_file << "Alternate";
      }
      numbers_file << "{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_stopped_muons << " \\pm " << n_stopped_muons_error << "}}" << std::endl;;
      
      numbers_file << "\\newcommand\\Sib" << ge_channel << "NCapturedMuons";
      if (transition_start > 2) {
	numbers_file << "Alternate";
      }
      numbers_file << "{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_captured_muons << " \\pm " << n_captured_muons_error << "}}" << std::endl;;

      // rounded figures for table
      numbers_file << "\\newcommand\\Sib" << ge_channel << "NXRaysTab";
      if (transition_start > 2) {
	numbers_file << "Alternate";
      }
      numbers_file << "{\\num[round-precision=3, round-mode=figures]{" << n_xrays/1e3 << "}(\\num[round-precision=1, round-mode=figures]{" << n_xrays_error/1e2 << "})}" << std::endl;
      
      numbers_file << "\\newcommand\\Sib" << ge_channel << "NStoppedMuonsTab";
      if (transition_start > 2) {
	numbers_file << "Alternate";
      }
      numbers_file << "{\\num[round-precision=3, round-mode=figures]{" << n_stopped_muons/1e6 << "}(\\num[round-precision=1,round-mode=figures]{" << n_stopped_muons_error/1e5 << "})}" << std::endl;;
      
      numbers_file << "\\newcommand\\Sib" << ge_channel << "NCapturedMuonsTab";
      if (transition_start > 2) {
	numbers_file << "Alternate";
      }
      numbers_file << "{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_captured_muons << " \\pm " << n_captured_muons_error << "}}" << std::endl;;

      // Write out numebrs without \pm
      // numbers_file << "\\newcommand\\Sib" << ge_channel << "NXRays";
      // if (transition_start > 2) {
      // 	numbers_file << "Alternate";
      // }
      // numbers_file << "NoPM{$" <<std::fixed << std::setprecision(1) << n_xrays/1e3 << "(" << std::setprecision(0) << (n_xrays_error/1e3)*10 << ") \\times 10^{3}$}" << std::endl;
      
      // numbers_file << "\\newcommand\\Sib" << ge_channel << "NStoppedMuons";
      // if (transition_start > 2) {
      //  	numbers_file << "Alternate";
      // }
      // numbers_file << "NoPM" << std::fixed << std::setprecision(1) << "{$" << n_stopped_muons/1e6 << "(" << std::setprecision(0) << (n_stopped_muons_error/1e6)*10 << ") \\times 10^{6}$}" << std::endl;;
      // numbers_file << "\\newcommand\\Sib" << ge_channel << "NCapturedMuons";
      // if (transition_start > 2) {
      //  	numbers_file << "Alternate";
      // }
      // numbers_file << "NoPM" << std::fixed << std::setprecision(1) << "{$" << n_captured_muons/1e6 << "(" << std::setprecision(0) << (n_captured_muons_error/1e6)*10 << ") \\times 10^{6}$}" << std::endl;;

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

      if (savedir != "") {
	std::string savename = savedir + "AlCapData_Si16bDataset_FullXRaySpectrum_" + ge_channel;

	if (transition_start > 2) {
	  savename += "_" + start_str.str() + "1s";
	}
	
	std::string pdfname = savename + ".pdf";
	c_XRaySpectrum->SaveAs(pdfname.c_str());
	std::string pngname = savename + ".png";
	c_XRaySpectrum->SaveAs(pngname.c_str());
      }

    
      TCanvas* c2 = new TCanvas();
      RooHist* pull = Eframe->pullHist();
      RooPlot* pull_frame = (ws->var("edep"))->frame();
      pull_frame->addPlotable(pull, "P");
      pull_frame->Draw(); 
      std::cout << Eframe->chiSquare() << std::endl;
      
    }
  }
  numbers_file << std::endl;
}
